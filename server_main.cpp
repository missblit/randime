#include <fstream>
#include <random>
#include "rapunzel/fcgi_connection_manager.h"
#include "randime_show.h"

std::vector<std::string> fairy_quotes() {
    return {"Be sure to eat your cabbages.",
            "An OVA in the hand is worth two unreleased.",
            "Everything is going according to keikaku.",
            "Wryyyy",
            "Anime is my body, and randomness is my blood.",
            "For you the day Randime graced your screen was the most important"
            " day of your life. But for me, it was Tuesday.",
            "I picked this randime myself!",
            "\\\(o_o)/",
            "This one reminds me of disco for some reason.",
            "HOOT HOOT HOOT HOOT HOOT HOOT HOOT \\\(0v0)/",
            "My favourite randime is a secret!",
            "My favorite color is yellow.",
            "My favourite food is melons.",
            "One time someone watched a randime and was never seen again...",
            "Be sure to randime responsibly!",
            "I embody all anime that ever was and ever will be",
            "The randime must flow",
            "I am become anime- Destroyer of worlds.",
            "Randime is the little-death that brings total obliteration.",
            "Watching randime can uncover the secrets of existence.",
            "There are rumours of a hidden anime dimension",
            "I am a fairy!",
            "Fairy, faerie, fae, it's all the same to me."
            "Fairies have our own language you see.",
            "All fairies love flowers. But I love anime more.",
            "Quark.",
            "I like mazes",
            "The fairy-realm can be a spooky place.",
            "The fairy-realm doesn't have enough anime.",
            "I learned human speech by watching anime!",
            "One time I watched an anime backwards.",
            "One time I watched every episode of an anime at once.",
            "I have seen infinitely many anime.",
            "I believe that anime can bring people together.",
            "Anime transcends human pettyness",
            "I like to gamble.",
            "Randime Fairy always wins!",
            "I'm sleepy",
            "Remember boys, my heart belongs to randime.",
            "The path to a woman's heart is through her stomach",
            "You should wish upon a shooting star.",
            "You don't always get the randime you want; but sometimes"
            " you get the randime you need.",
            "As a fairy doctor I recommend randime at least once a week.",
            "A randime by any other name would be as l33t",
            "Have you ever gone to an anime convention?",
            "I like to cosplay as anime fairies.",
            "I'm as old as the oldest anime, because I am randime incarnate",
            "I've seen anime you people wouldn't believe.",
            "Fiaries do exist",
            "My favorite fairy tale is \"The Little Mermaid\"",
            "From all the anime in existance, this is the one you must watch",
            "If nobody watches randime I'll cease to exist",
            "...",
            "baka",
            "I don't like scary stories.",
            "Some anime has scarred me.",
            "Some anime is absolutely beautiful.",
            "There's no such thing as an anime that isn't important in it's own"
            " way",
            "Even bad anime have their special place in the universe.",
            "I believe in other dimensions",
            "I believe that there's a perfect anime for everyone.",
            "Some people do not like anime. I don't understand but that is OK.",
            "The whole problem with the world is that fools and fanatics are "
            " always so certain of the best anime.",
            "Watch in the name of God, ye not guilty.",
            "Sometimes I like sci-fi anime but sometimes I like fantasy anime.",
            "I have my own fansubbing group, but it's a secret.",
            "All that is necessary for the triumph of bad anime is that good"
            " men watch nothing.",
            "They that can give up good anime to obtain a little temporary"
            " fan-service deserve neither good anime nor fan-service.",
            "In space no one can hear you squee.",
            "I am tiny! please do not try to hug me.",
            "Some of my favourite conversations were with trees and flowers.",
            "Plants have a better sense of anime pacing than you impatient"
            " humans.",
            "Besides anime I also like Star-Trek.",
            "Randime is lovely, dark and deep. But I have promises to keep."
            " And anime to watch before I sleep.",
            "No rest for the wicked.",
            "Randime is a great substitute for medidation.",
            "A randime is never late, nor is it early, it arrives precicely"
            " when it means to.",
            "It is a moral imperative to watch good anime.",
            "I don't like wizards.",
            "You humans don't know what you're missing. Wings are great.",
            "I use a QWERTY keyboard. A tiny one!",
            "Give me a hard-drive and I'll fill it.",
            "Watch, or do not. There is no 'later'.",
            "May the randime be with you",
            "Do you know how to watlz?",
            "A woman needs a randime like a fish needs water."};
}

int main() {
    std::random_device rd;
    std::mt19937 rng(rd());
    
    auto quotes = fairy_quotes(); 
    std::ifstream funi_db("/var/www/cgi-bin/funimation.dat");
    auto funi_shows = load_shows(funi_db);
    std::ifstream cr_db("/var/www/cgi-bin/crunchyroll.dat");
    auto cr_shows = load_shows(cr_db);
	if(funi_shows.empty()|| cr_shows.empty())
		throw std::runtime_error("Could not load show files");
    
    fcgi::connection_manager fcgi;
    
    std::uniform_int_distribution<> dist(0, funi_shows.size()-1);
    std::uniform_int_distribution<> q_dist(0, quotes.size()-1);
    while(true) {
        fcgi::request r = fcgi.get_request();
        auto qs = decode_querystring(r.parameter("QUERY_STRING"));

		//decide which sources to consider 
		bool funi = qs["funimation"] == "true";
		bool crunchy = qs["crunchyroll"] == "true";
		std::vector<std::vector<show>*> sources;
		int count = funi + crunchy;
		if(funi || !count)
			sources.push_back(&funi_shows);
		if(crunchy || !count)
			sources.push_back(&cr_shows);

		//randomly select the show
		std::uniform_int_distribution<> source_dist(0,sources.size()-1);
		std::vector<show> &source = *sources[source_dist(rng)];
		std::uniform_int_distribution<> show_dist(0,source.size()-1);
		show &s = source[show_dist(rng)];
		
		//output the chosen show
        r << "Content-type: text/html; charset=UTF-8\r\n\r\n"
             "<!DOCTYPE html>\n";
        r << "<title>" << s.title << "</title>\n";
        r << "<p>The gods of Randime have spoken \\o_o/<br>\n"
             "   You must watch:<br>\n";
        r << "<h1><a style=\"text-decoration:none;\" href=\""
		  << s.url << "\">" << s.title << "</a></h1>\n";
        r << "<p>Randime Fairy says:<br>\n"
             "<blockquote>" << s.description << "</blockquote></p>\n";
		//output a saying from the randime fairy too
        r << "<p><small>Randime fairy also says: " << quotes[q_dist(rng)]
          << "</small></p>\n";
    }
}
