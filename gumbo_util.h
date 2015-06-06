#ifndef MBLIT_GUMBO_UTIL_H
#define MBLIT_GUMBO_UTIL_H

#include <string>
#include <memory>
#include <vector>
#include <gumbo.h>

/** Download the contents of a URL into a string */
std::string download_url(const std::string &url);

/**
 * Some quick and dirty HTML matching classes for searching through 
 * a tree of gumbo nodes
 * 
 * these aren't pretty at all, but work for this problem */
class gumbo_selector {
public:
    virtual bool matches(const GumboNode &n) const = 0;
    virtual ~gumbo_selector();
};

/** Select if ALL child selectors also match */
class gumbo_multi_selector : public gumbo_selector {
    std::vector<std::unique_ptr<gumbo_selector>> selectors;
public:
    gumbo_multi_selector();
    
    void push(std::unique_ptr<gumbo_selector> &&ptr);
    virtual bool matches(const GumboNode &n) const;
};

/** Match the tag type */
class gumbotag_selector : public gumbo_selector {
    GumboTag t;
public:
    gumbotag_selector(GumboTag t);
    virtual bool matches(const GumboNode &n) const;
};

/** match one or more parts of an attribute (space delimited) */
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

/** Find every gumbo node that is matched by the selector */
std::vector<GumboNode *> gumbo_find_all(GumboNode *root,
                                        const gumbo_selector &s);
/** Find the first gumbo node that is matched by the selector
 * or null if none was found */
GumboNode *gumbo_find_first(GumboNode *root, const gumbo_selector &s);

/** Extract the text from a Gumbonode that contains text */
std::string gumbo_text(GumboNode *node);
/** Extract an attributes value from a gumbonode that contains an attribute */
std::string gumbo_value(GumboNode *node, const std::string &attr);

#endif