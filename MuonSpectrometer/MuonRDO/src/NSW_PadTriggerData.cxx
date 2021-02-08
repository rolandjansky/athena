#include "MuonRDO/NSW_PadTriggerData.h"

namespace Muon {
NSW_PadTriggerData::NSW_PadTriggerData(IdentifierHash identifierHash, uint8_t sectorID, uint8_t sectorSize,
    uint8_t endcap, uint32_t BCID, uint32_t L1ID, const std::array<hitlist_t, 3>& hitlists)
    : m_identifierHash(identifierHash), m_sectorID(sectorID), m_sectorSize(sectorSize), m_endcap(endcap), m_BCID(BCID),
        m_L1ID(L1ID), m_hitlists(hitlists) { }

IdentifierHash NSW_PadTriggerData::identifierHash() const {
    return m_identifierHash;
}

std::string NSW_PadTriggerData::string() const {
    std::stringstream sstream{};
    sstream << "IdentifierHash: " << m_identifierHash << ", size: " << size();
    return sstream.str();
}

uint8_t NSW_PadTriggerData::sectorID() const noexcept {
    return m_sectorID;
}

uint8_t NSW_PadTriggerData::sectorSize() const noexcept {
    return m_sectorSize;
}

uint8_t NSW_PadTriggerData::endcap() const noexcept {
    return m_endcap;
}

uint32_t NSW_PadTriggerData::BCID() const noexcept {
    return m_BCID;
}

uint32_t NSW_PadTriggerData::L1ID() const noexcept {
    return m_L1ID;
}

const std::array<NSW_PadTriggerData::hitlist_t, 3>& NSW_PadTriggerData::hitlists() const {
    return m_hitlists;
}

std::ostream& operator<<(std::ostream& stream, const NSW_PadTriggerData& rhs) {
    return stream << rhs.string();
}

MsgStream& operator<<(MsgStream& stream, const NSW_PadTriggerData& rhs) {
    return stream << rhs.string();
}

} // namespace Muon