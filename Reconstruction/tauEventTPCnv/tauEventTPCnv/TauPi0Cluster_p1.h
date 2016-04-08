/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @brief Declaration of persistent class for Analysis::TauPi0Cluster
 *
 * @author Felix Friedrich
 * @date November 2012
 *
 */

#ifndef TAUEVENTTPCNV_TAUPI0CLUSTER_P1_H
#define TAUEVENTTPCNV_TAUPI0CLUSTER_P1_H

//#include <vector>
#include "DataModelAthenaPool/ElementLink_p3.h"
//#include "DataModelAthenaPool/ElementLinkVector_p1.h"

/// forward declarations
class TauPi0ClusterCnv_p1;

/// persistent class version 1 for TauPi0Candidate
class TauPi0Cluster_p1
{
  public:
  TauPi0Cluster_p1() 
    : m_DELTA_PHI(0),
      m_DELTA_THETA(0),
      m_ENG_FRAC_EM(0),
      m_ENG_FRAC_MAX(0),
      m_ENG_FRAC_CORE(0),
      m_SECOND_ENG_DENS(0),
      m_LATERAL(0),
      m_LONGITUDINAL(0),
      m_EM1CoreFrac(0),
      m_asymmetryInEM1WRTTrk(0),
      m_NHitsInEM1(0),
      m_NPosECells_PS(0),
      m_NPosECells_EM1(0),
      m_NPosECells_EM2(0),
      m_firstEtaWRTClusterPosition_EM1(0),
      m_firstEtaWRTClusterPosition_EM2(0),
      m_secondEtaWRTClusterPosition_EM1(0),
      m_secondEtaWRTClusterPosition_EM2(0),
      m_BDTScore(0)
  {};
  
  /// necessary to have member data private
  friend class TauPi0ClusterCnv_p1;

  private:
  //ElementLinkIntVector_p1 m_CaloClusterVector;
  ElementLinkInt_p3 m_cluster;
  std::vector< TPObjRef > m_tauShotVector;
  
  //std::vector<float> m_clusterValueVector1;
  float m_DELTA_PHI;
  float m_DELTA_THETA; 
  float m_ENG_FRAC_EM;
  float m_ENG_FRAC_MAX;
  float m_ENG_FRAC_CORE;
  float m_SECOND_ENG_DENS;
  float m_LATERAL;
  float m_LONGITUDINAL;
  float m_EM1CoreFrac; 
  float m_asymmetryInEM1WRTTrk;
  int   m_NHitsInEM1;
  int   m_NPosECells_PS;
  int   m_NPosECells_EM1;
  int   m_NPosECells_EM2;
  float m_firstEtaWRTClusterPosition_EM1;
  float m_firstEtaWRTClusterPosition_EM2;
  float m_secondEtaWRTClusterPosition_EM1;
  float m_secondEtaWRTClusterPosition_EM2;
  float m_BDTScore;
};


#endif
