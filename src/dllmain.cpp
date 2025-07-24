#include "dllmain.h"

#include "shared.h"
#include "search.h"
#include "import.h"

bool Plugin::run(size_t arg) {
    const auto [eaStart, eaEnd] = dialog::mainDialog();

    if (eaStart == 0 || eaEnd == 0) {
        LOG_ERROR("Failed to get search range\n")
        return false;
    }

    auto const payload = dialog::parseJson("Choose file");

    if (!payload.is_array()) {
        LOG_ERROR("Payload type isn't array\n")
        return false;
    }

    Search batch{ eaStart, eaEnd };

    LOG_INFO("Starting search with %zu patterns\n", payload.size());

    for (auto& object : payload) {

        if (!object.contains("signature")) {
            continue;
        }

        Import import{ object };

        batch.add(object["signature"].get<std::string>().c_str(), [import](ea_t address) {
            import.handle(address);
        });
    }

    uint32 counter{};
    show_wait_box("Running...");
    batch.run(&counter);
    hide_wait_box();

    const auto total = payload.size();
    const auto failed = total - counter;

    LOG_INFO("Search completed %u/%zu patterns processed, %u failed\n",
        counter, total, failed);

    return true;
}

plugmod_t* init() {
    return new Plugin();
}
