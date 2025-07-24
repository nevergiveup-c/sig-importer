#include "dllmain.h"

#include "shared.h"
#include "search.h"
#include "import.h"

#include "fpro.h"
#include "name.hpp"
#include "offset.hpp"
#include "typeinf.hpp"

bool Plugin::run(size_t arg) {
    const auto [eaStart, eaEnd] = dialog::mainDialog();

    if (eaStart == 0 || eaEnd == 0) {
        LOG_ERROR("Failed to get search range\n")
        return false;
    }

    auto const payload = dialog::parseJson("Choose file");

    if (!payload.is_array()) {
        LOG_ERROR("Payload type isn't array\n")
        return false;
    }

    Search batch{ eaStart, eaEnd };

    LOG_INFO("Starting search with %zu patterns\n", payload.size());

    for (auto& object : payload) {

        if (!object.contains("signature")) {
            continue;
        }

        Import import{ object };

        batch.add(object["signature"].get<std::string>().c_str(), [import](ea_t address) {

            if (import.mOffset != 0) {
                address += import.mOffset;
            }

            if (import.mOpcodeLen != 0 && import.mDataLen != 0) {
                auto const relOffset = get_dword(address + import.mOpcodeLen);
                auto const nextInsn = address + import.mOpcodeLen + import.mDataLen;
                address = nextInsn + relOffset;
            }

            if (!import.mName.empty()) {
                set_name(address, import.mName.c_str(), SN_NOCHECK);
            }

            if (!import.mComment.empty()) {
                if (auto const fn = get_func(address); fn == nullptr) {
                    set_cmt(address, import.mComment.c_str(), true);
                }
                else {
                    set_func_cmt(fn, import.mComment.c_str(), true);
                }
            }

            if (import.mColor != 0) {
                auto const bgr = ((import.mColor & 0xFF) << 16) | (import.mColor & 0xFF00) | ((import.mColor >> 16) & 0xFF);
                set_item_color(address, bgr);
            }
        });
    }

    uint32 counter{};
    show_wait_box( "Running..." );
    batch.run(&counter);
    hide_wait_box( );

    const auto total = payload.size();
    const auto failed = total - counter;

    LOG_INFO("Search completed %u/%zu patterns processed, %u failed\n",
        counter, total, failed);

    return true;
}

plugmod_t* init() {
    return new Plugin();
}
