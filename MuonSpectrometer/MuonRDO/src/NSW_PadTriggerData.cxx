#include "MuonRDO/NSW_PadTriggerData.h"

namespace Muon {
NSW_PadTriggerData::NSW_PadTriggerData(IdentifierHash identifierHash) 
    : m_identifierHash(identifierHash) { }

IdentifierHash NSW_PadTriggerData::identifierHash() const {
    return m_identifierHash;
}

std::string NSW_PadTriggerData::string() const {
    std::stringstream sstream{};
    sstream << "IdentifierHash: " << m_identifierHash << ", size: " << size();
    return sstream.str();
}

std::ostream& operator<<(std::ostream& stream, const NSW_PadTriggerData& rhs) {
    return stream << rhs.string();
}

MsgStream& operator<<(MsgStream& stream, const NSW_PadTriggerData& rhs) {
    return stream << rhs.string();
}

} // namespace Muon