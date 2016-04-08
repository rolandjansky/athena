/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMuonEvent/TileMuFeature.h"
#include "TrigMuonEventTPCnv/TileMuFeature_p1.h"
#include "TrigMuonEventTPCnv/TileMuFeatureCnv_p1.h"

void TileMuFeatureCnv_p1::persToTrans(const TileMuFeature_p1* persObj,
                                            TileMuFeature* transObj,
                                            MsgStream &log)
{
   log << MSG::DEBUG << "TileMuFeatureCnv_p1::persToTrans called " << endreq;

   *transObj = TileMuFeature (persObj->m_eta,
                              persObj->m_phi,
                              persObj->m_energy_deposited,
                              persObj->m_quality_factor);
}

void TileMuFeatureCnv_p1::transToPers(const TileMuFeature* transObj,
                                            TileMuFeature_p1* persObj,
                                            MsgStream &log)
{
   log << MSG::DEBUG << "TileMuFeatureCnv_p1::transToPers called " << endreq;

   persObj->m_eta      = transObj->eta();
   persObj->m_phi      = transObj->phi();
   persObj->m_energy_deposited = transObj->enedep();
   persObj->m_quality_factor   = transObj->quality();
}

