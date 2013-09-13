/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "TrigMuonEvent/TileMuFeature.h"
#include "TrigMuonEventTPCnv/TileMuFeature_p1.h"
#undef private
#undef protected

#include "TrigMuonEventTPCnv/TileMuFeatureCnv_p1.h"

void TileMuFeatureCnv_p1::persToTrans(const TileMuFeature_p1* persObj,
                                            TileMuFeature* transObj,
                                            MsgStream &log)
{
   log << MSG::DEBUG << "TileMuFeatureCnv_p1::persToTrans called " << endreq;

   transObj->m_eta	= persObj->m_eta;
   transObj->m_phi 	= persObj->m_phi;
   for(int i=0; i<4; i++){
     (transObj->m_energy_deposited).push_back( 
		(persObj->m_energy_deposited).at(i));
   }
   transObj->m_quality_factor 	= persObj->m_quality_factor;
   //transObj->m_Pt_IDTrk 	= persObj->m_Pt_IDTrk;
   //transObj->m_Eta_IDTrk 	= persObj->m_Eta_IDTrk;
   //transObj->m_Phi_IDTrk	= persObj->m_Phi_IDTrk; 
   //transObj->m_PhiTR_IDTrk	= persObj->m_PhiTR_IDTrk; 
   //transObj->m_zPos_IDTrk 	= persObj->m_zPos_IDTrk;

   //transObj->m_IDTrk		= persObj->m_IDTrk;	
}

void TileMuFeatureCnv_p1::transToPers(const TileMuFeature* transObj,
                                            TileMuFeature_p1* persObj,
                                            MsgStream &log)
{
   log << MSG::DEBUG << "TileMuFeatureCnv_p1::transToPers called " << endreq;

   persObj->m_eta      = transObj->m_eta;
   persObj->m_phi      = transObj->m_phi;
   for(int i=0; i<4; i++){
     (persObj->m_energy_deposited).push_back(
                (transObj->m_energy_deposited).at(i));
   }
   persObj->m_quality_factor   = transObj->m_quality_factor;
   //persObj->m_Pt_IDTrk         = transObj->m_Pt_IDTrk;
   //persObj->m_Eta_IDTrk        = transObj->m_Eta_IDTrk;
   //persObj->m_Phi_IDTrk        = transObj->m_Phi_IDTrk;
   //persObj->m_PhiTR_IDTrk      = transObj->m_PhiTR_IDTrk;
   //persObj->m_zPos_IDTrk       = transObj->m_zPos_IDTrk;

   //persObj->m_IDTrk            = transObj->m_IDTrk;
}

