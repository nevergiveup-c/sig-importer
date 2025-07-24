#ifndef IMPORT_H
#define IMPORT_H

#include "json.hpp"
#include "ida.hpp"

#include "types.h"

class Import {
public:
    explicit Import(const nlohmann::json& j);

    bool handle(ea_t address) const;
    [[nodiscard]] const JsonFields& getFields() const;
private:
    bool adjustAddress(ea_t& address) const;

    void setName(ea_t address) const;
    void setDeclaration(ea_t address) const;
    void setComment(ea_t address) const;
    void setColor(ea_t address) const;
    void setBreakpoint(ea_t address) const;

    JsonFields mData{};
};

#endif //IMPORT_H
