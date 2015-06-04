#include <sstream>
#include <memory>
#include <vector>
#include <string>
#include <set>
#include <iterator>
#include <iostream>
#include <deque>
#include <curl/curl.h>
#include "gumbo_util.h"

size_t curl_write_cb(void *ptr, size_t size, size_t nmemb, void *userdata) {
    std::string &out = *static_cast<std::string*>(userdata);
    const char *bytes = static_cast<const char *>(ptr);
    out.insert(std::end(out), bytes, bytes+size*nmemb);
    return size*nmemb;
}

std::string download_url(const std::string &url) {
    std::string out;
    CURL *curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL,           url.c_str()  );
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_write_cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA,     &out         );
    auto res = curl_easy_perform(curl);
    if(res != CURLE_OK)
        throw std::runtime_error("curl_easy_perform failed");
    curl_easy_cleanup(curl);
    return out;
}


//gumbo selector
gumbo_selector::~gumbo_selector() {};

//gumbo multi selector
gumbo_multi_selector::gumbo_multi_selector() {}
 
void gumbo_multi_selector::push(std::unique_ptr<gumbo_selector> &&ptr) {
    selectors.emplace_back(std::move(ptr));
}
bool gumbo_multi_selector::matches(const GumboNode &n) const {
    for(const auto &ptr : selectors) {
        if(!ptr->matches(n))
            return false;
    }
    return true;
}

//gumbotag selector
gumbotag_selector::gumbotag_selector(GumboTag t) : t(t) {}

bool gumbotag_selector::matches(const GumboNode &n) const {
    if(n.type != GUMBO_NODE_ELEMENT)
        return false;
    return n.v.element.tag == t;
}

//gumboattribute_selector
gumboattribute_selector::gumboattribute_selector(const std::string &name)
: name(name), match_value(false)
{}

 gumboattribute_selector::gumboattribute_selector(const std::string &name,
                                                  const std::string &value)
: name(name), value({value}), match_value(true)
{}

 gumboattribute_selector::gumboattribute_selector(
     const std::string &name,
    const std::vector<std::string> &value)
: name(name), value(value), match_value(true)
{}

bool  gumboattribute_selector::matches(const GumboNode &n) const {
    if(n.type != GUMBO_NODE_ELEMENT)
        return false;
    const GumboVector &attr_v = n.v.element.attributes;
    GumboAttribute *attr = gumbo_get_attribute(&attr_v, name.c_str());
    if(!attr)
        return false;
    if(!match_value)
        return true;
    std::stringstream ss(attr->value);
    std::set<std::string> attrs((std::istream_iterator<std::string>(ss)),
                                    std::istream_iterator<std::string>());
    auto count = attrs.size();
    attrs.insert(std::begin(value), std::end(value));
    return attrs.size() == count;
}

std::vector<GumboNode *> gumbo_find_all(GumboNode *root,
                                        const gumbo_selector &s) {
    if(root == nullptr)
        return {};    
    std::vector<GumboNode *> res;
    std::deque<GumboNode *> stack = {root};
    while(!stack.empty()) {
        GumboNode *n = stack.front();
        stack.pop_front();
        if(s.matches(*n)) {
            res.push_back(n);
        }
        GumboVector *children;
        switch(n->type) {
            case GUMBO_NODE_DOCUMENT:
                children = &n->v.document.children;
                break;
            case GUMBO_NODE_ELEMENT:
                children = &n->v.element.children;
                break;
            default:
                continue;
        }
        for(std::size_t i = 0; i < children->length; i++)
            stack.push_back(static_cast<GumboNode*>(children->data[i]));
    }
    return res;
}

GumboNode *gumbo_find_first(GumboNode *root, const gumbo_selector &s) {
    auto v = gumbo_find_all(root,s);
    if(v.empty())
        return nullptr;
    return v[0];
}

std::string gumbo_text(GumboNode *node) {
    if(!node)
        throw std::runtime_error("Could not scrape text");
    GumboNode* text_node =
        static_cast<GumboNode*>(node->v.element.children.data[0]); 
    if(!(text_node->type == GUMBO_NODE_TEXT))
        throw std::runtime_error("Could not scrape text");
    return text_node->v.text.text;
}

std::string gumbo_value(GumboNode *n, const std::string &attr) {
    if(n->type != GUMBO_NODE_ELEMENT)
        throw std::runtime_error("Could not scrape value");
    const GumboVector &attr_v = n->v.element.attributes;
    GumboAttribute *gumbo_attr= gumbo_get_attribute(&attr_v, attr.c_str());
    return gumbo_attr->value;
}