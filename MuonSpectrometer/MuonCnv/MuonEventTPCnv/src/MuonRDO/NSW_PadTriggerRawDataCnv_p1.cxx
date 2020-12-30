#include "MuonEventTPCnv/MuonRDO/NSW_PadTriggerRawDataCnv_p1.h"

namespace Muon {
void NSW_PadTriggerRawDataCnv_p1::persToTrans(const NSW_PadTriggerRawData_p1* persistentObj, NSW_PadTriggerRawData* transientObj, MsgStream &log) {
    log << MSG::VERBOSE << "NSW_PadTriggerRawDataCnv_p1::persToTrans() called" << std::endl;
    transientObj->m_sectorID = persistentObj->m_sectorID;
    transientObj->m_endcap = persistentObj->m_endcap;
    transientObj->m_BCID = persistentObj->m_BCID;
    transientObj->m_L1ID = persistentObj->m_L1ID;

    std::copy(persistentObj->m_hitlists, persistentObj->m_hitlists + 3, transientObj->m_hitlists.begin());
    std::copy(persistentObj->m_bandIDs, persistentObj->m_bandIDs + 4, transientObj->m_bandIDs.begin());
    std::copy(persistentObj->m_phiIDs, persistentObj->m_phiIDs + 4, transientObj->m_phiIDs.begin());
    for (std::size_t i{}; i < 4; i++) {
        const auto& cur = persistentObj->m_coincidences[i];
        std::copy(cur, cur + 2, transientObj->m_coincidences[i].begin());
    }
}

void NSW_PadTriggerRawDataCnv_p1::transToPers(const NSW_PadTriggerRawData* transientObj, NSW_PadTriggerRawData_p1* persistentObj, MsgStream &log) {
    log << MSG::VERBOSE << "NSW_PadTriggerRawDataCnv_p1::transToPers() called" << std::endl;

    persistentObj->m_sectorID = transientObj->m_sectorID;
    persistentObj->m_endcap = transientObj->m_endcap;
    persistentObj->m_BCID = transientObj->m_BCID;
    persistentObj->m_L1ID = transientObj->m_L1ID;
        
    std::copy(transientObj->m_hitlists.begin(), transientObj->m_hitlists.begin() + 3, persistentObj->m_hitlists);
    std::copy(transientObj->m_bandIDs.begin(), transientObj->m_bandIDs.begin() + 4, persistentObj->m_bandIDs);
    std::copy(transientObj->m_phiIDs.begin(), transientObj->m_phiIDs.begin() + 4, persistentObj->m_phiIDs);
    
    for(std::size_t i{}; i < 4; i++) {
        const auto& cur = transientObj->m_coincidences[i];
        std::copy(cur.begin(), cur.begin() + 2, persistentObj->m_coincidences[i]);
    }
}

} // namespace Muon