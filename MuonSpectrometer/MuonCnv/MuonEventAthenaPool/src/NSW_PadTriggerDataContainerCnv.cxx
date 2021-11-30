#include "NSW_PadTriggerDataContainerCnv.h"

NSW_PadTriggerDataContainerCnv::NSW_PadTriggerDataContainerCnv(ISvcLocator* svcLocator)
    : NSW_PadTriggerDataContainerCnvBase(svcLocator) { }

NSW_PadTriggerDataContainer_PERS* NSW_PadTriggerDataContainerCnv::createPersistent(Muon::NSW_PadTriggerDataContainer* transientContainer) {
    MsgStream log{ msgSvc(), "NSW_PadTriggerDataContainerCnv" };
    return m_TPConverter.createPersistent(transientContainer, log);
}

Muon::NSW_PadTriggerDataContainer* NSW_PadTriggerDataContainerCnv::createTransient() {
    MsgStream log(msgSvc(), "NSW_PadTriggerDataContainerCnv");
    // UUID of of NSW_PadTriggerDataContainer_p1
    static pool::Guid p1_guid("5E3C969A-84BF-4DA1-94B9-F1A6A94C3123");
    if(compareClassGuid(p1_guid)) {
        std::unique_ptr<Muon::NSW_PadTriggerDataContainer_p1> pContainer
            { poolReadObject<Muon::NSW_PadTriggerDataContainer_p1>() };
        return m_TPConverter.createTransient(pContainer.get(), log);
    }
    throw std::runtime_error{ "No persistent version match for GUID on-disk" };
}
