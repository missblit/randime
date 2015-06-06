#ifndef MBLIT_RANDIME_SHOW_H
#define MBLIT_RANDIME_SHOW_H

#include <string>
#include <vector>
#include <stdexcept>
/** show record for keeping track of an anime show and it's URL and stuff */
struct show {
	/** The title of the show */
    std::string title;
	/** The full URL of the show */
    std::string url;
	/** The scraped description of the show */
    std::string description;
	/** Whether or not the show is a subscriber exclusive
	 *  (but this needs more work)
	 */
    bool subscriber_exclusive;
	
    /** Load show from binary input stream
	 */
    std::istream &load(std::istream &is) {
        title.clear();
        url.clear();
        description.clear();
        for(std::string *s : {&title, &url, &description}) {
            uint32_t len;
            is.read(reinterpret_cast<char*>(&len), sizeof len);
            if(!is)
                return is;
            std::vector<char> buff(len);
            is.read(buff.data(), len);
            s->insert(std::end(*s), std::begin(buff), std::end(buff));
        }
        uint8_t exclusive;
        is.read(reinterpret_cast<char*>(&exclusive), sizeof exclusive);
        return is;
    }
    
    /** Write show to binary output stream */
    std::ostream &save(std::ostream &os) {
		for(const std::string &str : {title, url, description}) {
            uint32_t len = str.size();
            os.write(reinterpret_cast<const char*>(&len), sizeof len);
            os.write(str.c_str(), len);
        }
        uint8_t exclusive = subscriber_exclusive;
        os.write(reinterpret_cast<const char*>(&exclusive), sizeof exclusive);
		return os;
	}
};

/** Load a collection of shows from binary input stream */
inline std::vector<show> load_shows(std::istream &is) {
    std::vector<show> shows;
    show s;
    while(s.load(is))
        shows.push_back(s);
    return shows;
}

/** Comparator for shows (so they can be put in a set) */
inline bool operator<(const show &a, const show &b) {
    if(a.title       != b.title)       return a.title       < b.title;
    if(a.url         != b.url)         return a.url         < b.url;
    if(a.description != b.description) return a.description < b.description;
    return a.subscriber_exclusive < b.subscriber_exclusive;
}

#endif