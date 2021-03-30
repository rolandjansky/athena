#include "TrigT1NSWSimTools/PadTriggerRDOConverter.h"

namespace NSWL1 {
namespace PadTriggerRDOConverter {
Muon::NSW_PadTriggerSegment segment(const NSWL1::PadTrigger& data) {
    return Muon::NSW_PadTriggerSegment{
        static_cast<uint8_t>(data.bandId()), 
        static_cast<uint8_t>(data.phiId()),
        {static_cast<uint8_t>(data.trgSelectedLayersInner().size()),
        static_cast<uint8_t>(data.trgSelectedLayersOuter().size())}
    };
}

// TODO should probably be a tool, and then we can retrieve event data (L1ID) from evtStore
StatusCode fillContainer(const std::unique_ptr<Muon::NSW_PadTriggerDataContainer>& out,
    const std::vector<std::unique_ptr<NSWL1::PadTrigger>>& triggers, const uint32_t L1ID) {
    // Filter by sector:
    for(const auto& pt : triggers) {
        // `sector` in range [1, 16]
        auto sector = pt->triggerSectorNumber();
        auto endcap = pt->sideId();
        if (sector == -1 || endcap == -1) {
            return StatusCode::FAILURE;
        }
        // Calculate hash, range [0, 32)
        const uint32_t hash = 16 * endcap + sector - 1;
        // FIXME indexFind() returns const_iterator, required for comparing with end()?
        if (out->indexFind(hash) == out->end()) {
            // No collection at hash, create a new one
            auto newCollection = new Muon::NSW_PadTriggerData(
                hash,
                pt->sectorId(),
                // Consistent with PadData
                pt->isSmall() ? 0 : 1,
                pt->sideId(),
                pt->bctag(),
                L1ID,
                {}
            );
            newCollection->push_back(std::make_unique<Muon::NSW_PadTriggerSegment>(segment(*pt)));
            if(out->addCollection(newCollection, hash).isFailure()) {
                return StatusCode::FAILURE;
            }
        } else {
            auto iter = out->find(hash);
            iter->getDataPtr()->push_back(std::make_unique<Muon::NSW_PadTriggerSegment>(segment(*pt)));
        }
    }
    return StatusCode::SUCCESS;
}
} // namespace PadTriggerRDOConverter
} // namespace NSWL1
