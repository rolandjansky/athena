#include "MuonEventTPCnv/MuonRDO/NSW_PadTriggerDataContainerCnv_p1.h"

namespace Muon {
void NSW_PadTriggerDataContainerCnv_p1::persToTrans(const NSW_PadTriggerDataContainer_p1* persistentObj, NSW_PadTriggerDataContainer* transientObj, MsgStream &log) {
    if (log.level() <= MSG::VERBOSE) {
        log << MSG::VERBOSE <<
            "Converting persistent NSW_PadTriggerDataContainer_p1 to transient NSW_PadTriggerDataContainer" << endmsg;
    }
    for (const auto& pCollection : persistentObj->m_collections) {
        std::array<std::vector<uint16_t>, 3> persistent_hitlists{ pCollection.m_precedingHitlist, pCollection.m_currentHitlist, pCollection.m_followingHitlist };
        // Can initialize here with std::move(persistent_hitlists) and modify the transient constructor accordingly
        auto tCollection = std::make_unique<NSW_PadTriggerData>(
            pCollection.m_identifierHash,
            pCollection.m_sectorID,
            static_cast<NSW_PadTriggerData::SectorSize>(pCollection.m_sectorSize),
            static_cast<NSW_PadTriggerData::Endcap>(pCollection.m_endcap),
            pCollection.m_BCID,
            pCollection.m_L1ID,
            persistent_hitlists);

        tCollection->reserve(pCollection.m_segments.size());
        for (std::size_t i{}; i < pCollection.m_segments.size(); i++) {
            tCollection->push_back(m_segmentConverter.createTransient(&pCollection.m_segments.at(i), log));
        }

        auto idHash = tCollection->identifierHash();
        if(transientObj->addCollection(tCollection.release(), idHash).isFailure()) {
            throw std::runtime_error{ "Could not add collection to transient container!" };
        }
    }
}


void NSW_PadTriggerDataContainerCnv_p1::transToPers(const NSW_PadTriggerDataContainer* transientObj, NSW_PadTriggerDataContainer_p1* persistentObj, MsgStream &log) {
    if (log.level() <= MSG::VERBOSE) {
        log << MSG::VERBOSE <<
            "Converting transient NSW_PadTriggerDataContainer to persistent NSW_PadTriggerDataContainer_p1" << endmsg;
    }
    persistentObj->m_collections.reserve(transientObj->size());
    // Iterate over collections
    for (const NSW_PadTriggerData* tCollection : *transientObj) {
        NSW_PadTriggerData_p1 pCollection{};
        pCollection.m_segments.reserve(tCollection->size());

        pCollection.m_identifierHash = tCollection->identifierHash();
        pCollection.m_sectorID = tCollection->sectorID();
        pCollection.m_sectorSize = static_cast<uint8_t>(tCollection->sectorSize());
        pCollection.m_endcap = static_cast<uint8_t>(tCollection->endcap());
        pCollection.m_BCID = tCollection->BCID();
        pCollection.m_L1ID = tCollection->L1ID();
        
        pCollection.m_precedingHitlist = tCollection->hitlists()[0];
        pCollection.m_currentHitlist = tCollection->hitlists()[1];
        pCollection.m_followingHitlist = tCollection->hitlists()[2];

        // Convert each element in the transient collection to its persistent form
        for (std::size_t i{}; i < tCollection->size(); i++) {
            NSW_PadTriggerSegment_p1 pSegment{};
            m_segmentConverter.transToPers(tCollection->at(i), &pSegment, log);
            pCollection.m_segments.push_back(pSegment);
        }
        persistentObj->m_collections.push_back(std::move(pCollection));
    }
}

} // namespace Muon
