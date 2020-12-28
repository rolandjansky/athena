#include "MuonEventTPCnv/MuonRDO/NSW_PadTriggerRawDataCnv_p1.h"

namespace Muon {
void NSW_PadTriggerRawDataCnv_p1::persToTrans(const NSW_PadTriggerRawData_p1* persistentObj, NSW_PadTriggerRawData* transientObj, MsgStream &log) {
    // TODO implement
    log << MSG::VERBOSE << "NSW_PadTriggerRawDataCnv_p1::persToTrans() called" << std::endl;
}

void NSW_PadTriggerRawDataCnv_p1::transToPers(const NSW_PadTriggerRawData* transientObj, NSW_PadTriggerRawData_p1* persistentObj, MsgStream &log) {
    log << MSG::VERBOSE << "NSW_PadTriggerRawDataCnv_p1::transToPers() called" << std::endl;

    persistentObj->m_sectorID = transientObj->m_sectorID;
    persistentObj->m_endcap = transientObj->m_endcap;
    persistentObj->m_BCID = transientObj->m_BCID;
    persistentObj->m_L1ID = transientObj->m_L1ID;
        
    std::copy(transientObj->m_hitlists.begin(), transientObj->m_hitlists.begin() + 4, persistentObj->m_hitlists);
    std::copy(transientObj->m_bandIDs.begin(), transientObj->m_bandIDs.begin() + 4, persistentObj->m_bandIDs);
    std::copy(transientObj->m_phiIDs.begin(), transientObj->m_phiIDs.begin() + 4, persistentObj->m_phiIDs);
    
    for(std::size_t i{}; i < 4; i++) {
        const auto& cur = transientObj->m_coincidences[i];
        std::copy(cur.begin(), cur.begin() + 2, persistentObj->m_coincidences[i]);
    }
}

NSW_PadTriggerRawData* NSW_PadTriggerRawDataCnv_p1::createTransient(const NSW_PadTriggerRawData_p1* persistentObj, MsgStream& log) {
    // TODO implement
    log << MSG::VERBOSE << "NSW_PadTriggerRawDataCnv_p1::createTransient() called" << std::endl;
    return new NSW_PadTriggerRawData;
}
} // namespace Muon