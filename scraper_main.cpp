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
    auto shows = funimation_scraper().get();
    std::ofstream file("funimation.dat", std::ios::trunc | std::ios::binary);
    for(show s : shows) {
        for(const std::string &str : {s.title, s.url, s.description}) {
            uint32_t len = str.size();
            file.write(reinterpret_cast<const char*>(&len), sizeof len);
            file.write(str.c_str(), len);
        }
        uint8_t exclusive = s.subscriber_exclusive;
        file.write(reinterpret_cast<const char*>(&exclusive), sizeof exclusive);
    }
}
