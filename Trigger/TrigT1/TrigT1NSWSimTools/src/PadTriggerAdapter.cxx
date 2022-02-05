#include "TrigT1NSWSimTools/PadTriggerAdapter.h"

namespace NSWL1 {
namespace PadTriggerAdapter {
Muon::NSW_PadTriggerSegment segment(const NSWL1::PadTrigger& data) {
    return Muon::NSW_PadTriggerSegment{
        static_cast<uint8_t>(data.bandId()),
        static_cast<uint8_t>(data.phiId()),
        {static_cast<uint8_t>(data.trgSelectedLayersInner().size()),
        static_cast<uint8_t>(data.trgSelectedLayersOuter().size())}
    };
}

// TODO should probably be a tool, and then we can retrieve event data from
// evtStore
StatusCode
fillContainer(const std::unique_ptr<Muon::NSW_PadTriggerDataContainer> &out,
              const std::vector<std::unique_ptr<NSWL1::PadTrigger>> &triggers,
              const uint32_t l1id) {
  using TriggerList = std::vector<const NSWL1::PadTrigger *>;
  using TriggerMap = std::map<uint32_t, TriggerList>;
  TriggerMap triggerMap;
  // Filter by sector:
  for (const auto &pt : triggers) {
    // `sector` in range [0, 16)
    auto sector = pt->triggerSectorNumber() - 1;
    auto endcap = pt->sideId();
    if (sector == -1 || endcap == -1) {
      return StatusCode::FAILURE;
    }
    // Calculate hash, range [0, 32)
    const uint32_t hash = 16 * endcap + sector;
    auto it = triggerMap.find(hash);
    if (it == triggerMap.end()) {
        it = triggerMap.insert(std::make_pair(hash, TriggerList())).first;
    }
    it->second.push_back(pt.get());
  }
  for (const auto &item : triggerMap) {
    uint32_t hash = item.first;
    const TriggerList &triggerList = item.second;
    auto pt = triggerList[0];
    auto newCollection = new Muon::NSW_PadTriggerData(
        hash, pt->sectorId(),
        static_cast<Muon::NSW_PadTriggerData::SectorSize>(pt->isSmall() ? 0
                                                                        : 1),
        static_cast<Muon::NSW_PadTriggerData::Endcap>(pt->sideId()),
        pt->bctag(), l1id, {});
    if (out->addCollection(newCollection, hash).isFailure()) {
      return StatusCode::FAILURE;
    }
    for (auto pt : triggerList)
      newCollection->push_back(
          std::make_unique<Muon::NSW_PadTriggerSegment>(segment(*pt)));
  }
  return StatusCode::SUCCESS;
}
} // namespace PadTriggerAdapter
} // namespace NSWL1
