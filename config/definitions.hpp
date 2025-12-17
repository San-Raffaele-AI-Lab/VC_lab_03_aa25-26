#ifndef DEFINITIONS_HPP
#define DEFINITIONS_HPP
#include <filesystem>
#include <source_location>

namespace fs = std::filesystem;

fs::path get_file_path() {
    const auto& location = std::source_location::current();
    fs::path source_file_path = location.file_name();

    fs::path absolute_path = fs::absolute(source_file_path);
    return absolute_path;
}

fs::path ROOT_DIR = get_file_path().parent_path().parent_path();
#endif
