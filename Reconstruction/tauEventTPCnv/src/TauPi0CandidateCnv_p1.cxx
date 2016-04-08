/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////
// Implementation of the TP converter
//////////////////////////////////////////////////////

#include "tauEvent/TauPi0Candidate.h"
#include "tauEventTPCnv/TauPi0CandidateCnv_p1.h"

void TauPi0CandidateCnv_p1::persToTrans(const TauPi0Candidate_p1 *persObj, Analysis::TauPi0Candidate *transObj, MsgStream &log)
{
  m_vecTauPi0ClusterCnv.persToTrans( &persObj->m_pi0ClusterVector, &(transObj->pi0ClusterVec()), log );
}

void TauPi0CandidateCnv_p1::transToPers(const Analysis::TauPi0Candidate *transObj, TauPi0Candidate_p1 *persObj, MsgStream &log)
{
  m_vecTauPi0ClusterCnv.transToPers( &(transObj->pi0ClusterVec()), &persObj->m_pi0ClusterVector, log );

}


