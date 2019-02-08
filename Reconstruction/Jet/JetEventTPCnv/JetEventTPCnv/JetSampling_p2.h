///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// JetSampling_p2.h 
// Header file for class JetSampling_p2
// Author: B.Salvachua < Belen.Salvachua@cern.ch> 
// Date:   Dec 2008
//    Based on JetSampling_p1
//    Add m_dr1, m_dr2, m_erad_tracks, m_pd
//    Move m_erad to m_erad_cells
//    Remove JetSum varialbes
//    Add more JetECS
//    Make them float
/////////////////////////////////////////////////////////////////// 
#ifndef JETEVENTTPCNV_JETSAMPLING_P2_H 
#define JETEVENTTPCNV_JETSAMPLING_P2_H 

// STL includes
//#include <vector>

// Boost includes
#include <boost/array.hpp>

// EventCommonTPCnv includes
#include "EventCommonTPCnv/HepLorentzVector_p1.h"

// forward declarations
class JetSamplingCnv_p2;

class JetSampling_p2 
{
public:
  /////////////////////////////////////////////////////////////////// 
  // public typedefs
  /////////////////////////////////////////////////////////////////// 

  typedef boost::array<float, 16> ECS_t;
  typedef boost::array<float, 10> ERad_t;

  /////////////////////////////////////////////////////////////////// 
  // Friend classes
  /////////////////////////////////////////////////////////////////// 

  // Make the AthenaPoolCnv class our friend
  friend class JetSamplingCnv_p2;

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
public: 

  /** Default constructor: 
   */
  JetSampling_p2();

  /** Destructor: 
   */
  ~JetSampling_p2() = default;

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  // don't bother make data private...

  /// Reconstructed jet at EM scale kinematics
  HepLorentzVector_p1 m_pr;

  /// Particle-In-Cone Truth jet kinematics
  HepLorentzVector_p1 m_pt;

  /// Nearest truth jet kinematics
  HepLorentzVector_p1 m_pn;

  // New HepLorentzVector for data driven analysis
  /// Data driven jet kinematics, could be any 4-mom vector used as reference for calibration
  HepLorentzVector_p1 m_pd;
  
  /// Calibrated jet kinematics (H1)
  HepLorentzVector_p1 m_h1;

  /// Calibrated jet kinematics (PISA)
  HepLorentzVector_p1 m_pisa;

  /// Calibbrated jet kinematics (samplings)
  HepLorentzVector_p1 m_samp;
  
  float m_dr1; //!> Distance to the 1st NTJ, if no found set to -1 
  float m_dr2; //!> Distance to the 2nd NTJ, if no found set to -1

  // Jet sampling layer info
  float m_ejsPreSamplerB;  //!> Energy deposited in layer PreSamplerB
  float m_ejsPreSamplerE;  //!> Energy deposited in layer PreSamlerE
  float m_ejsEMB1; //!> Energy deposited in layer EMB1
  float m_ejsEME1; //!> Energy deposited in layer EME1
  float m_ejsEMB2;//!> Energy deposited in layer EMB2
  float m_ejsEME2; //!> Energy deposited in layer EME2
  float m_ejsEMB3; //!> Energy deposited in layer EMB3
  float m_ejsEME3; //!> Energy deposited in layer EME3
  float m_ejsTileBar0; //!> Energy deposited in layer TileBar0
  float m_ejsTileExt0; //!> Energy deposited in layer TileExt0
  float m_ejsTileBar1; //!> Energy deposited in layer TileBar1
  float m_ejsTileExt1; //!> Energy deposited in layer TileExt1
  float m_ejsTileBar2; //!> Energy deposited in layer TileBar2
  float m_ejsTileExt2; //!> Energy deposited in layer TileExt2
  float m_ejsHEC0; //!> Energy deposited in layer HEC0
  float m_ejsHEC1; //!> Energy deposited in layer HEC1
  float m_ejsHEC2; //!> Energy deposited in layer HEC2
  float m_ejsHEC3; //!> Energy deposited in layer HEC3
  float m_ejsTileGap1; //!> Energy deposited in layer TileGap1
  float m_ejsTileGap2; //!> Energy deposited in layer TileGap2
  float m_ejsTileGap3; //!> Energy deposited in layer TileGap3
  float m_ejsFCAL0; //!> Energy deposited in layer FCAL0
  float m_ejsFCAL1; //!> Energy deposited in layer FCAL1
  float m_ejsFCAL2; //!> Energy deposited in layer FCAL2

