#include "shared.h"

#include "fpro.h"
#include "kernwin.hpp"
#include "segment.hpp"

namespace shared {
    std::pair<ea_t, ea_t> getGlobalSpace() {
        return { inf_get_min_ea(), inf_get_max_ea() };
    }
}

namespace dialog {
    nlohmann::json parseJson(const char* fmt2) {

        auto const path = ask_file(false, ".json", fmt2);

        if (path == nullptr) {
            LOG_ERROR("Failed to get a file name from dialog widget\n");
            return nlohmann::json{};
        }

       auto const file = qfopen(path, "rb");

        if (file == nullptr) {
            LOG_ERROR("Failed to get a file\n");
            return nlohmann::json{};
        }

        auto const size = qfsize(file);

        if (size == 0) {
            LOG_ERROR("Empty file received\n");
            qfclose(file);
            return nlohmann::json{};
        };

        std::vector<char> buffer(size);
        if (auto const bytesRead = qfread(file, buffer.data(), size); bytesRead != size) {
            LOG_ERROR("Failed to read complete file: read %zu bytes, expected %zu\n",
                      bytesRead, size);
            qfclose(file);
            return nlohmann::json{};
        }
        qfclose(file);

        try {
            return nlohmann::json::parse(buffer.data());
        } catch (const nlohmann::json::parse_error& e) {
            LOG_ERROR("Failed to parse json: %s\n", e.what());
            return nlohmann::json{};
        }
    }
    std::pair<ea_t, ea_t> getSegmentSpace(qstring& name) {

        auto const result = ask_str(&name, HIST_SEG, "Enter the segment");

        if (!result) {
            LOG_ERROR("Failed to get segment name from dialog widget\n");
            return {};
        }

        auto const segment = get_segm_by_name(name.c_str());

        if (segment == nullptr) {
            LOG_ERROR("Failed to get segment\n");
            return {};
        }

        auto const eaMax = inf_get_max_ea();
        auto const nextSeg = get_next_seg(segment->start_ea);

        //TODO: I don't know how to do it normally...
        ea_t end = nextSeg != nullptr ? nextSeg->start_ea : eaMax;

        return { segment->start_ea, end > 0 ? end : eaMax };
    }
    std::pair<ea_t, ea_t> mainDialog() {
        int choice{ 1 };

        static constexpr char form[] =
        "Signature importer settings\n"
        "\n"
        "Choose search scope:\n"
        "<~G~lobal (search in entire binary):R>\n"
        "<~S~egment only (search in selected segment, recommended):R>>\n"
        "Debug options:\n"
        "<~E~nable debug output:C>>\n"
        "\n";

        if (ask_form(form, &choice, &gDebugOutput) > 0) {
            if (choice == 0) {
                return shared::getGlobalSpace();
            }
            qstring segmentName{".text"};
            return getSegmentSpace(segmentName);
        }

        return { 0, 0 };
    }
}