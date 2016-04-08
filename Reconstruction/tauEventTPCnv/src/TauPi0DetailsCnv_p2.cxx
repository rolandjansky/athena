/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////
// Implementation of the TP converter
//////////////////////////////////////////////////////

#include "tauEvent/TauPi0Details.h"
#include "tauEventTPCnv/TauPi0DetailsCnv_p2.h"

void TauPi0DetailsCnv_p2::persToTrans(const TauPi0Details_p2 *persObj, Analysis::TauPi0Details *transObj, MsgStream &log)
{
  m_vecTauPi0ClusterCnv.persToTrans( &persObj->m_pi0ClusContainer, &(transObj->pi0ClusterVector()), log );
  m_vecTauPi0CandidateCnv.persToTrans( &persObj->m_pi0CandContainer, &(transObj->pi0CandidateVector()), log );
  m_vecTauShotCnv.persToTrans( &persObj->m_shotContainer, &(transObj->shotVector()), log );
  transObj->setVisTauhlv (persObj->m_visTau_hlv);

}

void TauPi0DetailsCnv_p2::transToPers(const Analysis::TauPi0Details *transObj, TauPi0Details_p2 *persObj, MsgStream &log)
{
  m_vecTauPi0ClusterCnv.transToPers( &(transObj->pi0ClusterVector()), &persObj->m_pi0ClusContainer, log );
  m_vecTauPi0CandidateCnv.transToPers( &(transObj->pi0CandidateVector()), &persObj->m_pi0CandContainer, log );
  m_vecTauShotCnv.transToPers( &(transObj->shotVector()), &persObj->m_shotContainer, log );
  persObj->m_visTau_hlv = transObj->visTauhlv();
}

