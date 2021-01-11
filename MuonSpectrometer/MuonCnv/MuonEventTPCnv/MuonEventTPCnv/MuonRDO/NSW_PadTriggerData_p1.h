#ifndef MUONEVENTTPCNV_NSW_PADTRIGGERDATA_P1_H
#define MUONEVENTTPCNV_NSW_PADTRIGGERDATA_P1_H

#include "MuonEventTPCnv/MuonRDO/NSW_PadTriggerSegment_p1.h"

#include <vector>

namespace Muon {
struct NSW_PadTriggerData_p1 : public std::vector<NSW_PadTriggerSegment_p1> {
    unsigned int m_identifierHash;
    uint8_t m_sectorID;
    uint8_t m_sectorSize;
    uint8_t m_endcap;
    uint32_t m_BCID;
    uint32_t m_L1ID;
};
} // namespace Muon

#endif // MUONEVENTTPCNV_NSW_PADTRIGGERDATA_P1_H
