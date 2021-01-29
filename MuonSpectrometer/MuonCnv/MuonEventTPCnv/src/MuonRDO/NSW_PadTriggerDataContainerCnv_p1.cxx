#include "MuonEventTPCnv/MuonRDO/NSW_PadTriggerDataContainerCnv_p1.h"

namespace Muon {
void NSW_PadTriggerDataContainerCnv_p1::persToTrans(const NSW_PadTriggerDataContainer_p1* persistentObj, NSW_PadTriggerDataContainer* transientObj, MsgStream &log) {
    log << MSG::VERBOSE << "NSW_PadTriggerDataContainerCnv_p1::persToTrans() called" << std::endl;
    for (const auto& pCollection : *persistentObj) {
        std::array<std::vector<uint16_t>, 3> persistent_hitlists{ pCollection.m_precedingHitlist, pCollection.m_currentHitlist, pCollection.m_followingHitlist };
        // Can initialize here with std::move(persistent_hitlists) and modify the transient constructor accordingly
        auto tCollection = std::make_unique<NSW_PadTriggerData>(pCollection.m_identifierHash, pCollection.m_sectorID,
            pCollection.m_sectorSize, pCollection.m_endcap, pCollection.m_BCID, pCollection.m_L1ID, persistent_hitlists);
        tCollection->reserve(pCollection.size());
        for (std::size_t i{}; i < pCollection.size(); i++) {
            tCollection->push_back(m_segmentConverter.createTransient(&pCollection.at(i), log));
        }

        auto idHash = tCollection->identifierHash();
        if(transientObj->addCollection(tCollection.release(), idHash).isFailure()) {
            throw std::runtime_error{ "Could not add collection to transient container!" };
        }
    }
}


void NSW_PadTriggerDataContainerCnv_p1::transToPers(const NSW_PadTriggerDataContainer* transientObj, NSW_PadTriggerDataContainer_p1* persistentObj, MsgStream &log) {
    log << MSG::VERBOSE << "NSW_PadTriggerDataContainerCnv_p1::transToPers() called" << std::endl;
    
    persistentObj->reserve(transientObj->size());
    // Iterate over collections
    for (const auto& tCollection : *transientObj) {
        NSW_PadTriggerData_p1 pCollection{};
        pCollection.reserve(tCollection->size());

        pCollection.m_identifierHash = tCollection->identifierHash();
        pCollection.m_sectorID = tCollection->sectorID();
        pCollection.m_sectorSize = tCollection->sectorSize();
        pCollection.m_endcap = tCollection->endcap();
        pCollection.m_BCID = tCollection->BCID();
        pCollection.m_L1ID = tCollection->L1ID();
        
        pCollection.m_precedingHitlist = tCollection->hitlists()[0];
        pCollection.m_currentHitlist = tCollection->hitlists()[1];
        pCollection.m_followingHitlist = tCollection->hitlists()[2];

        // Convert each element in the transient collection to its persistent form
        for (std::size_t i{}; i < tCollection->size(); i++) {
            NSW_PadTriggerSegment_p1 pSegment{};
            m_segmentConverter.transToPers(tCollection->at(i), &pSegment, log);
            pCollection.push_back(pSegment);
        }
        persistentObj->push_back(pCollection);
    }
}

} // namespace Muon
