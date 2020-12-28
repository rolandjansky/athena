#ifndef MUONRDO_NSW_PADTRIGGERRAWDATACOLLECTION
#define MUONRDO_NSW_PADTRIGGERRAWDATACOLLECTION

#include "DataModel/DataVector.h"
#include "Identifier/IdentifierHash.h"

#include "MuonRDO/NSW_PadTriggerRawData.h"

namespace Muon {
class NSW_PadTriggerRawDataCollection : public DataVector<NSW_PadTriggerRawData> {
public:
    NSW_PadTriggerRawDataCollection(IdentifierHash identifierHash);
    IdentifierHash identifierHash() const;
private:
    IdentifierHash m_identifierHash;
};
} // namespace Muon

#endif // MUONRDO_NSW_PADTRIGGERRAWDATACOLLECTION