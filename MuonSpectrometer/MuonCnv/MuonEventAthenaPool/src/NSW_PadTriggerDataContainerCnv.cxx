#include "NSW_PadTriggerDataContainerCnv.h"

NSW_PadTriggerDataContainerCnv::NSW_PadTriggerDataContainerCnv(ISvcLocator* svcLocator)
    : NSW_PadTriggerDataContainerCnvBase(svcLocator) { }

NSW_PadTriggerDataContainer_PERS* NSW_PadTriggerDataContainerCnv::createPersistent(Muon::NSW_PadTriggerDataContainer* transientContainer) {
    MsgStream log{ msgSvc(), "NSW_PadTriggerDataContainerCnv" };
    log << MSG::VERBOSE << "NSW_PadTriggerDataContainerCnv::createPersistent(): converting container" << std::endl;
    return m_TPConverter.createPersistent(transientContainer, log);
}

Muon::NSW_PadTriggerDataContainer* NSW_PadTriggerDataContainerCnv::createTransient() {
    MsgStream log(msgSvc(), "NSW_PadTriggerDataContainer");
    log << MSG::VERBOSE
        << "NSW_PadTriggerDataContainerCnv::createTransient(): reading container from persistent storage"
        << std::endl;
    // UUID of of NSW_PadTriggerDataContainer_p1
    static pool::Guid p1_guid("2930850B-526A-4A6B-BDC6-C86D43B06C7C");
    if(compareClassGuid(p1_guid)) {
        std::unique_ptr<Muon::NSW_PadTriggerDataContainer_p1> pContainer
            { poolReadObject<Muon::NSW_PadTriggerDataContainer_p1>() };
        return m_TPConverter.createTransient(pContainer.get(), log);
    }
    throw std::runtime_error{ "No persistent version match for GUID on-disk" };
}