#include "MuonRDO/NSW_PadTriggerRawData.h"

namespace Muon {
NSW_PadTriggerRawData::NSW_PadTriggerRawData(uint8_t sectorID, uint8_t endcap, uint16_t BCID, uint32_t L1ID,
    std::array<hitlist_t, BC_COUNT> hitlists, std::array<uint8_t, 4> bandIDs, std::array<uint8_t, 4> phiIDs,
    std::array<std::array<uint8_t, 2>, 4> coincidences)
    : m_sectorID(sectorID), m_endcap(endcap), m_BCID(BCID), m_L1ID(L1ID), m_hitlists(hitlists), m_bandIDs(bandIDs),
    m_phiIDs(phiIDs), m_coincidences(coincidences) { }

uint8_t NSW_PadTriggerRawData::sectorID() const {
    return m_sectorID;
}

uint8_t NSW_PadTriggerRawData::endcap() const {
    return m_endcap;
}

uint16_t NSW_PadTriggerRawData::BCID() const {
    return m_BCID;
}

uint32_t NSW_PadTriggerRawData::L1ID() const {
    return m_L1ID;
}

std::array<NSW_PadTriggerRawData::hitlist_t, NSW_PadTriggerRawData::BC_COUNT> NSW_PadTriggerRawData::hitlists() const {
    return m_hitlists;
}

std::array<uint8_t, 4> NSW_PadTriggerRawData::bandIDs() const {
    return m_bandIDs;
}

std::array<uint8_t, 4> NSW_PadTriggerRawData::phiIDs() const {
    return m_phiIDs;
}

std::array<std::array<uint8_t, 2>, 4> NSW_PadTriggerRawData::coincidences() const {
    return m_coincidences;
}

} // namespace Muon