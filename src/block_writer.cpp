// This file contains functions for interfacing with the hosts file.
// That includes writing and removing the block, and making backups before we do
// so.

#include "../include/blocklist.hpp"
#include "../include/block_writer.hpp"
#include <cstdlib>
#include <fstream>
#include <iostream>

using namespace std::string_literals;

std::string const BLOCK_START_MARKER { "# BEGIN BRANIGAN BLOCK"s };
std::string const BLOCK_END_MARKER { "# END BRANIGAN BLOCK"s };


// Writes the contents of the blocklist to the hosts file.
// Delineates the block with BLOCK_START_MARKER and BLOCK_END_MARKER, to make it easier
// to remove it later.
void
block_writer::write_to_hosts
(blocklist::Blocklist blocklist, std::string hosts_path)
{
    std::ofstream ofs {};
    ofs.open(hosts_path, std::ios::app);
    if ( !ofs ) {
        std::cout << "Error -- unable to open hosts file. Did you run as root?\n";
        exit(EXIT_FAILURE);
    }
    ofs << "\n" + BLOCK_START_MARKER + "\n";
    for ( auto x : blocklist ) {
        ofs << "127.0.0.1 " + x + "\n"
            << "::        " + x + "\n";
    }
    ofs << BLOCK_END_MARKER + "\n";
    ofs.close();
}


// Removes the block from the hosts file.
//
// Reads in the host file (line by line) into a vector, ignoring anything
// between the BLOCK_START_MARKER and BLOCK_END_MARKER delimiters.
// Then overwrites the hosts file with the contents of the vector
void block_writer::clear_block(std::string hosts_path)
{
    std::fstream hosts { hosts_path };
    std::vector<std::string> hosts_file_buffer {};
    std::string str {};
    auto within_block { false };
    
    // read the host file into a buffer
    while ( std::getline(hosts, str) ) {
        if ( str == BLOCK_START_MARKER ) {
            within_block = true;
            // when we wrote the block, we added a newline character before.
            // we should now remove it
            hosts_file_buffer.pop_back();
        }
        else if ( within_block == false ) {
            hosts_file_buffer.push_back(str);
        }
        else if ( str == BLOCK_END_MARKER ) {
            within_block = false;
        }
    }
    
    hosts.close();
    hosts.open(hosts_path, std::fstream::out);
    for ( auto x : hosts_file_buffer ) {
        hosts << x + "\n";
    }
    hosts.close();
}


// Creates a backup of the hosts file, e.g hosts.braniganbackup.startblock. 
void block_writer::backup_hosts_file(std::string const current_task)
{
    std::ifstream ifs { "/etc/hosts" };
    if ( !ifs ) {
        std::cout << "Unable to open hosts file. Did you run as root?\n";
        exit(EXIT_FAILURE);
    }
    std::ofstream ofs { "/etc/hosts.braniganbackup" + current_task };
    if ( !ofs ) {
        std::cout << "Unable to create a backup of the hosts file."
                  << "Did you run as root?\n";
        exit(EXIT_FAILURE);
    }
    std::string str {};
    while ( ifs ) {
        getline(ifs, str);
        ofs << str + '\n';
    }
}
