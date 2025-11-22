#include "import.h"

#include "shared.h"

#include "fpro.h"
#include "name.hpp"
#include "offset.hpp"
#include "typeinf.hpp"
#include "dbg.hpp"

Import::Import(const nlohmann::json &j) {
    if (j.contains("name")) {
        mData.mName = j["name"].get<std::string>().c_str();
    }

    if (j.contains("declaration")) {
        mData.mDeclaration = j["declaration"].get<std::string>().c_str();
    }

    if (j.contains("comment")) {
        mData.mComment = j["comment"].get<std::string>().c_str();
    }

    if (j.contains("color")) {
        mData.mColor = j["color"].get<uint32_t>();
    }

    if (j.contains("operations")) {
        if (const auto& ops = j["operations"]; ops.is_array() && !ops.empty()) {
            for (const auto& op : ops) {
                JsonFields::EaOpData operation{};

                if (op.contains("offset")) {
                    operation.mType = eEaCalc;
                    operation.mOffset = op["offset"].get<int64_t>();
                }
                else if (op.contains("insn_format")) {
                    if (const auto& layout = op["insn_format"]; layout.is_array() && layout.size() == MAX_INSN_LAYOUT_ARRAY_SIZE) {
                        operation.mType = eAddrCalc;
                        operation.mOpcodeLength = layout[eOpcodeLength].get<uint32_t>();
                        operation.mDisplLength = layout[eDisplLength].get<uint32_t>();
                    }
                }

                mData.mEaOpsChain.push_back(operation);
            }
        }
    }

    if (j.contains("breakpoint")) {
        if (const auto& bpt = j["breakpoint"]; bpt.is_array() && bpt.size() == MAX_BPT_ARRAY_SIZE) {
            mData.mBreakpoint.mExist = bpt[eExist].get<bool>();
            mData.mBreakpoint.mType = bpt[eType].get<bpttype_t>();
            mData.mBreakpoint.mSize = bpt[eSize].get<asize_t>();
        }
    }
}

void Import::handle(ea_t address) const {
    bool success = true;

    if (!adjustAddress(address)) {
        LOG_ERROR("Failed to adjust address: 0x%p, %s\n", address, getDisplayName());
        return;
    }

    if (!setName(address)) {
        LOG_ERROR("Failed to set name: %s (0x%p)\n", mData.mName.c_str(), address);
        success = false;
    }

    if (auto const result = setDeclaration(address); result != DeclResult::eSuccess) {
        switch (result) {
            case DeclResult::eNoTypeInfo:
                LOG_ERROR("Failed to set declaration, address doesn't support type info: %s (0x%p)\n",
                    mData.mDeclaration.c_str(), address);
                break;
            case DeclResult::eParseFailed:
                LOG_ERROR("Failed to parse declaration: %s (0x%p)\n", mData.mDeclaration.c_str(), address);
                break;
            case DeclResult::eSetTInfoFailed:
                LOG_ERROR("Failed to set declaration: %s (0x%p)\n", mData.mDeclaration.c_str(), address);
                break;
            default: break;
        }
        success = false;
    }

    if (auto const result = setComment(address); result != CmtResult::eSuccess) {
        LOG_ERROR("Failed to set %s: %s (0x%p)\n", (result == CmtResult::eFuncCmtFailed) ?
            "function comment" : "comment", mData.mComment.c_str(), address);
        success = false;
    }

    if (!setBreakpoint(address)) {
        LOG_ERROR("Failed to add breakpoint at 0x%p\n", address);
        success = false;
    }

    setColor(address);

    if (success) {
        LOG_DEBUG("Success: 0x%p, %s\n", address, getDisplayName());
    }
    else {
        LOG_DEBUG("Completed with errors: 0x%p, %s\n", address, getDisplayName());
    }
}

const JsonFields& Import::getFields() const {
    return mData;
}

const char* Import::getDisplayName() const {
    for (const auto* str : { &mData.mName, &mData.mDeclaration, &mData.mComment }) {
        if (!str->empty()) {
            return str->c_str();
        }
    }
    return "None";
}

bool Import::adjustAddress(ea_t &address) const {
    for (const auto& op : mData.mEaOpsChain) {
        if (op.mType == eEaCalc) {
            address += op.mOffset;
            if (!is_loaded(address)) {
                return false;
            }
        }
        else if (op.mType == eAddrCalc) {
            if (auto const data = address + op.mOpcodeLength; is_loaded(data)) {
                if (inf_is_64bit()) {
                    auto const relOffset = static_cast<int32_t>(get_dword(address + op.mOpcodeLength));
                    auto const nextInsn = address + op.mOpcodeLength + op.mDisplLength;
                    address = nextInsn + relOffset;
                }
                else {
                    address = get_dword(address + op.mOpcodeLength);
                }
            }
            else {
                return false;
            }
        }
    }
    return is_loaded(address);
}

bool Import::setName(ea_t const address) const {
    if (mData.mName.empty()) {
        return true;
    }

    if (!set_name(address, mData.mName.c_str(), SN_NOCHECK)) {
        return false;
    }

    return true;
}

Import::DeclResult Import::setDeclaration(ea_t const address) const {
    if (mData.mDeclaration.empty()) {
        return DeclResult::eSuccess;
    }

    if (!shared::hasTypeInfo(address)) {
        return DeclResult::eNoTypeInfo;
    }

    tinfo_t tif{};
    qstring name{};
    if (parse_decl(&tif, &name, nullptr, mData.mDeclaration.c_str(), PT_TYP | PT_VAR | PT_SIL) && tif.is_correct()) {
        if (!set_tinfo(address, &tif)) {
            return DeclResult::eSetTInfoFailed;
        }
    }
    else {
        return DeclResult::eParseFailed;
    }

    return DeclResult::eSuccess;
}

Import::CmtResult Import::setComment(ea_t const address) const {
    if (mData.mComment.empty()) {
        return CmtResult::eSuccess;
    }

    if (auto const func = get_func(address); func != nullptr && func->start_ea == address) {
        if (!set_func_cmt(func, mData.mComment.c_str(), true)) {
            return CmtResult::eFuncCmtFailed;
        }
    }

    else {
        if (!set_cmt(address, mData.mComment.c_str(), true)) {
            return CmtResult::eCmtFailed;
        }
    }

    return CmtResult::eSuccess;
}

bool Import::setBreakpoint(ea_t const address) const {
    if (!mData.mBreakpoint.mExist || exist_bpt(address)) {
        return true;
    }

    return add_bpt(address, mData.mBreakpoint.mSize,
        mData.mBreakpoint.mType == 0 ? BPT_DEFAULT : mData.mBreakpoint.mType);
}

void Import::setColor(ea_t const address) const {
    if (mData.mColor != 0) {
        auto const bgr = ((mData.mColor & 0xFF) << 16) | (mData.mColor & 0xFF00) | ((mData.mColor >> 16) & 0xFF);
        set_item_color(address, bgr);
    }
}
