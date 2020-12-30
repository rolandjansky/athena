#include "NSW_PadTriggerRawDataContainerCnv.h"

NSW_PadTriggerRawDataContainerCnv::NSW_PadTriggerRawDataContainerCnv(ISvcLocator* svcLocator)
    : NSW_PadTriggerRawDataContainerCnvBase(svcLocator) { }

NSW_PadTriggerRawDataContainer_PERS* NSW_PadTriggerRawDataContainerCnv::createPersistent(Muon::NSW_PadTriggerRawDataContainer* transientContainer) {
    MsgStream log{ msgSvc(), "NSW_PadTriggerRawDataContainerCnv" };
    log << MSG::VERBOSE << "NSW_PadTriggerRawDataContainerCnv::createPersistent(): converting container" << std::endl;
    return m_TPConverter.createPersistent(transientContainer, log);
}

Muon::NSW_PadTriggerRawDataContainer* NSW_PadTriggerRawDataContainerCnv::createTransient() {
    MsgStream log(msgSvc(), "NSW_PadTriggerRawDataContainer");
    log << MSG::VERBOSE
        << "NSW_PadTriggerRawDataContainerCnv::createTransient(): reading container from persistent storage"
        << std::endl;
    // UUID of of NSW_PadTriggerRawDataContainer_p1
    static pool::Guid p1_guid("2B1C6988-7B4C-4332-A2F5-83DC1D219650");
    if(compareClassGuid(p1_guid)) {
        std::unique_ptr<Muon::NSW_PadTriggerRawDataContainer_p1> pContainer
            { poolReadObject<Muon::NSW_PadTriggerRawDataContainer_p1>() };
        return m_TPConverter.createTransient(pContainer.get(), log);
    }
    throw std::runtime_error{ "No persistent version match for GUID on-disk" };
}