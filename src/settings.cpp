// Contains functions for interfacing with the settings file.
// The path to the settings file is kept in the settings.hpp header.

#include "../include/settings.hpp"
#include "../include/utilities.hpp"
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <filesystem>

// Writes the block ending time to the settings file.
void settings::write_ending_time_to_settings(std::string time)
{
    std::ofstream ofs { SETTINGS_PATH };
    std::cout << SETTINGS_PATH << "\n";
    if ( !ofs ) {
        std::cout << "Unable to write to settings. Did you run as root?\n";
    }
    ofs << time;
    ofs.close();
}

// Retrieves the block ending time from the settings file.
// Includes a check to make sure this is a valid format.
std::string settings::retrieve_end_time()
{
    std::ifstream ifs { settings::SETTINGS_PATH };
    if ( !ifs ) {
        std::cout << "Unable to open settings file.\n";
        std::cout << std::filesystem::current_path().string() + "\n";
        exit(EXIT_FAILURE);
    }
    std::string temp {};
    std::getline(ifs, temp);
    if ( utilities::is_valid_iso8601_datetime(temp) ) {
        return temp;
    }
    else {
        std::cout << "The end time found in the settings file is not valid.\n"
                  << "If you're seeing this message, something bad has happened!\n"
                  << "To clear the block you should change the first line "
                  << "of the settings file (" + SETTINGS_PATH + ") to:\n "
                  << "2020-01-01T12:00:00+0000\n"
                  << "and then run 'branigan --checkup'.\n";
        exit(EXIT_FAILURE);
    }
}
