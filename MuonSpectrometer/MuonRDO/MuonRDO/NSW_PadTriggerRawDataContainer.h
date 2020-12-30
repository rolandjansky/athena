#ifndef MUONRDO_NSW_PADTRIGGERRAWDATACONTAINER
#define MUONRDO_NSW_PADTRIGGERRAWDATACONTAINER

#include "GaudiKernel/MsgStream.h"

#include "MuonRDO/NSW_PadTriggerRawDataCollection.h"

#include "EventContainers/IdentifiableContainer.h"

#include "SGTools/CLASS_DEF.h"

#include <sstream>

namespace Muon {
class NSW_PadTriggerRawDataContainer : public IdentifiableContainer<NSW_PadTriggerRawDataCollection> {
public:
    NSW_PadTriggerRawDataContainer();
    NSW_PadTriggerRawDataContainer(unsigned int hashMax);
    
    static const CLID& classID();
    const CLID& clID() const override;

    std::string string() const;

    friend std::ostream& operator<<(std::ostream& stream, const NSW_PadTriggerRawDataContainer& rhs);
    friend MsgStream& operator<<(MsgStream& stream, const NSW_PadTriggerRawDataContainer& rhs);
};
} // namespace Muon

CLASS_DEF( Muon::NSW_PadTriggerRawDataContainer , 1156203348 , 1 )

#endif // MUONRDO_NSW_PADTRIGGERRAWDATACONTAINER