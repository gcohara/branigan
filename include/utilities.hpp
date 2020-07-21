#ifndef UTILITIES_HPP
#define UTILITIES_HPP 0

#include <string>
#include <chrono>

namespace utilities {
    bool const compare_files(std::string path1, std::string path2);
    std::chrono::system_clock::time_point const datetime_parser(std::string iso8601_datetime);
    bool const is_valid_iso8601_datetime(std::string s);
    std::chrono::system_clock::time_point const end_time_parser(std::string block_end_time);
    std::string const convert_timepoint_to_iso8601(std::chrono::system_clock::time_point tp);
    std::string const get_path_to_executable();
    std::string const get_path_to_executable_folder();
    std::string const get_default_blocklist_path();
}

#endif
