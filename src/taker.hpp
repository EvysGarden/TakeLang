#pragma once

#include "tools.hpp"

#include <any>
#include <libtcc.h>
#include <map>
#include <optional>
#define PY_SSIZE_T_CLEAN
#include <python3.10/Python.h>
#include <set>
#include <string>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

namespace tlg {
    enum Lang { eLangCpp, eLangPython, eLangC };

    struct Snippet final {
        std::string code;
        Lang        lang;

        Snippet() = default;
        Snippet(Lang lang) : lang(lang) {}

        std::string asString() const noexcept
        {
            std::stringstream stream;
            stream << "------\nlang: " << lang << "\ncode: \n" << code;
            return stream.str();
        }
    };

    struct Value final {
        std::string type;
        void*       value;
        size_t      size;
    };

    class Taker final {
        using State = std::variant<TCCState*>;

        std::string                            code;
        std::vector<Snippet>                   snippets;
        std::vector<State>                     states;
        std::unordered_map<std::string, Value> vars;

        void prepareSnippets();
        void runSnippets();
        void runCSnippet(const Snippet&);
        void runPythonSnippet(const Snippet&);

        std::string preparePythonInitVarsCode() const;
        bool        pythonInitialized = false;
        PyObject* catcherModule;

        std::optional<Value> getVar(std::string name, std::string type);

        public:
        Taker(std::filesystem::path path);
    };
} // namespace tlg
