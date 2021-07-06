#include "MuonRDO/NSW_PadTriggerSegment.h"

namespace Muon {
NSW_PadTriggerSegment::NSW_PadTriggerSegment(uint8_t bandID, uint8_t phiID, std::array<uint8_t, 2> activeLayers)
    : m_bandID(bandID), m_phiID(phiID), m_activeLayers(activeLayers) { }

uint8_t NSW_PadTriggerSegment::bandID() const noexcept {
    return m_bandID;
}

uint8_t NSW_PadTriggerSegment::phiID() const noexcept {
    return m_phiID;
}

std::array<uint8_t, 2> NSW_PadTriggerSegment::activeLayers() const noexcept {
    return m_activeLayers;
}

std::string NSW_PadTriggerSegment::string() const {
    std::stringstream sstream{};
    sstream << "band ID: " << std::to_string(m_bandID) << ", phi ID: " << std::to_string(m_phiID)
        << ", inner active layers: " << std::to_string(m_activeLayers[0]) << ", outer active layers: "
        << std::to_string(m_activeLayers[1]);
    return sstream.str();
}

MsgStream& operator<<(MsgStream& stream, const NSW_PadTriggerSegment& rhs) {
    return stream << rhs.string();
}

std::ostream& operator<<(std::ostream& stream, const NSW_PadTriggerSegment& rhs) {
    return stream << rhs.string();
}

} // namespace Muon