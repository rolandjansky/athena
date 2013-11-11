/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////
// Implementation of the TP converter
//////////////////////////////////////////////////////

//trick to access private members in TauPi0Cluster
//TODO: should be avoided
//see: https://twiki.cern.ch/twiki/bin/viewauth/Atlas/TransientPersistentSeparation#Avoiding_the_define_private_publ
#define private public
#define protected public
#include "tauEvent/TauPi0Cluster.h"
#undef private
#undef protected

#include "tauEventTPCnv/TauPi0ClusterCnv_p1.h"

void TauPi0ClusterCnv_p1::persToTrans(const TauPi0Cluster_p1 *persObj, Analysis::TauPi0Cluster *transObj, MsgStream &log)
{
  /// links to CaloClusters
  //m_clusterVectCnv.persToTrans( &persObj->m_CaloClusterVector, &transObj->m_CaloClusterVector, log );
  m_clusterCnv.persToTrans( &persObj->m_cluster, &transObj->m_cluster, log );
  m_vecTauShotCnv.persToTrans( &persObj->m_tauShotVector, &(transObj->m_tauShotVector), log );
  
  /// simple standard member data
  //transObj->m_clusterValueVector1 = persObj->m_clusterValueVector1;
  transObj->m_DELTA_PHI                           = persObj->m_DELTA_PHI;                        
  transObj->m_DELTA_THETA                         = persObj->m_DELTA_THETA;                      
  transObj->m_ENG_FRAC_EM                         = persObj->m_ENG_FRAC_EM;                      
  transObj->m_ENG_FRAC_MAX                        = persObj->m_ENG_FRAC_MAX;
  transObj->m_ENG_FRAC_CORE                       = persObj->m_ENG_FRAC_CORE;                    
  transObj->m_SECOND_ENG_DENS                     = persObj->m_SECOND_ENG_DENS;                 
  transObj->m_LATERAL                             = persObj->m_LATERAL;
  transObj->m_LONGITUDINAL                        = persObj->m_LONGITUDINAL; 
  transObj->m_EM1CoreFrac                         = persObj->m_EM1CoreFrac;                      
  transObj->m_asymmetryInEM1WRTTrk                = persObj->m_asymmetryInEM1WRTTrk;             
  transObj->m_NHitsInEM1                          = persObj->m_NHitsInEM1;                       
  transObj->m_NPosECells_PS                       = persObj->m_NPosECells_PS;                    
  transObj->m_NPosECells_EM1                      = persObj->m_NPosECells_EM1;                   
  transObj->m_NPosECells_EM2                      = persObj->m_NPosECells_EM2;                   
  transObj->m_firstEtaWRTClusterPosition_EM1      = persObj->m_firstEtaWRTClusterPosition_EM1;   
  transObj->m_firstEtaWRTClusterPosition_EM2      = persObj->m_firstEtaWRTClusterPosition_EM2;   
  transObj->m_secondEtaWRTClusterPosition_EM1     = persObj->m_secondEtaWRTClusterPosition_EM1;  
  transObj->m_secondEtaWRTClusterPosition_EM2     = persObj->m_secondEtaWRTClusterPosition_EM2;  
  transObj->m_BDTScore                            = persObj->m_BDTScore;
}

void TauPi0ClusterCnv_p1::transToPers(const Analysis::TauPi0Cluster *transObj, TauPi0Cluster_p1 *persObj, MsgStream &log)
{
  /// links to CaloClusters
  //m_clusterVectCnv.transToPers( &transObj->m_CaloClusterVector, &persObj->m_CaloClusterVector, log );
  m_clusterCnv.transToPers( &transObj->m_cluster, &persObj->m_cluster, log );
  m_vecTauShotCnv.transToPers( &(transObj->m_tauShotVector), &persObj->m_tauShotVector, log );
  
  /// simple standard member data
  //persObj->m_clusterValueVector1 = transObj->m_clusterValueVector1;
  persObj->m_DELTA_PHI                           = transObj->m_DELTA_PHI;                        
  persObj->m_DELTA_THETA                         = transObj->m_DELTA_THETA;                      
  persObj->m_ENG_FRAC_EM                         = transObj->m_ENG_FRAC_EM;                     
  persObj->m_ENG_FRAC_MAX                        = transObj->m_ENG_FRAC_MAX; 
  persObj->m_ENG_FRAC_CORE                       = transObj->m_ENG_FRAC_CORE;                    
  persObj->m_SECOND_ENG_DENS                     = transObj->m_SECOND_ENG_DENS;                  
  persObj->m_LATERAL                             = transObj->m_LATERAL;
  persObj->m_LONGITUDINAL                        = transObj->m_LONGITUDINAL;
  persObj->m_EM1CoreFrac                         = transObj->m_EM1CoreFrac;                      
  persObj->m_asymmetryInEM1WRTTrk                = transObj->m_asymmetryInEM1WRTTrk;             
  persObj->m_NHitsInEM1                          = transObj->m_NHitsInEM1;                       
  persObj->m_NPosECells_PS                       = transObj->m_NPosECells_PS;                    
  persObj->m_NPosECells_EM1                      = transObj->m_NPosECells_EM1;                   
  persObj->m_NPosECells_EM2                      = transObj->m_NPosECells_EM2;                   
  persObj->m_firstEtaWRTClusterPosition_EM1      = transObj->m_firstEtaWRTClusterPosition_EM1;   
  persObj->m_firstEtaWRTClusterPosition_EM2      = transObj->m_firstEtaWRTClusterPosition_EM2;   
  persObj->m_secondEtaWRTClusterPosition_EM1     = transObj->m_secondEtaWRTClusterPosition_EM1;  
  persObj->m_secondEtaWRTClusterPosition_EM2     = transObj->m_secondEtaWRTClusterPosition_EM2;
  persObj->m_BDTScore                            = transObj->m_BDTScore;
}


