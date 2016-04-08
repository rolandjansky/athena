/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigTopoEvent/ElectronMuonTopoInfo.h"
#include "TrigTopoEventTPCnv/ElectronMuonTopoInfo_p1.h"
#include "TrigTopoEventTPCnv/ElectronMuonTopoInfoCnv_p1.h"


void ElectronMuonTopoInfoCnv_p1::persToTrans(const ElectronMuonTopoInfo_p1* persObj, 
				       ElectronMuonTopoInfo* transObj, 
				       MsgStream &log)
{
   log << MSG::DEBUG << "ElectronMuonTopoInfoCnv_p1::persToTrans called " << endreq;

   transObj->SetRoiWord        (persObj->m_roiWord)        ;
   transObj->SetDeltaPhi       (persObj->m_DeltaPhi)       ;
   transObj->SetDeltaR         (persObj->m_DeltaR)          ;
   transObj->SetInvMass        (persObj->m_InvMass)         ;
   transObj->SetElecValid      (persObj->m_electronValid)   ;
   transObj->SetOppositeCharge (persObj->m_oppositeCharge)  ;
   transObj->SetVertexState    (persObj->m_vertexState)     ;
 
}


void ElectronMuonTopoInfoCnv_p1::transToPers(const ElectronMuonTopoInfo* transObj, 
				       ElectronMuonTopoInfo_p1* persObj, 
				       MsgStream &log)
{
   log << MSG::DEBUG << "ElectronMuonTopoInfoCnv_p1::transToPers called " << endreq;

   persObj->m_roiWord        = transObj->RoiWord()        ;
   persObj->m_DeltaPhi       = transObj->DeltaPhi()       ;
   persObj->m_DeltaR         = transObj->DeltaR()         ;
   persObj->m_InvMass        = transObj->InvMass()        ;
   persObj->m_electronValid  = transObj->ElecValid()  ;
   persObj->m_oppositeCharge = transObj->OppositeCharge() ;
   persObj->m_vertexState    = transObj->VertexState()    ;

}
