/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "TrigMuonEvent/MuonFeature.h"
#include "TrigMuonEventTPCnv/MuonFeature_p1.h"
#undef private
#undef protected

#include "TrigMuonEventTPCnv/MuonFeatureCnv_p1.h"

void MuonFeatureCnv_p1::persToTrans(const MuonFeature_p1* persObj, 
				          MuonFeature* transObj, 
				          MsgStream &log)
{
   log << MSG::DEBUG << "MuonFeatureCnv_p1::persToTrans called " << endreq;

   transObj->m_RoIId     = persObj->m_RoIId    ; 
   transObj->m_saddress  = persObj->m_saddress ;
   transObj->m_pt	 = persObj->m_pt       ;
   transObj->m_radius    = persObj->m_radius   ; 
   transObj->m_eta       = persObj->m_eta      ;
   transObj->m_phi       = persObj->m_phi      ;
   transObj->m_dir_phi   = persObj->m_dir_phi  ;
   transObj->m_zeta      = persObj->m_zeta     ; 
   transObj->m_dir_zeta  = persObj->m_dir_zeta ;
   transObj->m_beta      = persObj->m_beta     ;     
}


void MuonFeatureCnv_p1::transToPers(const MuonFeature* transObj, 
				          MuonFeature_p1* persObj, 
				          MsgStream &log)
{
   log << MSG::DEBUG << "MuonFeatureCnv_p1::transToPers called " << endreq;

   persObj->m_RoIId     = transObj->m_RoIId    ; 
   persObj->m_saddress  = transObj->m_saddress ;
   persObj->m_pt	= transObj->m_pt       ;
   persObj->m_radius    = transObj->m_radius   ; 
   persObj->m_eta       = transObj->m_eta      ;
   persObj->m_phi       = transObj->m_phi      ;
   persObj->m_dir_phi   = transObj->m_dir_phi  ;
   persObj->m_zeta      = transObj->m_zeta     ; 
   persObj->m_dir_zeta  = transObj->m_dir_zeta ;
   persObj->m_beta      = transObj->m_beta     ;     
}
