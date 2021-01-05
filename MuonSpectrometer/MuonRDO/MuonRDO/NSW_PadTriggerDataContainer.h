#ifndef MUONRDO_NSW_PADTRIGGERRAWDATACONTAINER
#define MUONRDO_NSW_PADTRIGGERRAWDATACONTAINER

#include "GaudiKernel/MsgStream.h"

#include "MuonRDO/NSW_PadTriggerData.h"

#include "EventContainers/IdentifiableContainer.h"

#include "SGTools/CLASS_DEF.h"

#include <sstream>

namespace Muon {
class NSW_PadTriggerDataContainer : public IdentifiableContainer<NSW_PadTriggerData> {
public:
    NSW_PadTriggerDataContainer();
    NSW_PadTriggerDataContainer(unsigned int hashMax);
    
    static const CLID& classID();
    const CLID& clID() const override;

    std::string string() const;

    friend std::ostream& operator<<(std::ostream& stream, const NSW_PadTriggerDataContainer& rhs);
    friend MsgStream& operator<<(MsgStream& stream, const NSW_PadTriggerDataContainer& rhs);
};
} // namespace Muon

CLASS_DEF( Muon::NSW_PadTriggerDataContainer , 1176278125 , 1 )

#endif // MUONRDO_NSW_PADTRIGGERRAWDATACONTAINER