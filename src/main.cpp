#include <chrono>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <filesystem>
#include "../include/blocklist.hpp"
#include "../include/block_writer.hpp"
#include "../include/daemon.hpp"
#include "../include/date/date.h"
#include "../include/utilities.hpp"
#include "../include/settings.hpp"

using namespace std::string_literals;
using namespace date;

bool block_has_ended();
void print_documentation();
void
install_block
(std::string blocklist_path, std::chrono::system_clock::time_point ending_time);


int main(int argc, char * argv[])
{
    // Given enough args? No, print documentation!
    if ( argc < 2 ) { 
        print_documentation();
    }
    
    // The install option takes an optional path to a blocklist (and defaults to
    // default_blocklist_path if this isn't given) and a datetime or duration.
    // the datetime/duration is converted to an ISO 6801 datetime in the format
    // YYYY-MM-DDTHH:MM:SS+HHMM, and this is used to install the block.
    else if ( argv[1] == "--install"s ) {
        if ( argc != 4 && argc != 3 ) {
            std::cout << "Incorrect number of arguments to install block.\n";
            print_documentation();
            exit(EXIT_FAILURE);
        }
        std::string blocklist_path;
        std::string end_time_or_duration;
        if ( argc == 4 ) {
            blocklist_path = argv[2];
            end_time_or_duration = argv[3];
        }
        else if ( argc == 3 ) {
            blocklist_path = utilities::get_default_blocklist_path();
            end_time_or_duration = argv[2];
        }
        auto const end_time{ utilities::end_time_parser(end_time_or_duration) };
        // need to add in something to check we are root
        install_block(blocklist_path, end_time);        
    }

    // This is run periodically by the systemd daemon, and is also needed in
    // case the daemon fails to clear the block for some reason.
    // Eventually, this should check for tampering, and reinstall the block if
    // it's detected.
    else if ( argv[1] == "--checkup"s ) {
        std::cout << "Performing checkup!\n";
        if ( block_has_ended() ) {
            // should also check if the block is even active
            block_writer::backup_hosts_file( "stop"s );
            block_writer::clear_block();
            sc_daemon::remove_daemon();
            std::cout << "The block has finished, and has been removed.\n";
        }
        else {
            std::cout << "The block is still in place.\n";
        }
    }

    else {
        std::cout << "Invalid argument.\n";
        print_documentation();
        exit(EXIT_FAILURE);
    }
    
}


// Installs the block.
// First, writes end time to the settings file.
// Then backs the hosts file up, then writes to the host file, rerouting sites
// in the blocklist so they're inaccessible. 
// Finally, creates the systemd service and timer that will clear the block
// when the end time comes.
void
install_block
(std::string const blocklist_path, std::chrono::system_clock::time_point const ending_time)
{
    blocklist::Blocklist blocklist { blocklist::get_blocklist(blocklist_path) };
    auto end_time_string { utilities::convert_timepoint_to_iso8601(ending_time)};
    settings::write_ending_time_to_settings(end_time_string);
    block_writer::backup_hosts_file( "start"s );
    block_writer::write_to_hosts(blocklist);
    sc_daemon::create_daemon(ending_time);
    std::cout << "Block has been activated.\n";
}



// Checks whether the block has ended.
// Retrieves the end time from settings, then compares it to current time.
// Returns true if end time before current time, and false if not.
bool block_has_ended()
{
    std::string const temp { settings::retrieve_end_time() };
    auto const end_time { utilities::end_time_parser(temp) };
    auto const current_time { std::chrono::system_clock::now() };
    auto const time_until_end
        { std::chrono::duration_cast<std::chrono::minutes>
          (end_time - current_time).count() };
    
    if ( time_until_end > 0 ) {
        return false;
    }
    else {
        return true;
    }

}

void print_documentation()
{
    std::cout << "Documentation goes here \n";
}
