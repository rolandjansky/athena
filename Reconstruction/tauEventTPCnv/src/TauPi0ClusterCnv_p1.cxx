/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////
// Implementation of the TP converter
//////////////////////////////////////////////////////

#include "tauEvent/TauPi0Cluster.h"
#include "tauEventTPCnv/TauPi0ClusterCnv_p1.h"

void TauPi0ClusterCnv_p1::persToTrans(const TauPi0Cluster_p1 *persObj, Analysis::TauPi0Cluster *transObj, MsgStream &log)
{
  /// links to CaloClusters
  //m_clusterVectCnv.persToTrans( &persObj->m_CaloClusterVector, &transObj->m_CaloClusterVector, log );
  m_clusterCnv.persToTrans( &persObj->m_cluster, &transObj->clusterLink(), log );
  m_vecTauShotCnv.persToTrans( &persObj->m_tauShotVector, &(transObj->tauShotVec()), log );
  
  /// simple standard member data
  //transObj->m_clusterValueVector1 = persObj->m_clusterValueVector1;
  transObj->setDeltaPhi                           (persObj->m_DELTA_PHI);
  transObj->setDeltaTheta                         (persObj->m_DELTA_THETA);
  transObj->setEngFracEM                          (persObj->m_ENG_FRAC_EM);
  transObj->setEngFracMAX                         (persObj->m_ENG_FRAC_MAX);
  transObj->setEngFracCore                        (persObj->m_ENG_FRAC_CORE);
  transObj->setSecondEngDens                      (persObj->m_SECOND_ENG_DENS);
  transObj->setLATERAL                            (persObj->m_LATERAL);
  transObj->setLONGITUDINAL                       (persObj->m_LONGITUDINAL);
  transObj->setEM1CoreFrac                        (persObj->m_EM1CoreFrac);
  transObj->setAsymmetryInEM1WRTTrk               (persObj->m_asymmetryInEM1WRTTrk);
  transObj->setNHitsInEM1                         (persObj->m_NHitsInEM1);
  transObj->setNPosECells_PS                      (persObj->m_NPosECells_PS);
  transObj->setNPosECells_EM1                     (persObj->m_NPosECells_EM1);
  transObj->setNPosECells_EM2                     (persObj->m_NPosECells_EM2);
  transObj->setFirstEtaWRTClusterPosition_EM1     (persObj->m_firstEtaWRTClusterPosition_EM1);
  transObj->setFirstEtaWRTClusterPosition_EM2     (persObj->m_firstEtaWRTClusterPosition_EM2);
  transObj->setSecondEtaWRTClusterPosition_EM1    (persObj->m_secondEtaWRTClusterPosition_EM1);
  transObj->setSecondEtaWRTClusterPosition_EM2    (persObj->m_secondEtaWRTClusterPosition_EM2);
  transObj->setBDTScore                           (persObj->m_BDTScore);
}

void TauPi0ClusterCnv_p1::transToPers(const Analysis::TauPi0Cluster *transObj, TauPi0Cluster_p1 *persObj, MsgStream &log)
{
  /// links to CaloClusters
  //m_clusterVectCnv.transToPers( &transObj->m_CaloClusterVector, &persObj->m_CaloClusterVector, log );
  m_clusterCnv.transToPers( &transObj->clusterLink(), &persObj->m_cluster, log );
  m_vecTauShotCnv.transToPers( &(transObj->tauShotVec()), &persObj->m_tauShotVector, log );
  
  /// simple standard member data
  //persObj->m_clusterValueVector1 = transObj->m_clusterValueVector1;
  persObj->m_DELTA_PHI                           = transObj->deltaPhi();
  persObj->m_DELTA_THETA                         = transObj->deltaTheta();
  persObj->m_ENG_FRAC_EM                         = transObj->engFracEM();
  persObj->m_ENG_FRAC_MAX                        = transObj->engFracMAX();
  persObj->m_ENG_FRAC_CORE                       = transObj->engFracCore();
  persObj->m_SECOND_ENG_DENS                     = transObj->secondEngDens();
  persObj->m_LATERAL                             = transObj->LATERAL();
  persObj->m_LONGITUDINAL                        = transObj->LONGITUDINAL();
  persObj->m_EM1CoreFrac                         = transObj->EM1CoreFrac();
  persObj->m_asymmetryInEM1WRTTrk                = transObj->asymmetryInEM1WRTTrk();             
  persObj->m_NHitsInEM1                          = transObj->NHitsInEM1();
  persObj->m_NPosECells_PS                       = transObj->NPosECells_PS();
  persObj->m_NPosECells_EM1                      = transObj->NPosECells_EM1();
  persObj->m_NPosECells_EM2                      = transObj->NPosECells_EM2();
  persObj->m_firstEtaWRTClusterPosition_EM1      = transObj->firstEtaWRTClusterPosition_EM1();   
  persObj->m_firstEtaWRTClusterPosition_EM2      = transObj->firstEtaWRTClusterPosition_EM2();
  persObj->m_secondEtaWRTClusterPosition_EM1     = transObj->secondEtaWRTClusterPosition_EM1();
  persObj->m_secondEtaWRTClusterPosition_EM2     = transObj->secondEtaWRTClusterPosition_EM2();
  persObj->m_BDTScore                            = transObj->BDTScore();
}


