/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////
// Implementation of the TP converter
//////////////////////////////////////////////////////

//trick to access private members in TauPi0Candidate
//TODO: should be avoided
//see: https://twiki.cern.ch/twiki/bin/viewauth/Atlas/TransientPersistentSeparation#Avoiding_the_define_private_publ
#define private public
#define protected public
#include "tauEvent/TauPi0Candidate.h"
#undef private
#undef protected

#include "tauEventTPCnv/TauPi0CandidateCnv_p1.h"

void TauPi0CandidateCnv_p1::persToTrans(const TauPi0Candidate_p1 *persObj, Analysis::TauPi0Candidate *transObj, MsgStream &log)
{
    m_vecTauPi0ClusterCnv.persToTrans( &persObj->m_pi0ClusterVector, &(transObj->m_pi0ClusterVector), log );
}

void TauPi0CandidateCnv_p1::transToPers(const Analysis::TauPi0Candidate *transObj, TauPi0Candidate_p1 *persObj, MsgStream &log)
{
    m_vecTauPi0ClusterCnv.transToPers( &(transObj->m_pi0ClusterVector), &persObj->m_pi0ClusterVector, log );

}


