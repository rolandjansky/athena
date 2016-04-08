/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMuonEvent/MuonFeature.h"
#include "TrigMuonEventTPCnv/MuonFeature_p3.h"
#include "TrigMuonEventTPCnv/MuonFeatureCnv_p3.h"

void MuonFeatureCnv_p3::persToTrans(const MuonFeature_p3* persObj, 
				          MuonFeature* transObj, 
				          MsgStream &log)
{
   log << MSG::DEBUG << "MuonFeatureCnv_p3::persToTrans called " << endreq;

   *transObj = MuonFeature (persObj->m_allTheInts[1], // saddress
                            persObj->m_allTheFloats[0], // pt
                            persObj->m_allTheFloats[1], // radius
                            persObj->m_allTheFloats[2], // eta
                            persObj->m_allTheFloats[3], // phi
                            persObj->m_allTheFloats[4], // dir_phi
                            persObj->m_allTheFloats[5], // zeta
                            persObj->m_allTheFloats[6], // dir_zeta
                            persObj->m_allTheFloats[7], // beta
                            persObj->m_allTheFloats[8], // sp1_r
                            persObj->m_allTheFloats[9], // sp1_z
                            persObj->m_allTheFloats[10], // sp1_slope
                            persObj->m_allTheFloats[11], // sp2_r
                            persObj->m_allTheFloats[12], // sp2_z
                            persObj->m_allTheFloats[13], // sp2_slope
                            persObj->m_allTheFloats[14], // sp3_r
                            persObj->m_allTheFloats[15], // sp3_z
                            persObj->m_allTheFloats[16], // sp3_slope
                            persObj->m_allTheFloats[17], // br_radius
                            persObj->m_allTheFloats[18], // br_sagitta
                            persObj->m_allTheFloats[19], // ec_alpha
                            persObj->m_allTheFloats[20], // ec_beta
                            persObj->m_allTheFloats[21], // dq_var1
                            persObj->m_allTheFloats[22]); // dq_var2
   transObj->set_roiId   (persObj->m_allTheInts[0]);
   transObj->set_algoId  (persObj->m_algoId);
}


void MuonFeatureCnv_p3::transToPers(const MuonFeature* transObj, 
				          MuonFeature_p3* persObj, 
				          MsgStream &log)
{
   log << MSG::DEBUG << "MuonFeatureCnv_p3::transToPers called " << endreq;

   persObj->m_algoId           = transObj->algoId()    ; 
   persObj->m_allTheInts[0]    = transObj->roiId()     ; 
   persObj->m_allTheInts[1]    = transObj->saddress()  ;
   persObj->m_allTheFloats[0]  = transObj->pt()        ;
   persObj->m_allTheFloats[1]  = transObj->radius()    ;
   persObj->m_allTheFloats[2]  = transObj->eta()       ;
   persObj->m_allTheFloats[3]  = transObj->phi()       ;
   persObj->m_allTheFloats[4]  = transObj->dir_phi()   ;
   persObj->m_allTheFloats[5]  = transObj->zeta()      ;
   persObj->m_allTheFloats[6]  = transObj->dir_zeta()  ;
   persObj->m_allTheFloats[7]  = transObj->beta()      ;     
   persObj->m_allTheFloats[8]  = transObj->sp1_r()     ;     
   persObj->m_allTheFloats[9]  = transObj->sp1_z()     ;     
   persObj->m_allTheFloats[10] = transObj->sp1_slope() ;     
   persObj->m_allTheFloats[11] = transObj->sp2_r()     ;     
   persObj->m_allTheFloats[12] = transObj->sp2_z()     ;     
   persObj->m_allTheFloats[13] = transObj->sp2_slope() ;     
   persObj->m_allTheFloats[14] = transObj->sp3_r()     ;     
   persObj->m_allTheFloats[15] = transObj->sp3_z()     ;     
   persObj->m_allTheFloats[16] = transObj->sp3_slope() ;     
   persObj->m_allTheFloats[17] = transObj->br_radius() ;     
   persObj->m_allTheFloats[18] = transObj->br_sagitta();     
   persObj->m_allTheFloats[19] = transObj->ec_alpha()  ;
   persObj->m_allTheFloats[20] = transObj->ec_beta()   ;
   persObj->m_allTheFloats[21] = transObj->dq_var1()  ;
   persObj->m_allTheFloats[22] = transObj->dq_var2()  ;
}
