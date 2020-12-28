#ifndef MUONEVENTTPCNV_NSW_PADTRIGGERRAWDATACONTAINER_P1_H
#define MUONEVENTTPCNV_NSW_PADTRIGGERRAWDATACONTAINER_P1_H

#include "MuonEventTPCnv/MuonRDO/NSW_PadTriggerRawDataCollection_p1.h"

#include <vector>

namespace Muon {
struct NSW_PadTriggerRawDataContainer_p1 : public std::vector<NSW_PadTriggerRawDataCollection_p1>
    { };
} // namespace Muon

#endif // MUONEVENTTPCNV_NSW_PADTRIGGERRAWDATACONTAINER_P1_H
