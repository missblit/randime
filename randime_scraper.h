#ifndef MBLIT_RANDIME_SCRAPER_H
#define MBLIT_RANDIME_SCRAPER_H

#include "randime_show.h"

class scraper {
public:
    virtual std::vector<show> get() = 0;
};

class funimation_scraper : public scraper {
public:
    std::vector<show> get();
};

class crunchyroll_scraper : public scraper {
public:
    std::vector<show> get();
};
    

#endif