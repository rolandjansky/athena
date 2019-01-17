///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// Muon_p6.h 
// Header file for class Muon_p6
// Author: Ketevi A. Assamagan <ketevi@bnl.gov>
// Date:  June 2008 

/////////////////////////////////////////////////////////////////// 
#ifndef RECTPCNV_MUON_P6_H 
#define RECTPCNV_MUON_P6_H 1

// STL includes
#include <inttypes.h>
#include <vector>

// DataModelAthenaPool includes
#include "DataModelAthenaPool/ElementLink_p3.h"
#include "DataModelAthenaPool/ElementLinkVector_p1.h"

// EventCommonTPCnv includes
#include "EventCommonTPCnv/P4IPtCotThPhiM_p1.h"

// ParticleEventTPCnv includes
#include "ParticleEventTPCnv/ParticleBase_p2.h"

// forward declarations
class MuonCnv_p6;

class Muon_p6 
{
  /////////////////////////////////////////////////////////////////// 
  // Friend classes
  /////////////////////////////////////////////////////////////////// 

  // Make the AthenaPoolCnv class our friend
  friend class MuonCnv_p6;

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
public: 

  /** Default constructor: 
   */
  Muon_p6();

  /** Destructor: 
   */
  ~Muon_p6() = default;

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  /////////////////////////////////////////////////////////////////// 
  // Private data: 
  /////////////////////////////////////////////////////////////////// 
private: 

  /// the 4-mom part
  P4IPtCotThPhiM_p1 m_momentum;

  /// the ParticleBase part
  ParticleBase_p2 m_particleBase;

  // the Muon part 

  /// author
  unsigned int m_author;

  /// switches
  bool m_hasCombinedMuon;                   // needed ?
  bool m_hasInDetTrackParticle;             // needed ?
  bool m_hasMuonExtrapolatedTrackParticle;  // needed ?
  bool m_hasInnerExtrapolatedTrackParticle; // needed ?
  bool m_hasCombinedMuonTrackParticle;      // needed ?

  // not used ?
  bool m_hasCluster;

  /// Chi2 of the track matching
  float m_matchChi2;

  /// Low Pt muon stuff
  std::vector<int> m_associatedEtaDigits;
  std::vector<int> m_associatedPhiDigits;
  //<--

  /// link to associated TrackParticle (Inner Detector)
  ElementLinkInt_p3 m_inDetTrackParticle;
 
  /// link to the Trk::Segment
  ElementLinkIntVector_p1 m_muonSegments;
 
  /// link to associated TrackParticle (Muon Spectrometer)
  ElementLinkInt_p3 m_muonSpectrometerTrackParticle;
  
  /// link to associated TrackParticle (Muon extrapolated)
  ElementLinkInt_p3 m_muonExtrapolatedTrackParticle;
  
  /// link to associated TrackParticle (Inner extrapolated)
  ElementLinkInt_p3 m_innerExtrapolatedTrackParticle;
  
  /// links to associated TrackParticle (Combined muon)
  ElementLinkInt_p3 m_combinedMuonTrackParticle;
  
  /// link to CaloCluster
  ElementLinkInt_p3 m_cluster;

  /// muon parameters
  std::vector<float> m_parameters;

  bool m_bestMatch;
  int m_matchNumberDoF;

  bool m_isAlsoFoundByLowPt;
  bool m_isAlsoFoundByCaloMuonId;

  /// energy loss in calorimeter
  ElementLinkInt_p3 m_caloEnergyLoss;

  unsigned short m_caloMuonAlsoFoundByMuonReco;

  bool m_isCorrected;

  uint16_t m_allAuthors;
  uint16_t m_isMuonBits;
  float    m_isMuonLikelihood;

  int m_outerMatchNumberDoF;

  /// links to associated TrackParticle (statistically combined muon)
  ElementLinkInt_p3 m_statCombinedMuonTrackParticle;

}; 

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

#endif //> RECTPCNV_MUON_P6_H
