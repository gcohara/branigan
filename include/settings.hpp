#ifndef SETTINGS_HPP
#define SETTINGS_HPP 0
#include <string>
#include "utilities.hpp"

namespace settings {
    std::string const static SETTINGS_PATH
        { utilities::get_path_to_executable_folder() + "/../.resources/branigan-settings" };
    void write_ending_time_to_settings(std::string time);
    std::string retrieve_end_time();
}

#endif
