
#include <thread>

#include "search.h"
#include "shared.h"

#include "kernwin.hpp"
#include "bytes.hpp"

bool Search::scan(const qstring& sig, const callback_t& callback) const {

    compiled_binpat_vec_t binaryPattern;
    parse_binpat_str(&binaryPattern, mRegion.mStart, sig.c_str(), 16);

    auto const ea = bin_search(mRegion.mStart,  mRegion.mEnd,
        binaryPattern, BIN_SEARCH_NOCASE | BIN_SEARCH_FORWARD);

    if (ea == BADADDR) {
        return false;
    }

    callback(ea);
    return true;
}

Search::Search(const ea_t start, const ea_t end) {
    mRegion.mStart = start;
    mRegion.mEnd = end;
}

void Search::add(const qstring& sig, const callback_t& callback) {
    mPatterns.emplace_back(pattern_t{ sig, callback });
}

void Search::clear() {
    mPatterns.clear();
}

void Search::run(uint32* found) {
    for (const auto& [sig, callback]: mPatterns) {
        if (user_cancelled()) {
            clear();
            return;
        }
        if (!scan(sig, callback)) {
            LOG_ERROR("Failed to search: %s\n", sig.c_str())
        }
        else {
            if (found != nullptr) {
                ++*found;
            }
        }

    }
    clear();
}
