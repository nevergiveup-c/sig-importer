#ifndef BATCH_H
#define BATCH_H

#include <functional>
#include <vector>
#include <optional>

#include "ida.hpp"

class Search {
    using callback_t = std::function<void(ea_t)>;

public:
    Search(ea_t start, ea_t end);

    void add(const qstring& sig, const callback_t& callback = nullptr);
    void clear();
    void run(uint32* found = nullptr);

private:
    struct region_t {
        ea_t mStart{};
        ea_t mEnd{};
    };

    struct pattern_t {
        qstring mSig{};
        callback_t mCallback{};
    };

    region_t mRegion{};
    std::vector<pattern_t> mPatterns;

    bool scan(const qstring& sig, const callback_t& callback) const;
};

#endif //BATCH_H
