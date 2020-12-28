#include "NSW_PadTriggerRawDataContainerCnv.h"

NSW_PadTriggerRawDataContainerCnv::NSW_PadTriggerRawDataContainerCnv(ISvcLocator* svcLocator)
    : NSW_PadTriggerRawDataContainerCnvBase(svcLocator) { }

NSW_PadTriggerRawDataContainer_PERS* NSW_PadTriggerRawDataContainerCnv::createPersistent(Muon::NSW_PadTriggerRawDataContainer* transientContainer) {
    MsgStream log{ msgSvc(), "NSW_PadTriggerRawDataContainerCnv" };
    log << MSG::VERBOSE << "NSW_PadTriggerRawDataContainerCnv::createPersistent(): converting container" << std::endl;
    return m_TPConverter.createPersistent(transientContainer, log);
}

Muon::NSW_PadTriggerRawDataContainer* NSW_PadTriggerRawDataContainerCnv::createTransient() {
    // TODO implement
    return new Muon::NSW_PadTriggerRawDataContainer{};
}