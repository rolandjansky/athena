///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// JetSampling_p1.h 
// Header file for class JetSampling_p1
// Author: S.Binet<binet@cern.ch>
// Date:   July 2007
/////////////////////////////////////////////////////////////////// 
#ifndef JETEVENTTPCNV_JETSAMPLING_P1_H 
#define JETEVENTTPCNV_JETSAMPLING_P1_H 

// STL includes
//#include <vector>

// Boost includes
#include <boost/array.hpp>

// EventCommonTPCnv includes
#include "EventCommonTPCnv/HepLorentzVector_p1.h"

// forward declarations
class JetSamplingCnv_p1;

class JetSampling_p1 
{
public:
  /////////////////////////////////////////////////////////////////// 
  // public typedefs
  /////////////////////////////////////////////////////////////////// 

  typedef boost::array<double, 2>  JS_t;
  typedef boost::array<double, 16> ECS_t;
  typedef boost::array<double, 10> ERad_t;

  /////////////////////////////////////////////////////////////////// 
  // Friend classes
  /////////////////////////////////////////////////////////////////// 

  // Make the AthenaPoolCnv class our friend
  friend class JetSamplingCnv_p1;

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
public: 

  /** Default constructor: 
   */
  JetSampling_p1();

  /** Destructor: 
   */
  ~JetSampling_p1() = default;

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  // don't bother make data private...

  /// Reco jet kinematics
  HepLorentzVector_p1 m_pr;

  /// Truth jet kinematics
  HepLorentzVector_p1 m_pt;

  /// Nearest truth jet kinematics
  HepLorentzVector_p1 m_pn;
  
  /// Calib jet kinematics (H1)
  HepLorentzVector_p1 m_h1;

  /// Calib jet kinematics (PISA)
  HepLorentzVector_p1 m_pisa;

  /// Calib jet kinematics (samplings)
  HepLorentzVector_p1 m_samp;
  
  // Jet sampling layer info
  double m_ejsPreSamplerB;
  double m_ejsPreSamplerE;
  double m_ejsEMB1;
  double m_ejsEME1;  
  double m_ejsEMB2;
  double m_ejsEME2;
  double m_ejsEMB3;
  double m_ejsEME3;
  double m_ejsTileBar0;
  double m_ejsTileExt0;
  double m_ejsTileBar1;
  double m_ejsTileExt1;
  double m_ejsTileBar2;
  double m_ejsTileExt2;  
  double m_ejsHEC0;
  double m_ejsHEC1;
  double m_ejsHEC2;
  double m_ejsHEC3;
  double m_ejsTileGap1;
  double m_ejsTileGap2;
  double m_ejsTileGap3;
  double m_ejsFCAL0;
  double m_ejsFCAL1;
  double m_ejsFCAL2;

  double m_tot;
  double m_ctot;
  double m_ehad;
  double m_eem;
  
  // JetSums 
  JS_t  m_edEMB0Cell;
  ECS_t m_edEMB1Cell;
  ECS_t m_edEMB2Cell;
  JS_t  m_edEME0Cell;
  ECS_t m_edEME1Cell;
  ECS_t m_edEME2Cell;
  ECS_t m_edTile1Cell;
  ECS_t m_edTile2Cell;
  ECS_t m_edHec1Cell;
  ECS_t m_edHec2Cell;
  ECS_t m_edFCal1Cell;
  ECS_t m_edFCal2Cell;
  double m_eCryo;
  double m_eGap;
  double m_eScint;
  double m_eNull;

  // JetECS
  ECS_t m_ePreSamBCell;
  ECS_t m_ePreSamECell;
  ECS_t m_eEMB1Cell;
  ECS_t m_eEMB2Cell;
  ECS_t m_eEMB3Cell;
  ECS_t m_eEME1Cell;
  ECS_t m_eEME2Cell;
  ECS_t m_eEME3Cell;
  ECS_t m_eTileBar0Cell;
  ECS_t m_eTileBar1Cell;
  ECS_t m_eTileBar2Cell;
  ECS_t m_eTileExt0Cell;
  ECS_t m_eTileExt1Cell;
  ECS_t m_eTileExt2Cell;
  ECS_t m_eHec0Cell;
  ECS_t m_eHec1Cell;
  ECS_t m_eHec2Cell;
  ECS_t m_eHec3Cell;
  ECS_t m_eFCal0Cell;
  ECS_t m_eFCal1Cell;
  ECS_t m_eFCal2Cell;

  // Energy in cone radii
  // static const int m_rsiz = 10;
  ERad_t m_erad;
}; 

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

#endif //> JETEVENTTPCNV_JETSAMPLING_P1_H
