#ifndef SHARED_H
#define SHARED_H

#include "json.hpp"
#include "ida.hpp"

#define LOG(prefix, format, ...) msg("[SigImp] [%s] [%d:%s] >> " format, prefix, __LINE__, __func__, ##__VA_ARGS__);
#define LOG_ERROR(format, ...) LOG("ERROR", format, ##__VA_ARGS__)
#define LOG_INFO(format, ...)  LOG("INFO", format, ##__VA_ARGS__)
#define LOG_DEBUG(format, ...) LOG("DEBUG", format, ##__VA_ARGS__)

namespace shared {
    std::pair<ea_t, ea_t> getGlobalSpace();
}

namespace dialog {
    nlohmann::json parseJson(const char* fmt2);
    std::pair<ea_t, ea_t> getSegmentSpace(qstring& name);
    std::pair<ea_t, ea_t> getEaRange();
}

#endif //SHARED_H
