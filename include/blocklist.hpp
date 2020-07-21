#ifndef BLOCKLIST_HPP
#define BLOCKLIST_HPP 0

#include <vector>
#include <string>

namespace blocklist {
    typedef std::string Url;
    typedef std::vector<Url> Blocklist;

    Blocklist get_blocklist(std::string path_to_blocklist);
}
#endif 
