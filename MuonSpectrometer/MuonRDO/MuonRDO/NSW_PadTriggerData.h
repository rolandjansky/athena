#ifndef MUONRDO_NSW_PADTRIGGERDATA
#define MUONRDO_NSW_PADTRIGGERDATA

#include "GaudiKernel/MsgStream.h"

#include "DataModel/DataVector.h"
#include "Identifier/IdentifierHash.h"

#include "MuonRDO/NSW_PadTriggerSegment.h"

#include <sstream>

namespace Muon {
class NSW_PadTriggerData : public DataVector<NSW_PadTriggerSegment> {
public:
    NSW_PadTriggerData(IdentifierHash identifierHash);
    IdentifierHash identifierHash() const;

    std::string string() const;

    friend std::ostream& operator<<(std::ostream& stream, const NSW_PadTriggerData& rhs);
    friend MsgStream& operator<<(MsgStream& stream, const NSW_PadTriggerData& rhs);
private:
    IdentifierHash m_identifierHash;
    uint8_t m_sectorID;
    uint8_t m_sectorSize;
    uint8_t m_endcap;
    uint32_t m_BCID;
    uint32_t m_L1ID;
};
} // namespace Muon

#endif // MUONRDO_NSW_PADTRIGGERDATA