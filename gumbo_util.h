#ifndef MBLIT_GUMBO_UTIL_H
#define MBLIT_GUMBO_UTIL_H

#include <string>
#include <memory>
#include <vector>
#include <gumbo.h>


std::string download_url(const std::string &url);


/* selector to match nodes when looking through HTML tree */
class gumbo_selector {
public:
    virtual bool matches(const GumboNode &n) const = 0;
    virtual ~gumbo_selector();
};

class gumbo_multi_selector : public gumbo_selector {
    std::vector<std::unique_ptr<gumbo_selector>> selectors;
public:
    gumbo_multi_selector();
    
    void push(std::unique_ptr<gumbo_selector> &&ptr);
    virtual bool matches(const GumboNode &n) const;
};

class gumbotag_selector : public gumbo_selector {
    GumboTag t;
public:
    gumbotag_selector(GumboTag t);
    virtual bool matches(const GumboNode &n) const;
};
class gumboattribute_selector : public gumbo_selector {
    std::string name;
    std::vector<std::string> value;
    bool match_value;
public:
    gumboattribute_selector(const std::string &name);
    gumboattribute_selector(const std::string &name, const std::string &value);
    gumboattribute_selector(const std::string &name,
                            const std::vector<std::string> &value);
    virtual bool matches(const GumboNode &n) const;
};

std::vector<GumboNode *> gumbo_find_all(GumboNode *root,
                                        const gumbo_selector &s);
GumboNode *gumbo_find_first(GumboNode *root, const gumbo_selector &s);

std::string gumbo_text(GumboNode *node);
std::string gumbo_value(GumboNode *node, const std::string &attr);

#endif