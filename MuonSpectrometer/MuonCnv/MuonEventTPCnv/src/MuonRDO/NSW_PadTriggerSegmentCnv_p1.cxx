#include "MuonEventTPCnv/MuonRDO/NSW_PadTriggerSegmentCnv_p1.h"


namespace Muon {
void NSW_PadTriggerSegmentCnv_p1::persToTrans(const NSW_PadTriggerSegment_p1* persistentObj, NSW_PadTriggerSegment* transientObj, MsgStream &log) {
    log << MSG::VERBOSE << "Converting persistent NSW_PadTriggerSegment_p1 to transient NSW_PadTriggerSegment" << endmsg;
    transientObj->m_phiID = persistentObj->m_phiID;
    transientObj->m_bandID = persistentObj->m_bandID;
    transientObj->m_activeLayers[0] = persistentObj->m_innerActiveLayers;
    transientObj->m_activeLayers[1] = persistentObj->m_outerActiveLayers;
}

void NSW_PadTriggerSegmentCnv_p1::transToPers(const NSW_PadTriggerSegment* transientObj, NSW_PadTriggerSegment_p1* persistentObj, MsgStream &log) {
    log << MSG::VERBOSE << "Converting transient NSW_PadTriggerSegment to persistent NSW_PadTriggerSegment_p1" << endmsg;
    persistentObj->m_phiID = transientObj->m_phiID;
    persistentObj->m_bandID = transientObj->m_bandID;
    persistentObj->m_innerActiveLayers = transientObj->m_activeLayers[0];
    persistentObj->m_outerActiveLayers = transientObj->m_activeLayers[1];
}

} // namespace Muon
