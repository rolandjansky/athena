#ifndef MUONRDO_NSW_PADTRIGGERRAWDATACOLLECTION
#define MUONRDO_NSW_PADTRIGGERRAWDATACOLLECTION

#include "GaudiKernel/MsgStream.h"

#include "DataModel/DataVector.h"
#include "Identifier/IdentifierHash.h"

#include "MuonRDO/NSW_PadTriggerRawData.h"

#include <sstream>

namespace Muon {
class NSW_PadTriggerRawDataCollection : public DataVector<NSW_PadTriggerRawData> {
public:
    NSW_PadTriggerRawDataCollection(IdentifierHash identifierHash);
    IdentifierHash identifierHash() const;

    std::string string() const;

    friend std::ostream& operator<<(std::ostream& stream, const NSW_PadTriggerRawDataCollection& rhs);
    friend MsgStream& operator<<(MsgStream& stream, const NSW_PadTriggerRawDataCollection& rhs);
private:
    IdentifierHash m_identifierHash;
};
} // namespace Muon

#endif // MUONRDO_NSW_PADTRIGGERRAWDATACOLLECTION