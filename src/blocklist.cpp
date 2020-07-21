#include "../include/blocklist.hpp"
#include <iostream>
#include <fstream>

// Reads the blocklist into a Blocklist variable and returns it.
blocklist::Blocklist blocklist::get_blocklist(std::string path_to_blocklist)
{
    std::ifstream ifs { path_to_blocklist };
    if ( !ifs ) {
        std::cout << "ERROR -- Blocklist at " + path_to_blocklist
                  << "could not be opened.\n";
        exit(EXIT_FAILURE);
    }
    
    blocklist::Blocklist blocklist {};
    while ( ifs ) {
        Url str {};
        if ( getline(ifs, str) ) {
            blocklist.push_back(str);
        }
    }
    return blocklist;
}
