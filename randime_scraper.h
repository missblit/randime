#ifndef MBLIT_RANDIME_SCRAPER_H
#define MBLIT_RANDIME_SCRAPER_H

#include "randime_show.h"

/** interface for randime website scrapers */
class scraper {
public:
	/** return a vector of shows */
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