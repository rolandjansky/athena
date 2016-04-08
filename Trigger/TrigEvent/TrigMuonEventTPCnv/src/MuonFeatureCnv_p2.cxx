/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMuonEvent/MuonFeature.h"
#include "TrigMuonEventTPCnv/MuonFeature_p2.h"
#include "TrigMuonEventTPCnv/MuonFeatureCnv_p2.h"

void MuonFeatureCnv_p2::persToTrans(const MuonFeature_p2* persObj, 
				          MuonFeature* transObj, 
				          MsgStream &log)
{
   log << MSG::DEBUG << "MuonFeatureCnv_p2::persToTrans called " << endreq;

   *transObj = MuonFeature (persObj->m_saddress,
                            persObj->m_pt,
                            persObj->m_radius,
                            persObj->m_eta,
                            persObj->m_phi,
                            persObj->m_dir_phi,
                            persObj->m_zeta,
                            persObj->m_dir_zeta,
                            persObj->m_beta,
                            persObj->m_sp1_r,
                            persObj->m_sp1_z,
                            persObj->m_sp1_slope,
                            persObj->m_sp2_r,
                            persObj->m_sp2_z,
                            persObj->m_sp2_slope,
                            persObj->m_sp3_r,
                            persObj->m_sp3_z,
                            persObj->m_sp3_slope,
                            persObj->m_br_radius,
                            persObj->m_br_sagitta,
                            persObj->m_ec_alpha,
                            persObj->m_ec_beta,
                            persObj->m_dq_var1,
                            persObj->m_dq_var2);
   transObj->set_roiId   (persObj->m_RoIId);
   transObj->set_algoId  (persObj->m_algoId);
}


void MuonFeatureCnv_p2::transToPers(const MuonFeature* transObj, 
				          MuonFeature_p2* persObj, 
				          MsgStream &log)
{
   log << MSG::DEBUG << "MuonFeatureCnv_p2::transToPers called " << endreq;

   persObj->m_algoId    = transObj->algoId(); 
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
   persObj->m_sp1_r      = transObj->sp1_r()     ;     
   persObj->m_sp1_z      = transObj->sp1_z()     ;     
   persObj->m_sp1_slope  = transObj->sp1_slope() ;     
   persObj->m_sp2_r      = transObj->sp2_r()     ;     
   persObj->m_sp2_z      = transObj->sp2_z()     ;     
   persObj->m_sp2_slope  = transObj->sp2_slope() ;     
   persObj->m_sp3_r      = transObj->sp3_r()     ;     
   persObj->m_sp3_z      = transObj->sp3_z()     ;     
   persObj->m_sp3_slope  = transObj->sp3_slope() ;     
   persObj->m_br_radius  = transObj->br_radius() ;     
   persObj->m_br_sagitta = transObj->br_sagitta();     
   persObj->m_ec_alpha   = transObj->ec_alpha()  ;
   persObj->m_ec_beta    = transObj->ec_beta()   ;
   persObj->m_dq_var1    = transObj->dq_var1()  ;
   persObj->m_dq_var2    = transObj->dq_var2()  ;
}
