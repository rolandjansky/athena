#ifndef MUONRDO_NSW_PADTRIGGERRAWDATA_H
#define MUONRDO_NSW_PADTRIGGERRAWDATA_H

#include <array>
#include <vector>

namespace Muon {
class NSW_PadTriggerRawData {
// TODO: Perhaps move this out of the class?
using hitlist_t = std::vector<uint16_t>;
friend class NSW_PadTriggerRawDataCnv_p1;

private:
    static constexpr std::size_t BC_COUNT{ 3 };

    // Metadata
    uint8_t m_sectorID;
    uint8_t m_endcap;
    uint16_t m_BCID;
    uint32_t m_L1ID;

    // Pad trigger input
    std::array<hitlist_t, BC_COUNT> m_hitlists;

    // Pad trigger decision
    std::array<uint8_t, 4> m_bandIDs;
    std::array<uint8_t, 4> m_phiIDs;
    // Coincidence counts for each of the two wedges, per band ID.
    std::array<std::array<uint8_t, 2>, 4> m_coincidences;
};
} // namespace Muon

#endif // MUONRDO_NSW_PADTRIGGERRAWDATA_H