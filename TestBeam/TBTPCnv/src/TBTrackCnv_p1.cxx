/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TBTPCnv/TBTrackCnv_p1.h"
#define private public
#define protected public
#include "TBEvent/TBTrack.h"

#undef private
#undef protected
#include "Identifier/Identifier.h"
#include "TBTPCnv/TBTrack_p1.h"



void
TBTrackCnv_p1::persToTrans(const TBTrack_p1* pers, 
                                     TBTrack* trans, MsgStream &/*log*/)
{

  trans->m_hitNumberU = pers->m_hitNumberU;
  trans->m_hitNumberV = pers->m_hitNumberV;

  unsigned int nResidualus = pers->m_residualu.size();
  trans->m_residualu.reserve(nResidualus);
  trans->m_residualu  = pers->m_residualu;

  unsigned int nResidualvs = pers->m_residualv.size();
  trans->m_residualv.reserve(nResidualvs);
  trans->m_residualv  = pers->m_residualv;

  trans->m_chi2       = pers->m_chi2;
  trans->m_chi2u      = pers->m_chi2u;
  trans->m_chi2v      = pers->m_chi2v;
  trans->m_angle      = pers->m_angle;
  trans->m_uslope     = pers->m_uslope;
  trans->m_vslope     = pers->m_vslope;
  trans->m_uintercept = pers->m_uintercept;
  trans->m_vintercept = pers->m_vintercept;
  trans->m_cryou      = pers->m_cryou;
  trans->m_cryov      = pers->m_cryov;
  trans->m_cryow      = pers->m_cryow;

}


void
TBTrackCnv_p1::transToPers(const TBTrack* trans, 
                                     TBTrack_p1* pers, MsgStream &/*log*/)
{

  pers->m_hitNumberU  = trans->m_hitNumberU;
  pers->m_hitNumberV  = trans->m_hitNumberV;

  unsigned int nResidualus = trans->m_residualu.size();
  pers->m_residualu.reserve(nResidualus);
  pers->m_residualu   = trans->m_residualu;

  unsigned int nResidualvs = trans->m_residualv.size();
  pers->m_residualv.reserve(nResidualvs);
  pers->m_residualv   = trans->m_residualv;

  pers->m_chi2        = trans->m_chi2;
  pers->m_chi2u       = trans->m_chi2u;
  pers->m_chi2v       = trans->m_chi2v;
  pers->m_angle       = trans->m_angle;
  pers->m_uslope      = trans->m_uslope;
  pers->m_vslope      = trans->m_vslope;
  pers->m_uintercept  = trans->m_uintercept;
  pers->m_vintercept  = trans->m_vintercept;
  pers->m_cryou       = trans->m_cryou;
  pers->m_cryov       = trans->m_cryov;
  pers->m_cryow       = trans->m_cryow;

  
}

