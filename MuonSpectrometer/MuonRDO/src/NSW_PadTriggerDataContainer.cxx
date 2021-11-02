#include "MuonRDO/NSW_PadTriggerDataContainer.h"

namespace Muon {

NSW_PadTriggerDataContainer::NSW_PadTriggerDataContainer()
    : IdentifiableContainer<NSW_PadTriggerData>(HASH_MAX) { }

NSW_PadTriggerDataContainer::NSW_PadTriggerDataContainer(unsigned int hashMax)
    : IdentifiableContainer<NSW_PadTriggerData>(hashMax) { }

const CLID& NSW_PadTriggerDataContainer::classID() {
    return ClassID_traits<NSW_PadTriggerDataContainer>::ID();
}

const CLID& NSW_PadTriggerDataContainer::clID() const {
    return classID();
}

std::string NSW_PadTriggerDataContainer::string() const {
    std::stringstream sstream{};
    sstream << "Number of collections: " << numberOfCollections() << ". Contains collections: " << std::endl;
    for (const NSW_PadTriggerData* collection : *this) {
        sstream << collection->string() << std::endl;
    }
    return sstream.str();
}

std::ostream& operator<<(std::ostream& stream, const NSW_PadTriggerDataContainer& rhs) {
    return stream << rhs.string();
}

MsgStream& operator<<(MsgStream& stream, const NSW_PadTriggerDataContainer& rhs) {
    return stream << rhs.string();
}
} // namespace Muon
