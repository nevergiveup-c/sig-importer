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
#define PLUGIN_HOTKEY "Ctrl+Shift+F10"

plugmod_t* idaapi init();

plugin_t PLUGIN {
    IDP_INTERFACE_VERSION,
    PLUGIN_MULTI,
    init,
    nullptr,
    nullptr,
    PLUGIN_NAME " 9x for IDA Pro by nevergiveup-c",
    "1. Run the plugin.\n"
    "2. Choose search scope: Global or segment only.\n"
    "3. If segment only selected, enter segment name (e.g., .text).\n"
    "4. Choose *.json file (json format you can see in README.md).\n"
    "5. Wait for the scanning process to complete.",
    PLUGIN_NAME,
    PLUGIN_HOTKEY
};

#endif //DLL_MAIN_H