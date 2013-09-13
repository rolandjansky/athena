/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "TrigMuonEvent/MuonFeature.h"
#include "TrigMuonEventTPCnv/MuonFeature_p3.h"
#undef private
#undef protected

#include "TrigMuonEventTPCnv/MuonFeatureCnv_p3.h"

void MuonFeatureCnv_p3::persToTrans(const MuonFeature_p3* persObj, 
				          MuonFeature* transObj, 
				          MsgStream &log)
{
   log << MSG::DEBUG << "MuonFeatureCnv_p3::persToTrans called " << endreq;

   transObj->m_algoId     = persObj->m_algoId    ; 
   transObj->m_RoIId      = persObj->m_allTheInts[0]    ; 
   transObj->m_saddress   = persObj->m_allTheInts[1]    ;
   transObj->m_pt	  = persObj->m_allTheFloats[0]  ;
   transObj->m_radius	  = persObj->m_allTheFloats[1]  ;
   transObj->m_eta        = persObj->m_allTheFloats[2]  ;
   transObj->m_phi        = persObj->m_allTheFloats[3]  ;
   transObj->m_dir_phi    = persObj->m_allTheFloats[4]  ;
   transObj->m_zeta	  = persObj->m_allTheFloats[5]  ;
   transObj->m_dir_zeta	  = persObj->m_allTheFloats[6]  ;
   transObj->m_beta       = persObj->m_allTheFloats[7]  ;     
   transObj->m_sp1_r      = persObj->m_allTheFloats[8]  ;     
   transObj->m_sp1_z      = persObj->m_allTheFloats[9]  ;     
   transObj->m_sp1_slope  = persObj->m_allTheFloats[10] ;     
   transObj->m_sp2_r      = persObj->m_allTheFloats[11] ;     
   transObj->m_sp2_z      = persObj->m_allTheFloats[12] ;     
   transObj->m_sp2_slope  = persObj->m_allTheFloats[13] ;     
   transObj->m_sp3_r      = persObj->m_allTheFloats[14] ;     
   transObj->m_sp3_z      = persObj->m_allTheFloats[15] ;     
   transObj->m_sp3_slope  = persObj->m_allTheFloats[16] ;     
   transObj->m_br_radius  = persObj->m_allTheFloats[17] ;     
   transObj->m_br_sagitta = persObj->m_allTheFloats[18] ;     
   transObj->m_ec_alpha   = persObj->m_allTheFloats[19] ;
   transObj->m_ec_beta    = persObj->m_allTheFloats[20] ;
   transObj->m_dq_var1    = persObj->m_allTheFloats[21] ;
   transObj->m_dq_var2    = persObj->m_allTheFloats[22] ;
}


void MuonFeatureCnv_p3::transToPers(const MuonFeature* transObj, 
				          MuonFeature_p3* persObj, 
				          MsgStream &log)
{
   log << MSG::DEBUG << "MuonFeatureCnv_p3::transToPers called " << endreq;

   persObj->m_algoId           = transObj->m_algoId    ; 
   persObj->m_allTheInts[0]    = transObj->m_RoIId     ; 
   persObj->m_allTheInts[1]    = transObj->m_saddress  ;
   persObj->m_allTheFloats[0]  = transObj->m_pt        ;
   persObj->m_allTheFloats[1]  = transObj->m_radius    ;
   persObj->m_allTheFloats[2]  = transObj->m_eta       ;
   persObj->m_allTheFloats[3]  = transObj->m_phi       ;
   persObj->m_allTheFloats[4]  = transObj->m_dir_phi   ;
   persObj->m_allTheFloats[5]  = transObj->m_zeta      ;
   persObj->m_allTheFloats[6]  = transObj->m_dir_zeta  ;
   persObj->m_allTheFloats[7]  = transObj->m_beta      ;     
   persObj->m_allTheFloats[8]  = transObj->m_sp1_r     ;     
   persObj->m_allTheFloats[9]  = transObj->m_sp1_z     ;     
   persObj->m_allTheFloats[10] = transObj->m_sp1_slope ;     
   persObj->m_allTheFloats[11] = transObj->m_sp2_r     ;     
   persObj->m_allTheFloats[12] = transObj->m_sp2_z     ;     
   persObj->m_allTheFloats[13] = transObj->m_sp2_slope ;     
   persObj->m_allTheFloats[14] = transObj->m_sp3_r     ;     
   persObj->m_allTheFloats[15] = transObj->m_sp3_z     ;     
   persObj->m_allTheFloats[16] = transObj->m_sp3_slope ;     
   persObj->m_allTheFloats[17] = transObj->m_br_radius ;     
   persObj->m_allTheFloats[18] = transObj->m_br_sagitta;     
   persObj->m_allTheFloats[19] = transObj->m_ec_alpha  ;
   persObj->m_allTheFloats[20] = transObj->m_ec_beta   ;
   persObj->m_allTheFloats[21] = transObj->m_dq_var1  ;
   persObj->m_allTheFloats[22] = transObj->m_dq_var2  ;
}
