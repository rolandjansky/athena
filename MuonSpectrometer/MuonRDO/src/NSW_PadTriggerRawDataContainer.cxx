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

std::string NSW_PadTriggerRawDataContainer::string() const {
    std::stringstream sstream{};
    sstream << "Number of collections: " << numberOfCollections() << ". Contains collections: \n" << std::endl;
    for (const auto& collection : *this) {
        sstream << collection->string() << std::endl;
    }
    return sstream.str();
}

std::ostream& operator<<(std::ostream& stream, const NSW_PadTriggerRawDataContainer& rhs) {
    return stream << rhs.string();
}

MsgStream& operator<<(MsgStream& stream, const NSW_PadTriggerRawDataContainer& rhs) {
    return stream << rhs.string();
}
} // namespace Muon