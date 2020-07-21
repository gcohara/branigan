
#ifndef HOSTS_WRITER_HPP
#define HOSTS_WRITER_HPP 0

#include "../include/blocklist.hpp"

using namespace std::string_literals;
namespace block_writer {
    void write_to_hosts(blocklist::Blocklist blocklist, std::string hosts_path = "/etc/hosts"s);
    void backup_hosts_file(std::string current_task);
    void clear_block(std::string hosts_path = "/etc/hosts"s);
}
#endif
