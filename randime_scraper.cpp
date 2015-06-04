#include <vector>
#include <memory>
#include <string>
#include <iostream>
#include <thread>
#include <chrono>
#include <set>
#include <regex>
#include "gumbo_util.h"
#include "randime_show.h"
#include "randime_scraper.h"

show retrieve_funimation_show(GumboNode *item_cell) {
    show s;
    //Find the URL
    {
        gumbo_multi_selector selector;
        selector.push(std::make_unique<gumbotag_selector>(GUMBO_TAG_A));
        selector.push(std::make_unique<gumboattribute_selector>(
            "class", "item-title"
        ));
        GumboNode *title_node = gumbo_find_first(item_cell, selector);
        s.url = gumbo_value(title_node, "href");
     }
    //find the title
    {
        gumbo_multi_selector selector;
        selector.push(std::make_unique<gumbotag_selector>(GUMBO_TAG_DIV));
        selector.push(std::make_unique<gumboattribute_selector>(
            "class", "thumb"
        ));
        GumboNode *thumb_node = gumbo_find_first(item_cell, selector);
        
        gumbo_multi_selector img_selector;
        img_selector.push(std::make_unique<gumbotag_selector>(GUMBO_TAG_IMG));
        GumboNode *img_node = gumbo_find_first(thumb_node, img_selector);
        s.title = gumbo_value(img_node, "alt");
    }
    //find the description
    {
        gumbo_multi_selector selector;
        selector.push(std::make_unique<gumbotag_selector>(GUMBO_TAG_DIV));
        selector.push(std::make_unique<gumboattribute_selector>(
            "class", "summary-short"
        ));   
        GumboNode* desc_node = gumbo_find_first(item_cell, selector);
        s.description = gumbo_text(desc_node);
    }
    //find the subscriber exclusiveness
    {
        gumbo_multi_selector selector;
        selector.push(std::make_unique<gumbotag_selector>(GUMBO_TAG_SPAN));
        selector.push(std::make_unique<gumboattribute_selector>(
            "class", "e"
        ));   
        GumboNode* sub_node = gumbo_find_first(item_cell, selector);
        s.subscriber_exclusive = sub_node;
    }
    return s;
}
std::vector<show> retrieve_funimation_shows() {
    bool done = false;
    int offset = 0;
    std::set<show> shows; //needs to be set in-case of duplicates
    while(!done) {
        std::string offset_str = std::to_string(offset);
        std::string page = "http://www.funimation.com/shows?offset=";
        page += offset_str;
        
        std::cout << "Querying " << page << "\n";
        std::string html = download_url(page);
        GumboOutput *output = gumbo_parse(html.c_str());

        //first find <ul class="container items-row">
        GumboNode *content_module;
        {
            gumbo_multi_selector s;
            s.push(std::make_unique<gumbotag_selector>(GUMBO_TAG_UL));
            s.push(std::make_unique<gumboattribute_selector>(
                "class",
                std::vector<std::string>{"container","items-row"}
            ));
            content_module = gumbo_find_first(output->root, s);
        }
        
        //next find all the item cells for individual shows
        std::vector<GumboNode *> cells;
        {
            gumbo_multi_selector s;
            s.push(std::make_unique<gumbotag_selector>(GUMBO_TAG_LI));
            s.push(std::make_unique<gumboattribute_selector>(
                "class","item-cell"
            ));
            cells = gumbo_find_all(content_module, s);        
        }
        
        //next scrape the show information
        for(auto &c : cells) {
            auto s = retrieve_funimation_show(c);
            shows.insert(s);
        }
        gumbo_destroy_output(&kGumboDefaultOptions, output);
        
        //sleep a bit after every page retrieval
        std::cout << "Retrieved " << cells.size() << " shows...\n";
        done = cells.size() < 20;
        if(!done)
            std::this_thread::sleep_for(std::chrono::seconds(5));

        offset += cells.size();
    }
    return std::vector<show>(std::begin(shows), std::end(shows));
}

std::vector<show> funimation_scraper::get() {
    return retrieve_funimation_shows();
}

std::vector<show> crunchyroll_scraper::get() {
	std::set<show> shows;
	std::string page = "http://www.crunchyroll.com/"
	                   "videos/anime/alpha?group=all";
	std::string html = download_url(page);
	GumboOutput *output = gumbo_parse(html.c_str());

	//first find <ul class="container items-row">
	GumboNode *container_node;
	{
		gumbo_multi_selector content_s;
		content_s.push(std::make_unique<gumbotag_selector>(GUMBO_TAG_DIV));
		content_s.push(std::make_unique<gumboattribute_selector>(
			"class", "videos-column-container"
		));
		container_node = gumbo_find_first(output->root, content_s);
	}
	
	std::string regex_str = R"str(\("#media_group_([[:digit:]]+)"\)\.data\('bubble_data', \{"name":"([^"]*)","description":"(([^"]|\\")*)")str";
	std::regex regex(regex_str, std::regex_constants::extended);

	std::sregex_iterator it(html.begin(), html.end(), regex);
	for(; it != std::sregex_iterator(); it++) {
		std::string media_group = (*it)[1];
		std::string name        = (*it)[2];
		std::string description = (*it)[3];
		
		/* Find the URL */
		gumbo_multi_selector group_s;
		group_s.push(std::make_unique<gumbotag_selector>(GUMBO_TAG_LI));
		group_s.push(std::make_unique<gumboattribute_selector>(
			"id", (std::string("media_group_") + media_group)
		));
		GumboNode *group_node = gumbo_find_first(container_node, group_s);
		
		gumbo_multi_selector show_s;
		show_s.push(std::make_unique<gumbotag_selector>(GUMBO_TAG_A));
		show_s.push(std::make_unique<gumboattribute_selector>(
			"token", "shows-portraits"
		));
		GumboNode *show_node = gumbo_find_first(group_node, show_s);
		
		//Can't do anything without the show's URL
		if(!show_node)
			continue;
		
		std::string url =   std::string("http://www.crunchyroll.com")
		                  + gumbo_value(show_node, "href");
						  
		show s;
		s.title = name;
		s.url = url;
		s.description = description;
		
		/* todo: it's too hard to detect this so make it optional in the struct
		 */
		s.subscriber_exclusive = false;
		shows.insert(s);
	}
    return std::vector<show>(std::begin(shows), std::end(shows));
}
