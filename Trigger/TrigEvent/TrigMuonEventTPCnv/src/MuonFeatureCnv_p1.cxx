/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMuonEvent/MuonFeature.h"
#include "TrigMuonEventTPCnv/MuonFeature_p1.h"
#include "TrigMuonEventTPCnv/MuonFeatureCnv_p1.h"

void MuonFeatureCnv_p1::persToTrans(const MuonFeature_p1* persObj, 
				          MuonFeature* transObj, 
				          MsgStream &log)
{
   log << MSG::DEBUG << "MuonFeatureCnv_p1::persToTrans called " << endreq;

   *transObj = MuonFeature (persObj->m_saddress,
                            persObj->m_pt,
                            persObj->m_radius,
                            persObj->m_eta,
                            persObj->m_phi,
                            persObj->m_dir_phi,
                            persObj->m_zeta,
                            persObj->m_dir_zeta,
                            persObj->m_beta);
   transObj->set_roiId   (persObj->m_RoIId);
}


void MuonFeatureCnv_p1::transToPers(const MuonFeature* transObj, 
				          MuonFeature_p1* persObj, 
				          MsgStream &log)
{
   log << MSG::DEBUG << "MuonFeatureCnv_p1::transToPers called " << endreq;

   persObj->m_RoIId     = transObj->roiId()    ; 
   persObj->m_saddress  = transObj->saddress() ;
   persObj->m_pt	= transObj->pt()       ;
   persObj->m_radius    = transObj->radius()   ; 
   persObj->m_eta       = transObj->eta()      ;
   persObj->m_phi       = transObj->phi()      ;
   persObj->m_dir_phi   = transObj->dir_phi()  ;
   persObj->m_zeta      = transObj->zeta()     ; 
   persObj->m_dir_zeta  = transObj->dir_zeta() ;
   persObj->m_beta      = transObj->beta()     ;     
}
