#ifndef BASH_CLI_COMMANDS_UTILS_H
#define BASH_CLI_COMMANDS_UTILS_H

#include "basecmd.hpp"

#include <filesystem>
#include <algorithm>
#include <sstream>
#include <string>
#include <utility>
#include <fstream>


namespace commands {

    namespace fs = std::filesystem;

    bool is_file_exist(const fs::path &full_filepath) {
        return fs::exists(full_filepath);
    }

    bool is_readable(const fs::path &filepath) {
        return std::ifstream(filepath).is_open();
    }

    bool is_writable(const fs::path &filepath) {
        return std::ofstream(filepath).is_open();
    }

}


#endif //BASH_CLI_COMMANDS_UTILS_H
