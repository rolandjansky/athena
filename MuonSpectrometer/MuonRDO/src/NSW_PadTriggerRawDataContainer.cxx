#include "MuonRDO/NSW_PadTriggerRawDataContainer.h"

namespace Muon {

NSW_PadTriggerRawDataContainer::NSW_PadTriggerRawDataContainer()
    : IdentifiableContainer<NSW_PadTriggerRawDataCollection>(0) { }

NSW_PadTriggerRawDataContainer::NSW_PadTriggerRawDataContainer(unsigned int hashMax) 
    : IdentifiableContainer<NSW_PadTriggerRawDataCollection>(hashMax) { }
    
const CLID& NSW_PadTriggerRawDataContainer::classID() {
    return ClassID_traits<NSW_PadTriggerRawDataContainer>::ID();
}

const CLID& NSW_PadTriggerRawDataContainer::clID() const {
    return classID();    
}

} // namespace Muon