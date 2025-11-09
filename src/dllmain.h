// ReSharper disable CppNonInlineVariableDefinitionInHeaderFile

#ifndef DLL_MAIN_H
#define DLL_MAIN_H

#include <ida.hpp>
#include <idp.hpp>
#include <loader.hpp>

class Plugin final : public plugmod_t {
public:
    bool idaapi run(size_t arg) override;
};

#define PLUGIN_NAME "Signature importer"
#define PLUGIN_VERSION "1.0.4"
#define PLUGIN_HOTKEY "Ctrl+Shift+F10"

plugmod_t* idaapi init();

plugin_t PLUGIN {
    IDP_INTERFACE_VERSION,
    PLUGIN_MULTI,
    init,
    nullptr,
    nullptr,
    PLUGIN_NAME " v" PLUGIN_VERSION " for IDA Pro 9.x by nevergiveup-c",
    "1. Run the plugin.\n"
    "2. Choose search scope: global, segment only or custom.\n"
    "Search descriptions:\n"
    " - If segment only is selected, enter segment name (e.g., .text);\n"
    " - If custom is selected, enter start and end address (e.g., 0x1000 - 0x100000);\n"
    "4. Choose *.json file (you can see json format in README.md).\n"
    "5. Wait for the scanning process to complete.",
    PLUGIN_NAME,
    PLUGIN_HOTKEY
};

#endif //DLL_MAIN_H