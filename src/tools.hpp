#pragma once

#include <filesystem>
#include <fstream>
#include <sstream>

namespace tlg {
    inline std::string readFile(std::filesystem::path path)
    {
        std::ifstream     file { path };
        std::stringstream strbuf;
        if (file.is_open()) { strbuf << file.rdbuf(); }
        return strbuf.str();
    }
} // namespace tlg