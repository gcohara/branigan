// This contains various utilities used throughout the program, that don't
// really belong elsewhere. Most of these are to do with dealing with end times.

#include "../include/utilities.hpp"
#include "../include/date/date.h"

#include <chrono>
#include <cstdlib>
#include <ostream>
#include <sstream>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <filesystem>


using namespace date;

std::string const utilities::get_default_blocklist_path()
{
    char * homedir_raw { getenv("HOME") };
    if ( homedir_raw == NULL ) {
        std::cout << "Unable to get default blocklist path (couldn't read $HOME)\n"
                  << "Please try running again, but with the path to the blocklist.\n";
        exit(EXIT_FAILURE);
    }
    std::string homedir { homedir_raw };
    homedir += "/.braniganblocklist";
    return homedir;
}

// Converts a time point into a string containing the time in the ISO8601 format
// (YYYY-MM-DDTHH:MM:SS+HHMM)
std::string const utilities::convert_timepoint_to_iso8601
(std::chrono::system_clock::time_point tp)
{
    std::stringstream ss {};
    date::to_stream(ss, "%FT%T%z", tp);
    return ss.str();
}

// Parses in the block ending time argument, and returns it as a time point.
//
// Can accept the block end time in the following formats:
// YYYY-MM-DDTHH:MM:SS+HHMM e.g 2020-01-31T19:30:01+0100
// YYYY-MM-DDTHH:MM+HHMM e.g 2020-01-31T19:30+0100
// YYYY-MM-DDTHH:MM:SS e.g 2020-01-31T19:30:01
// YYYY-MM-DDTHH:MM e.g 2020-01-31T19:30
// HH:MM e.g 18:30 
//
// If the time zone is not provided, we assume the user means the given time
// to be in their computer's current time zone, which we obtain by using 
// tm_gmtoff. This is a little bit naughty, but should be fine.
//
// For the HH:MM format, if that time in the current day has passed, we assume
// the user means that time tomorrow.
std::chrono::system_clock::time_point const
utilities::end_time_parser
(std::string block_end_time)
{
    // we do a naughty thing here - we use a nonstandard element of the
    // tm struct: tm_gmtoff.
    std::istringstream ss { block_end_time };
    auto dt { std::chrono::system_clock::now() };
    auto tm { std::chrono::system_clock::to_time_t(dt) };
    // duration defaults to seconds, so don't need second template argument
    std::chrono::duration<long> time_zone_offset { std::localtime(&tm)->tm_gmtoff };
    // we first try YYYY-MM-DD T HH:MM:SS+HHMM
    if ( date::from_stream(ss, "%F T %T %z", dt) ) {
    }
    // then try YYYY-MM-DD T HH:MM+HHMM
    else if ( ss.clear(), ss.seekg(0); date::from_stream(ss, "%F T %R %z", dt)) {
    }
    // then try YYYY-MM-DD T HH:MM:SS
    // statements from here on will need correcting for local time
    else if ( ss.clear(), ss.seekg(0); date::from_stream(ss, "%F T %T", dt)) {
        dt -= time_zone_offset;
    }
    // then try YYYY-MM-DD T HH:MM
    else if ( ss.clear(), ss.seekg(0); date::from_stream(ss, "%F T %R", dt)) {
        dt -= time_zone_offset;
    }
    // and finally try HH:MM
    else {
        ss.clear();
        ss.seekg(0);
        std::chrono::minutes hm;
        if ( ss >> date::parse("%R", hm) ) {
            auto today { date::floor<days>(dt) };
            dt = sys_days{today} + hm;
            dt -= time_zone_offset;
        }
        else {
            std::cout << "Block end time was given in an incorrect format!\n";
            exit(EXIT_FAILURE);
        }
        // if end time has already passed, we assume the user means tomorrow
        auto difference_between_now_and_given_time
            { std::chrono::duration_cast<std::chrono::minutes>
                    (std::chrono::system_clock::now() - dt).count()};
        if (difference_between_now_and_given_time > 0) {
            std::chrono::duration<long> day { 60*60*24 };
            dt += day;
        }
    }
    return dt;
}

// Checks whether a string is a valid ISO8601 datetime or not.
// That is - is the string in the format YYYY-MM-DDTHH:MM:SS+HHMM,
// for example 2020-01-31T19:00:00+0100
bool const utilities::is_valid_iso8601_datetime(std::string s)
{
    std::istringstream ss { s };
    std::chrono::system_clock::time_point dt {};
    date::from_stream(ss, "%FT%T%z", dt);
    return ss.good();
}

// Tells us whether two files are the same or not.
//
// Returns 0 if (text) files are different, and 1 if they're same
// reads in line by line and compares each line
// should probably send messages to stderr instead
bool const utilities::compare_files(std::string path1, std::string path2)
{
    std::ifstream file1 { path1 };
    std::ifstream file2 { path2 };
    if ( !file1 ) {
        std::cout << "Unable to open " + path1 + "within compare_files.\n";
        if ( !file2 ) {
            std::cout << "Unable to open " + path2 + "within compare_files.\n";
        }
        exit(EXIT_FAILURE);
    }
    std::string temp_str1;
    std::string temp_str2;
    while ( file1 && file2 ) {
        std::getline(file1, temp_str1);
        std::getline(file2, temp_str2);
        if ( temp_str1 != temp_str2 ) {
            std::cout << "Strings are different\n";
            return false;
        }
    }
    bool files_are_same_length { static_cast<bool>(!(file1 || file2)) };
    return files_are_same_length;
}

// Returns the path to the Branigan executable.
std::string const utilities::get_path_to_executable()
{
    int constexpr MAX_EXEC_PATH_LENGTH {10001};
    char buffer[MAX_EXEC_PATH_LENGTH];
    ssize_t len = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
    if ( len != -1 ) {
        buffer[len] = '\0';
        return std::string(buffer);
    }
    std::cout << "Error: Path to the Branigan executable is too long (10,000 character limit).\n"
              << "Please move the executable up a few folders and try again.\n "
              << "Alternatively, post an issue on the GitHub page and I'll fix this for you!\n";
    exit(EXIT_FAILURE);
}

// Returns the path to the folder containing the Branigan executable.
std::string const utilities::get_path_to_executable_folder()
{
    std::filesystem::path exec_path { utilities::get_path_to_executable() };
    return exec_path.parent_path().string();
}
