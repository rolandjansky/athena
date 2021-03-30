#ifndef TRIGT1NSWSIMTOOLS_PADTRIGGERRDOCONVERTER_H
#define TRIGT1NSWSIMTOOLS_PADTRIGGERRDOCONVERTER_H

#include "MuonRDO/NSW_PadTriggerData.h"
#include "MuonRDO/NSW_PadTriggerDataContainer.h"
#include "MuonRDO/NSW_PadTriggerSegment.h"
#include "TrigT1NSWSimTools/PadTrigger.h"

namespace NSWL1{
namespace PadTriggerRDOConverter {
// Convert a single simulated pad trigger segment to the RDO version of a pad trigger segment.
Muon::NSW_PadTriggerSegment segment(const NSWL1::PadTrigger& data);

// Fill a pad trigger data container with data from a vector of pad trigger segments.
// If the container isn't empty, new data is simply appended to it.
StatusCode fillContainer(const std::unique_ptr<Muon::NSW_PadTriggerDataContainer>& out,
    const std::vector<std::unique_ptr<NSWL1::PadTrigger>>& triggers, const uint32_t L1ID);

} // namespace NSWL1
} // namespace PadTriggerRDOConverter

#endif // TRIGT1NSWSIMTOOLS_PADTRIGGERRDOCONVERTER_H
