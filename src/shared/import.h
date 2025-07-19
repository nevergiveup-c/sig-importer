#ifndef IMPORT_H
#define IMPORT_H

#include "json.hpp"
#include "ida.hpp"

class Import {
public:
    explicit Import(const nlohmann::json& j) {

        if (j.contains("name")) {
            mName = j["name"].get<std::string>().c_str();
        }

        if (j.contains("cmt")) {
            mComment = j["cmt"].get<std::string>().c_str();
        }

        if (j.contains("color")) {
            mColor = j["color"].get<uint32_t>();
        }

        if (j.contains("offset")) {
            mOffset = j["offset"].get<int64_t>();
        }

        if (j.contains("rip_layout")) {
            if (const auto& layout = j["rip_layout"]; layout.is_array() && layout.size() == 2) {
                mOpcodeLen = layout[0].get<uint32_t>();
                mDataLen = layout[1].get<uint32_t>();
            }
        }
    }

    qstring mName{};
    qstring mComment{};
    uint32_t mColor{ 0 };
    int64_t mOffset{ 0 };
    uint32_t mOpcodeLen{ 0 };
    uint32_t mDataLen{ 0 };
};

#endif //IMPORT_H
