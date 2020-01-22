/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TBTPCnv/TBTrackCnv_p1.h"
#include "TBEvent/TBTrack.h"
#include "Identifier/Identifier.h"
#include "TBTPCnv/TBTrack_p1.h"



void
TBTrackCnv_p1::persToTrans(const TBTrack_p1* pers, 
                                     TBTrack* trans, MsgStream &/*log*/) const
{
  *trans = TBTrack (pers->m_residualu,
                    pers->m_residualv);

  trans->setChi2       (pers->m_chi2);
  trans->setChi2_u     (pers->m_chi2u);
  trans->setChi2_v     (pers->m_chi2v);
  trans->setAngle      (pers->m_angle);
  trans->setUslope     (pers->m_uslope);
  trans->setVslope     (pers->m_vslope);
  trans->setUintercept (pers->m_uintercept);
  trans->setVintercept (pers->m_vintercept);
  trans->setCryoHitu   (pers->m_cryou);
  trans->setCryoHitv   (pers->m_cryov);
  trans->setCryoHitw   (pers->m_cryow);
}


void
TBTrackCnv_p1::transToPers(const TBTrack* trans, 
                                     TBTrack_p1* pers, MsgStream &/*log*/) const
{
  pers->m_hitNumberU  = trans->getHitNumberU();
  pers->m_hitNumberV  = trans->getHitNumberV();

  pers->m_residualu   = trans->getResidualu();
  pers->m_residualv   = trans->getResidualv();

  pers->m_chi2        = trans->getChi2_global();
  pers->m_chi2u       = trans->getChi2_u();
  pers->m_chi2v       = trans->getChi2_v();
  pers->m_angle       = trans->getAngle();
  pers->m_uslope      = trans->getUslope();
  pers->m_vslope      = trans->getVslope();
  pers->m_uintercept  = trans->getUintercept();
  pers->m_vintercept  = trans->getVintercept();
  pers->m_cryou       = trans->getCryoHitu();
  pers->m_cryov       = trans->getCryoHitv();
  pers->m_cryow       = trans->getCryoHitw();
  
}

