#ifndef MUONRDO_NSW_PADTRIGGERRAWDATA_H
#define MUONRDO_NSW_PADTRIGGERRAWDATA_H

#include "GaudiKernel/MsgStream.h"

#include <array>
#include <vector>
#include <sstream>

namespace Muon {
class NSW_PadTriggerRawData {
using hitlist_t = std::vector<uint16_t>;
friend class NSW_PadTriggerRawDataCnv_p1;

private:
    // TODO: Perhaps move this out of the class / make public?
    static constexpr std::size_t BC_COUNT{ 3 };

    // Metadata
    uint8_t m_sectorID;
    uint8_t m_endcap;
    // BCID of the "L1A" bunch crossing (not of the additional two recorded)
    uint16_t m_BCID;
    uint32_t m_L1ID;

    // Pad trigger input
    std::array<hitlist_t, BC_COUNT> m_hitlists;

    // Pad trigger decision
    std::array<uint8_t, 4> m_bandIDs;
    std::array<uint8_t, 4> m_phiIDs;
    // Coincidence counts for each of the two wedges, per band ID.
    std::array<std::array<uint8_t, 2>, 4> m_coincidences;
public:
    NSW_PadTriggerRawData() = default;
    // Maybe too many parameters - can refactor class to hold e.g. struct TriggerMetadata, PadTriggerDecision, etc.
    NSW_PadTriggerRawData(uint8_t sectorID, uint8_t endcap, uint16_t BCID, uint32_t L1ID,
        std::array<hitlist_t, BC_COUNT> hitlists, std::array<uint8_t, 4> bandIDs, std::array<uint8_t, 4> phiIDs,
        std::array<std::array<uint8_t, 2>, 4> m_coincidences);

    uint8_t sectorID() const;
    uint8_t endcap() const;
    uint16_t BCID() const;
    uint32_t L1ID() const;

    std::array<hitlist_t, BC_COUNT> hitlists() const;
    
    std::array<uint8_t, 4> bandIDs() const;
    std::array<uint8_t, 4> phiIDs() const;
    std::array<std::array<uint8_t, 2>, 4> coincidences() const;

    std::string string() const;

    friend MsgStream& operator<<(MsgStream& stream, const NSW_PadTriggerRawData& rhs);
    friend std::ostream& operator<<(std::ostream& stream, const NSW_PadTriggerRawData& rhs);
};
} // namespace Muon

#endif // MUONRDO_NSW_PADTRIGGERRAWDATA_H