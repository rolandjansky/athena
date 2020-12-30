#include "MuonRDO/NSW_PadTriggerRawDataCollection.h"

namespace Muon {
NSW_PadTriggerRawDataCollection::NSW_PadTriggerRawDataCollection(IdentifierHash identifierHash) 
    : m_identifierHash(identifierHash) { }

IdentifierHash NSW_PadTriggerRawDataCollection::identifierHash() const {
    return m_identifierHash;
}

std::string NSW_PadTriggerRawDataCollection::string() const {
    std::stringstream sstream{};
    sstream << "IdentifierHash: " << m_identifierHash << ", size: " << size();
    return sstream.str();
}

std::ostream& operator<<(std::ostream& stream, const NSW_PadTriggerRawDataCollection& rhs) {
    return stream << rhs.string();
}

MsgStream& operator<<(MsgStream& stream, const NSW_PadTriggerRawDataCollection& rhs) {
    return stream << rhs.string();
}

} // namespace Muon