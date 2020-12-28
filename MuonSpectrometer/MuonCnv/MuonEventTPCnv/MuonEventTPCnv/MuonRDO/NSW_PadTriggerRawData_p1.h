#ifndef MUONEVENTTPCNV_NSW_PADTRIGGERRAWDATA_P1_H
#define MUONEVENTTPCNV_NSW_PADTRIGGERRAWDATA_P1_H

#include <cinttypes>
#include <vector>

namespace Muon {
struct NSW_PadTriggerRawData_p1 {
using hitlist_t = std::vector<uint16_t>;
    // Metadata
    uint8_t m_sectorID;
    uint8_t m_endcap;
    uint16_t m_BCID;
    uint32_t m_L1ID;

    // Pad trigger input
    hitlist_t m_hitlists[3];

    // Pad trigger decision
    uint8_t m_bandIDs[4];
    uint8_t m_phiIDs[4];
    // Coincidence counts for each of the two wedges, per band ID.
    uint8_t m_coincidences[4][2];
};
} // namespace Muon

#endif // MUONEVENTTPCNV_NSW_PADTRIGGERRAWDATA_P1_H