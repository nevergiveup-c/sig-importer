#ifndef IMPORT_H
#define IMPORT_H

#include "json.hpp"
#include "ida.hpp"

#include "types.h"

class Import {
public:
    explicit Import(const nlohmann::json& j);

    void handle(ea_t address) const;
    [[nodiscard]] const JsonFields& getFields() const;
    [[nodiscard]] const char* getDisplayName() const;

private:
    enum class DeclResult { eSuccess, eNoTypeInfo, eParseFailed, eSetTInfoFailed };
    enum class CmtResult { eSuccess, eFuncCmtFailed, eCmtFailed };

    bool       adjustAddress(ea_t& address) const;
    bool       setName(ea_t address) const;
    DeclResult setDeclaration(ea_t address) const;
    CmtResult  setComment(ea_t address) const;
    bool       setBreakpoint(ea_t address) const;
    void       setColor(ea_t address) const;

    JsonFields mData{};
};

#endif //IMPORT_H
