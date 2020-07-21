#ifndef DAEMON_HPP
#define DAEMON_HPP
#include <chrono>

namespace sc_daemon
{
    void create_daemon(std::chrono::system_clock::time_point end_time);
    void remove_daemon();
}

#endif
