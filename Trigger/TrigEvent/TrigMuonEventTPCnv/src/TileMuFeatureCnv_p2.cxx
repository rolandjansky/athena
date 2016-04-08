/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMuonEvent/TileMuFeature.h"
#include "TrigMuonEventTPCnv/TileMuFeature_p2.h"
#include "TrigMuonEventTPCnv/TileMuFeatureCnv_p2.h"

void TileMuFeatureCnv_p2::persToTrans(const TileMuFeature_p2* persObj,
                                            TileMuFeature* transObj,
                                            MsgStream &log)
{
   log << MSG::DEBUG << "TileMuFeatureCnv_p2::persToTrans called " << endreq;

   *transObj = TileMuFeature (persObj->m_allTheFloats[0], // eta
                              persObj->m_allTheFloats[1], // phi
                              persObj->m_energy_deposited,
                              persObj->m_allTheFloats[2]); // qual
}

void TileMuFeatureCnv_p2::transToPers(const TileMuFeature* transObj,
                                            TileMuFeature_p2* persObj,
                                            MsgStream &log)
{
   log << MSG::DEBUG << "TileMuFeatureCnv_p2::transToPers called " << endreq;

   persObj->m_allTheFloats[0]      = transObj->eta();
   persObj->m_allTheFloats[1]      = transObj->phi();
   persObj->m_energy_deposited = transObj->enedep();
   persObj->m_allTheFloats[2]    = transObj->quality();
}