  float m_tot;  //!> Total jet energy, used for checks
  float m_ctot; //!> used for checks 
  float m_ehad; //!> Deposited jet energy in hadronic calorimeters at EM scale
  float m_eem;  //!> Deposited jet energy in electromagnetic calorimeters at EM scale
  
  // JetSums: Variables used for H1 have been removed from JetSampling_p2
  // However they can be calculated using JetECS variables

  float m_eCryo; //!> Estimate of the energy in Cryostat
  float m_eGap;  //!> TileGap3 
  float m_eScint; //!> TileGap1+TileGap2
  float m_eNull;

  // JetECS, extended to finer granularity, cell enregy density bins, up to 16 elements 
  // Some of the old variables (JetSampling_p1) have been split to achieve more granularity 
  ECS_t m_ePreSamBCell; //!> PreSamplerB
  ECS_t m_ePreSamECell; //!> PreSamplerE
  ECS_t m_eEMB1Cell; //!> EMB1
  ECS_t m_eEMB2Cell1; //!> EMB2 eta < 0.8
  ECS_t m_eEMB2Cell2; //!> EMB2 eta >= 0.8
  ECS_t m_eEMB3Cell1; //!> EMB3 eta < 0.8
  ECS_t m_eEMB3Cell2; //!> EMB3 eta >= 0.8
  ECS_t m_eEME1Cell;  //!> EME1 
  ECS_t m_eEME2Cell1; //!> EME2 eta < 2.5
  ECS_t m_eEME2Cell2; //!> EME2 eta >= 2.5
  ECS_t m_eEME3Cell1; //!> EME3 eta < 2.5
  ECS_t m_eEME3Cell2; //!> EME3 eta >= 2.5
  ECS_t m_eTileBar0Cell; //!> TileBar0
  ECS_t m_eTileBar1Cell; //!> TileBar1
  ECS_t m_eTileBar2Cell; //!> TileBar2
  ECS_t m_eTileExt0Cell; //!> TileExt0
  ECS_t m_eTileExt1Cell; //!> TileExt1
  ECS_t m_eTileExt2Cell; //!> TileExt2
  ECS_t m_eHec0Cell1; //!> HEC0 eta < 2.5
  ECS_t m_eHec0Cell2; //!> HEC0 eta >= 2.5
  ECS_t m_eHec1Cell1; //!> HEC1 eta < 2.5
  ECS_t m_eHec1Cell2; //!> HEC1 eta >= 2.5
  ECS_t m_eHec2Cell1; //!> HEC2 eta < 2.5
  ECS_t m_eHec2Cell2; //!> HEC2 eta >= 2.5
  ECS_t m_eHec3Cell1; //!> HEC3 eta < 2.5
  ECS_t m_eHec3Cell2; //!> HEC3 eta >= 2.5
  ECS_t m_eFCal0Cell; //!> FCAL0 
  ECS_t m_eFCal1Cell; //!> FCAL1
  ECS_t m_eFCal2Cell; //!> FCAL2

  // Name in JetSampling_p2 have been changed, before it was called m_erad;
  // size 10
  ERad_t m_erad_cells;  //!> Energy in cone radii from calo cells

  // New variable to store enregy in cone radii calcualted from tracks (not in JetSampling_p1)
  // size 10
  ERad_t m_erad_tracks; //> Energy in cone radii, from tracks
}; 

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

#endif //> JETEVENTTPCNV_JETSAMPLING_P2_H
