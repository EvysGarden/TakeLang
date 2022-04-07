#include "taker.hpp"
#include "libtcc.h"
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <sstream>
#include <variant>

namespace tlg {
    Taker::Taker(std::filesystem::path path)
    {
        code = readFile(path);
        prepareSnippets();
        runSnippets();
    }

    std::optional<Value> Taker::getVar(std::string name, std::string type)
    {
        if (vars.contains(name)) return vars.at(name);

        for (const auto& state : states) {
            if (std::holds_alternative<TCCState*>(state)) {
                if (auto value = tcc_get_symbol(std::get<TCCState*>(state), name.c_str())) {
                    return vars.emplace(name, Value { type, value }).first->second;
                }
            }
        }

        return std::nullopt;
    }

    void Taker::prepareSnippets()
    {
        std::stringstream stream { code };

        for (std::string s; !stream.eof();) {
            std::getline(stream, s);

            if (s.starts_with(':')) {
                auto lang = s.substr(1);
                if (lang == "c") {
                    snippets.emplace_back(eLangC);
                } else if (lang == "python") {
                    snippets.emplace_back(eLangPython);
                } else if (lang == "cpp") {
                    snippets.emplace_back(eLangCpp);
                }
                continue;
            }

            snippets.back().code.append(s).append("\n");
        }
    }

    void Taker::runCSnippet(const Snippet& snippet)
    {
        TCCState* state = tcc_new();
        if (!state) {
            std::cout << "tccstate init failed" << std::endl;
            return;
        }

        // compile snippet
        tcc_set_output_type(state, TCC_OUTPUT_MEMORY);
        if (tcc_compile_string(state, snippet.code.c_str()) == -1) {
            std::cout << "something went wrong while compiling C code" << std::endl;
            return;
        }

        // push back state
        states.push_back(state);

        // set an error function or something, idk
        tcc_set_error_func(state, stderr, [](void* opaque, const char* msg) {
            std::fprintf((FILE*)opaque, "%s\n", msg);
        });

        // provide symbols
        {
            std::stringstream stream { snippet.code };

            for (std::string str; !stream.eof();) {
                stream >> str;
                if (str == "extern") {
                    std::string type, name;
                    stream >> type;
                    stream >> name;

                    if (name == "*") {
                        type.append("*");
                        stream >> name;
                    }
                    while (name.starts_with("*")) {
                        type.append("*");
                        name = name.substr(1);
                    }
                    if (name.ends_with(';')) { name = name.substr(0, name.size() - 1); }
                    if (auto value = getVar(name, type)) {
                        tcc_add_symbol(state, name.c_str(), value->value);
                    } else {
                        std::cout << "value " << name << " not found" << std::endl;
                    }
                }
            }
        }

        if (tcc_relocate(state, TCC_RELOCATE_AUTO) < 0) {
            std::cout << "couldn't relocate code" << std::endl;
            return;
        }

        // run (for your life)

        int (*fun)() = (int (*)())tcc_get_symbol(state, "run");
        if (fun) { fun(); }
    }

    std::string Taker::preparePythonInitVarsCode() const
    {
        std::stringstream res;
        for (const auto& var : vars) {
            res << var.first << " = ";
            if (var.second.type == "int") {
                res << *(int*)var.second.value << '\n';
                continue;
            }
            if (var.second.type == "string") {
                res << *(std::string*)var.second.value << '\n';
                continue;
            }
            if (var.second.type == "char") {
                res << *(char*)var.second.value << '\n';
                continue;
            }
            if (var.second.type == "char*") {
                res << '"' << *(char**)var.second.value << '"' << '\n'; 
                continue;
            }
            if (var.second.type == "bool") {
                res << *(bool*)var.second.value << '\n';
                continue;
            }
            if (var.second.type == "float") {
                res << *(float*)var.second.value << '\n';
                continue;
            }
            if (var.second.type == "double") {
                res << *(double*)var.second.value << '\n';
                continue;
            }
            res << "[" << *((std::uint8_t*)var.second.value);
            for (size_t i = 1; i < var.second.size; ++i) {
                res << ", " << ((std::uint8_t*)var.second.value)[i];
            }
            res << "]\n";
        }
        return res.str();
    }

    void Taker::runPythonSnippet(const Snippet& snippet)
    {
        if (!pythonInitialized) {
            std::string stdOutErr = "import sys\n\
class CatchOutErr:\n\
    def __init__(self):\n\
        self.value = ''\n\
    def write(self, txt):\n\
        self.value += txt\n\
catchOutErr = CatchOutErr()\n\
sys.stdout = catchOutErr\n\
sys.stderr = catchOutErr\n\
"; // this is python code to redirect stdouts/stderr

            Py_Initialize();
            catcherModule = PyImport_AddModule("__main__"); // create main module
            PyRun_SimpleString(stdOutErr.c_str());          // invoke code to redirect

            pythonInitialized = true;
        }
        PyRun_SimpleString(preparePythonInitVarsCode().c_str());
        PyRun_SimpleString(snippet.code.c_str());

        PyObject* catcher = PyObject_GetAttrString(
            catcherModule, "catchOutErr"); // get our catchOutErr created above
        PyErr_Print();                     // make python print any errors

        PyObject* result = PyObject_GetAttrString(
            catcher, "value"); // get the stdout and stderr from our catchOutErr object

        PyObject* encoded = PyUnicode_AsEncodedString(result, "utf-8", "strict");

        std::cout << PyBytes_AsString(encoded) << std::endl;
    }

    void Taker::runSnippets()
    {
        // vars.emplace("a", new int(23));

        for (const auto& snippet : snippets) {
            // compile C using libtcc
            switch (snippet.lang) {
                case eLangC: {
                    runCSnippet(snippet);
                    break;
                }
                case eLangPython: {
                    runPythonSnippet(snippet);
                    break;
                }
                default: break;
            }
        }
    }
} // namespace tlg