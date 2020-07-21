// This file contains functions used for installing and removing a daemon that
// uninstalls the block, and checks on it every so often.
// This is currently reliant on systemd. This shouldn't be an issue, seeing as
// it's used on all distros except a few enthusiast ones.

#include <chrono>
#include <cstdlib>
#include <iostream>
#include <filesystem>
#include <fstream>
#include "../include/daemon.hpp"
#include "../include/date/date.h"
#include "../include/utilities.hpp"

using namespace std::string_literals;

std::string const static SYSTEM_D_UNIT_PATH {"/etc/systemd/system/"};
std::string const static LOAD_DAEMON_INTO_SYSD
{"systemctl enable branigan.service && "
        "systemctl start branigan.timer"};
std::string const static REMOVE_DAEMON_FROM_SYSD
{"systemctl stop branigan.timer && "
        "systemctl disable branigan.service"};
std::string const static DELETE_DAEMON_FILES
{"rm /etc/systemd/system/branigan.timer "
        "/etc/systemd/system/branigan.service"};

// Creates two files: a systemd timer and a systemd serivce. The timer runs
// the service: five seconds after boot, every fifteen minutes, and at the scheduled
// block end time.
//
// The service runs braningan --checkup.
void sc_daemon::create_daemon(std::chrono::system_clock::time_point end_time)
{
    // create the timer daemon
    std::ofstream ofs_t { SYSTEM_D_UNIT_PATH + "branigan.timer" };
    // Parse end time into a string stream
    std::stringstream end_time_ss {};
    date::to_stream(end_time_ss, "%F %R:00 UTC", end_time);
        
    ofs_t << "[Unit]\n"
          << "Description=\"Daemon for Branigan\"\n\n"
          << "[Timer]\n"
          << "OnBootSec=5s\n"
          << "OnUnitActiveSec=15min\n"
          << "OnCalendar=" << end_time_ss.str() << "\n";
    ofs_t.close();
        
    // now create the service itself
    std::ofstream ofs_s { SYSTEM_D_UNIT_PATH + "branigan.service" };
    ofs_s << "[Unit]\n"
          << "Description=\"Daemon for Branigan\"\n\n"
          << "[Service]\n"
          << "ExecStart=" <<  utilities::get_path_to_executable() << " --checkup\n\n"
        // why was this next line necessary again??...
          << "WorkingDirectory=" + std::filesystem::current_path().string() + "\n"
          << "[Install]\n"
          << "WantedBy=multi-user.target\n";
    ofs_s.close();
        
    // finally, enable the service and start the timer
    std::system(LOAD_DAEMON_INTO_SYSD.c_str());
}

// Stops the systemd timer, disables the service, and then deletes the
// associated files in /etc/systemd/system/.
void sc_daemon::remove_daemon()
{
    std::system(REMOVE_DAEMON_FROM_SYSD.c_str());
    std::system(DELETE_DAEMON_FILES.c_str());
}
