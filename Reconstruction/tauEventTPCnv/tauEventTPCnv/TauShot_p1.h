/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @brief Declaration of persistent class for Analysis::TauShot
 *
 * @author Felix Friedrich
 * @date November 2012
 *
 */

#ifndef TAUEVENTTPCNV_TAUSHOT_P1_H
#define TAUEVENTTPCNV_TAUSHOT_P1_H

//#include <vector>
#include "DataModelAthenaPool/ElementLink_p3.h"
//#include "DataModelAthenaPool/ElementLinkVector_p1.h"
#include "EventCommonTPCnv/P4EEtaPhiMFloat_p2.h"

/// forward declarations
class TauShotCnv_p1;

/// persistent class version 1 for TauPi0Candidate
class TauShot_p1
{
  public:
  TauShot_p1()
    // Set default values for ints or floats here
    // : m_DELTA_PHI(0)
  {};
  
  /// necessary to have member data private
  friend class TauShotCnv_p1;

  private:
  //ElementLinkIntVector_p1 m_CaloClusterVector;
  ElementLinkInt_p3 m_cluster;
  ElementLinkInt_p3 m_seedCell;
  P4EEtaPhiMFloat_p2 m_P4EEtaPhiM; 
   
  //std::vector<float> m_clusterValueVector1;
  // declare ints or floats here
  int m_nCellsInEta=-1111;
  float m_pt1=-1111;
  float m_pt3=-1111;
  float m_pt5=-1111;
  float m_ws5=-1111;
  float m_sdevEta5_WRTmean=-1111;
  float m_sdevEta5_WRTmode=-1111;
  float m_sdevPt5=-1111;
  float m_deltaPt12_min=-1111;
  float m_Fside_3not1=-1111;
  float m_Fside_5not1=-1111;
  float m_Fside_5not3=-1111;
  float m_fracSide_3not1=-1111;
  float m_fracSide_5not1=-1111;
  float m_fracSide_5not3=-1111;
  float m_pt1OverPt3=-1111;
  float m_pt3OverPt5=-1111;
  float m_mergedScore=-1111;
  float m_signalScore=-1111;
  int m_nPhotons=-1111;
};


#endif
