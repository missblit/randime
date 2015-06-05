#include <string>
#include <vector>
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <iterator>
#include <memory>
#include <sstream>
#include <set>
#include <deque>
#include <thread>
#include <chrono>
#include <initializer_list>
#include <gumbo.h>
#include <curl/curl.h>
#include "gumbo_util.h"
#include "randime_show.h"
#include "randime_scraper.h"

int main() {
    auto crunchy_shows = crunchyroll_scraper().get();
    auto funi_shows  = funimation_scraper().get();
    std::ofstream funi_f("funimation.dat", std::ios::trunc | std::ios::binary);
    std::ofstream cr_f("crunchyroll.dat", std::ios::trunc | std::ios::binary);
	if(!funi_shows.empty())
		for(show s : funi_shows)
			s.save(funi_f);
	else
		std::cout << "Could not scrape funimation shows\n";
	if(!crunchy_shows.empty())
		for(show s : crunchy_shows)
			s.save(cr_f);
	else
		std::cout << "Could not scrape crunchyroll shows\n";
}
