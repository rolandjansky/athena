/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "TrigMuonEvent/MuonFeature.h"
#include "TrigMuonEventTPCnv/MuonFeature_p2.h"
#undef private
#undef protected

#include "TrigMuonEventTPCnv/MuonFeatureCnv_p2.h"

void MuonFeatureCnv_p2::persToTrans(const MuonFeature_p2* persObj, 
				          MuonFeature* transObj, 
				          MsgStream &log)
{
   log << MSG::DEBUG << "MuonFeatureCnv_p2::persToTrans called " << endreq;

   transObj->m_algoId     = persObj->m_algoId    ; 
   transObj->m_RoIId      = persObj->m_RoIId     ; 
   transObj->m_saddress   = persObj->m_saddress  ;
   transObj->m_pt	  = persObj->m_pt        ;
   transObj->m_radius	  = persObj->m_radius    ;
   transObj->m_eta        = persObj->m_eta       ;
   transObj->m_phi        = persObj->m_phi       ;
   transObj->m_dir_phi    = persObj->m_dir_phi   ;
   transObj->m_zeta	  = persObj->m_zeta      ;
   transObj->m_dir_zeta	  = persObj->m_dir_zeta  ;
   transObj->m_beta       = persObj->m_beta      ;     
   transObj->m_sp1_r      = persObj->m_sp1_r     ;     
   transObj->m_sp1_z      = persObj->m_sp1_z     ;     
   transObj->m_sp1_slope  = persObj->m_sp1_slope ;     
   transObj->m_sp2_r      = persObj->m_sp2_r     ;     
   transObj->m_sp2_z      = persObj->m_sp2_z     ;     
   transObj->m_sp2_slope  = persObj->m_sp2_slope ;     
   transObj->m_sp3_r      = persObj->m_sp3_r     ;     
   transObj->m_sp3_z      = persObj->m_sp3_z     ;     
   transObj->m_sp3_slope  = persObj->m_sp3_slope ;     
   transObj->m_br_radius  = persObj->m_br_radius ;     
   transObj->m_br_sagitta = persObj->m_br_sagitta;     
   transObj->m_ec_alpha   = persObj->m_ec_alpha  ;
   transObj->m_ec_beta    = persObj->m_ec_beta   ;
   transObj->m_dq_var1    = persObj->m_dq_var1  ;
   transObj->m_dq_var2    = persObj->m_dq_var2  ;
}


void MuonFeatureCnv_p2::transToPers(const MuonFeature* transObj, 
				          MuonFeature_p2* persObj, 
				          MsgStream &log)
{
   log << MSG::DEBUG << "MuonFeatureCnv_p2::transToPers called " << endreq;

   persObj->m_algoId     = transObj->m_algoId    ; 
   persObj->m_RoIId      = transObj->m_RoIId     ; 
   persObj->m_saddress   = transObj->m_saddress  ;
   persObj->m_pt	 = transObj->m_pt        ;
   persObj->m_radius	 = transObj->m_radius    ;
   persObj->m_eta        = transObj->m_eta       ;
   persObj->m_phi        = transObj->m_phi       ;
   persObj->m_dir_phi    = transObj->m_dir_phi   ;
   persObj->m_zeta	 = transObj->m_zeta      ;
   persObj->m_dir_zeta	 = transObj->m_dir_zeta  ;
   persObj->m_beta       = transObj->m_beta      ;     
   persObj->m_sp1_r      = transObj->m_sp1_r     ;     
   persObj->m_sp1_z      = transObj->m_sp1_z     ;     
   persObj->m_sp1_slope  = transObj->m_sp1_slope ;     
   persObj->m_sp2_r      = transObj->m_sp2_r     ;     
   persObj->m_sp2_z      = transObj->m_sp2_z     ;     
   persObj->m_sp2_slope  = transObj->m_sp2_slope ;     
   persObj->m_sp3_r      = transObj->m_sp3_r     ;     
   persObj->m_sp3_z      = transObj->m_sp3_z     ;     
   persObj->m_sp3_slope  = transObj->m_sp3_slope ;     
   persObj->m_br_radius  = transObj->m_br_radius ;     
   persObj->m_br_sagitta = transObj->m_br_sagitta;     
   persObj->m_ec_alpha   = transObj->m_ec_alpha  ;
   persObj->m_ec_beta    = transObj->m_ec_beta   ;
   persObj->m_dq_var1    = transObj->m_dq_var1  ;
   persObj->m_dq_var2    = transObj->m_dq_var2  ;
}
