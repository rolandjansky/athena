/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "TrigTopoEvent/ElectronMuonTopoInfo.h"
#undef private
#undef protected


#include "TrigTopoEventTPCnv/ElectronMuonTopoInfo_p1.h"
#include "TrigTopoEventTPCnv/ElectronMuonTopoInfoCnv_p1.h"


void ElectronMuonTopoInfoCnv_p1::persToTrans(const ElectronMuonTopoInfo_p1* persObj, 
				       ElectronMuonTopoInfo* transObj, 
				       MsgStream &log)
{
   log << MSG::DEBUG << "ElectronMuonTopoInfoCnv_p1::persToTrans called " << endreq;

   transObj->m_roiWord        = persObj->m_roiWord        ;
   transObj->m_DeltaPhi       = persObj->m_DeltaPhi       ;
   transObj->m_DeltaR         = persObj->m_DeltaR          ;
   transObj->m_InvMass        = persObj->m_InvMass         ;
   transObj->m_electronValid  = persObj->m_electronValid   ;
   transObj->m_oppositeCharge = persObj->m_oppositeCharge  ;
   transObj->m_vertexState    = persObj->m_vertexState     ;
 
}


void ElectronMuonTopoInfoCnv_p1::transToPers(const ElectronMuonTopoInfo* transObj, 
				       ElectronMuonTopoInfo_p1* persObj, 
				       MsgStream &log)
{
   log << MSG::DEBUG << "ElectronMuonTopoInfoCnv_p1::transToPers called " << endreq;

   persObj->m_roiWord        = transObj->m_roiWord        ;
   persObj->m_DeltaPhi       = transObj->m_DeltaPhi       ;
   persObj->m_DeltaR         = transObj->m_DeltaR         ;
   persObj->m_InvMass        = transObj->m_InvMass        ;
   persObj->m_electronValid  = transObj->m_electronValid  ;
   persObj->m_oppositeCharge = transObj->m_oppositeCharge ;
   persObj->m_vertexState    = transObj->m_vertexState    ;

}
