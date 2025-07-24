#ifndef TYPES_H
#define TYPES_H

#include "ida.hpp"
#include "idd.hpp"

constexpr size_t MAX_BPT_ARRAY_SIZE = 3;
constexpr size_t MAX_INSN_LAYOUT_ARRAY_SIZE = 2;

enum OpType {
    eEaCalc,
    eRipCalc
};

enum InsnArrayIdx {
    eOpcodeLength,
    eDisplLength,
};

enum BptArrayIdx {
    eExist,
    eType,
    eSize,
};

struct JsonFields {
    struct EaOpData {
        OpType mType{};
        int64_t mOffset{ 0 };
        uint32_t mOpcodeLength{ 0 };
        uint32_t mDisplLength{ 0 };
    };
    struct BptData {
        bool mExist{};
        bpttype_t mType{};
        asize_t mSize{};
    };

    qstring mName{};
    qstring mDeclaration{};
    qstring mComment{};
    uint32_t mColor{ 0 };
    qvector<EaOpData> mEaOpsChain{};
    BptData mBreakpoint{};
};

#endif //TYPES_H
