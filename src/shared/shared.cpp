#include "shared.h"

#include "fpro.h"
#include "kernwin.hpp"
#include "segment.hpp"
#include "nalt.hpp"
#include "typeinf.hpp"

namespace shared {
    std::pair<ea_t, ea_t> getGlobalRange() {
        return { inf_get_min_ea(), inf_get_max_ea() };
    }

    bool hasTypeInfo(const ea_t address) {
        auto const flags = get_flags(address);
        return is_func(flags) || is_data(flags) || is_unknown(flags);
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

        std::vector<char> buffer(size + 1);
        if (auto const bytesRead = qfread(file, buffer.data(), size); bytesRead != size) {
            LOG_ERROR("Failed to read complete file: read %zu bytes, expected %zu\n",
                      bytesRead, size);
            qfclose(file);
            return nlohmann::json{};
        }
        buffer[size] = '\0';
        qfclose(file);

        try {
            return nlohmann::json::parse(buffer.data());
        } catch (const nlohmann::json::parse_error& e) {
            LOG_ERROR("Failed to parse json: %s\n", e.what());
            return nlohmann::json{};
        }
    }
    std::pair<ea_t, ea_t> getSegmentRange(qstring& name) {
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
    std::pair<ea_t, ea_t> getCustomRange(ea_t& start, ea_t& end) {
        bool askResult{};

        askResult = ask_addr(&start, "Enter start address");

        if (!askResult) {
            LOG_ERROR("Failed to get start address\n");
            return {};
        }

        askResult = ask_addr(&end, "Enter end address");

        if (!askResult) {
            LOG_ERROR("Failed to get end address\n");
            return {};
        }

        return {start, end };
    }
    std::pair<ea_t, ea_t> mainDialog() {
        int choice{ 1 };

        static constexpr char form[] =
        "Signature importer settings\n"
        "\n"
        "Choose search scope:\n"
        "<~G~lobal (search in entire binary):R>\n"
        "<~S~egment only (search in selected segment, recommended):R>\n"
        "<~C~ustom (search within range):R>>\n"
        "Debug options:\n"
        "<~E~nable debug output:C>>\n"
        "\n";

        if (ask_form(form, &choice, &gDebugOutput) > 0) {
            switch (choice) {
                case eGlobalRange: return shared::getGlobalRange();
                case eSegmentRange: {
                    qstring segmentName{".text"};
                    return getSegmentRange(segmentName);
                }
                case eCustomRange: {
                    ea_t startAddress{ 0x1000 }, endAddress{ 0x100000 };
                    return getCustomRange(startAddress, endAddress);
                }
                default: break;
            }
        }
        return { 0, 0 };
    }
}