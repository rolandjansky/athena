#include "MuonEventTPCnv/MuonRDO/NSW_PadTriggerRawDataContainerCnv_p1.h"

namespace Muon {
void NSW_PadTriggerRawDataContainerCnv_p1::persToTrans(const NSW_PadTriggerRawDataContainer_p1* persistentObj, NSW_PadTriggerRawDataContainer* transientObj, MsgStream &log) {
    log << MSG::VERBOSE << "NSW_PadTriggerRawDataContainerCnv_p1::persToTrans() called" << std::endl;
    for (const auto& pCollection : *persistentObj) {
        auto tCollection = std::make_unique<NSW_PadTriggerRawDataCollection>(pCollection.m_identifierHash);
        tCollection->reserve(pCollection.size());
        for (std::size_t i{}; i < pCollection.size(); i++) {
            tCollection->push_back(m_dataConverter.createTransient(&pCollection.at(i), log));
        }
        auto idHash = tCollection->identifierHash();
        if(transientObj->addCollection(tCollection.release(), idHash).isFailure()) {
            throw std::runtime_error{ "Could not add collection to transient container!" };
        }
    }
}

void NSW_PadTriggerRawDataContainerCnv_p1::transToPers(const NSW_PadTriggerRawDataContainer* transientObj, NSW_PadTriggerRawDataContainer_p1* persistentObj, MsgStream &log) {
    // Can use T_AthenaPoolTPCnvIDCont / T_AthenaPoolTPPtrVectorCnv here, should we?
    log << MSG::VERBOSE << "NSW_PadTriggerRawDataContainerCnv_p1::transToPers() called" << std::endl;
    
    persistentObj->reserve(transientObj->size());
    // Iterate over collections
    for (const auto& tCollection : *transientObj) {
        NSW_PadTriggerRawDataCollection_p1 pCollection{};
        pCollection.reserve(tCollection->size());
        pCollection.m_identifierHash = tCollection->identifierHash();
        // Convert each element in the transient collection to its persistent form
        for (std::size_t i{}; i < tCollection->size(); i++) {
            NSW_PadTriggerRawData_p1 pData{};
            m_dataConverter.transToPers(tCollection->at(i), &pData, log);
            pCollection.push_back(std::move(pData));
        }
        persistentObj->push_back(pCollection);
    }
}

} // namespace Muon