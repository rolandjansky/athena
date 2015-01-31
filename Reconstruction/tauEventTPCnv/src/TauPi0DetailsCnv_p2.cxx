/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////
// Implementation of the TP converter
//////////////////////////////////////////////////////

//trick to access private members in TauPi0Details
//TODO: should be avoided
//see: https://twiki.cern.ch/twiki/bin/viewauth/Atlas/TransientPersistentSeparation#Avoiding_the_define_private_publ
#define private public
#define protected public
#include "tauEvent/TauPi0Details.h"
#undef private
#undef protected

#include "tauEventTPCnv/TauPi0DetailsCnv_p2.h"

void TauPi0DetailsCnv_p2::persToTrans(const TauPi0Details_p2 *persObj, Analysis::TauPi0Details *transObj, MsgStream &log)
{
  m_vecTauPi0ClusterCnv.persToTrans( &persObj->m_pi0ClusContainer, &(transObj->m_pi0ClusContainer), log );
  m_vecTauPi0CandidateCnv.persToTrans( &persObj->m_pi0CandContainer, &(transObj->m_pi0CandContainer), log );
  m_vecTauShotCnv.persToTrans( &persObj->m_shotContainer, &(transObj->m_shotContainer), log );
  transObj->m_visTau_hlv = persObj->m_visTau_hlv;

}

void TauPi0DetailsCnv_p2::transToPers(const Analysis::TauPi0Details *transObj, TauPi0Details_p2 *persObj, MsgStream &log)
{
  m_vecTauPi0ClusterCnv.transToPers( &(transObj->m_pi0ClusContainer), &persObj->m_pi0ClusContainer, log );
  m_vecTauPi0CandidateCnv.transToPers( &(transObj->m_pi0CandContainer), &persObj->m_pi0CandContainer, log );
  m_vecTauShotCnv.transToPers( &(transObj->m_shotContainer), &persObj->m_shotContainer, log );
  persObj->m_visTau_hlv = transObj->m_visTau_hlv;
}

