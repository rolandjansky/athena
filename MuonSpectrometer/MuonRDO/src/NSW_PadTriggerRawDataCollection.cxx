#include "MuonRDO/NSW_PadTriggerRawDataCollection.h"

namespace Muon {
NSW_PadTriggerRawDataCollection::NSW_PadTriggerRawDataCollection(IdentifierHash identifierHash) 
    : m_identifierHash(identifierHash) { }

IdentifierHash NSW_PadTriggerRawDataCollection::identifierHash() const {
    return m_identifierHash;
}
} // namespace Muon