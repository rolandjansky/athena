///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// Muon_p4.h 
// Header file for class Muon_p4
// Author: Ketevi A. Assamagan <ketevi@bnl.gov>
// Date:  June 2008 

/////////////////////////////////////////////////////////////////// 
#ifndef RECTPCNV_MUON_P4_H 
#define RECTPCNV_MUON_P4_H 

// STL includes
#include <inttypes.h>
#include <vector>

// DataModelAthenaPool includes
#include "DataModelAthenaPool/ElementLink_p1.h"
#include "DataModelAthenaPool/ElementLinkVector_p1.h"

// EventCommonTPCnv includes
#include "EventCommonTPCnv/P4IPtCotThPhiM_p1.h"

// ParticleEventTPCnv includes
#include "ParticleEventTPCnv/ParticleBase_p1.h"

// forward declarations
class MuonCnv_p4;

class Muon_p4 
{
  /////////////////////////////////////////////////////////////////// 
  // Friend classes
  /////////////////////////////////////////////////////////////////// 

  // Make the AthenaPoolCnv class our friend
  friend class MuonCnv_p4;

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
public: 

  /** Default constructor: 
   */
  Muon_p4()
    : m_author(0),
      m_hasCombinedMuon(0),
      m_hasInDetTrackParticle(0),
      m_hasMuonExtrapolatedTrackParticle(0),
      m_hasInnerExtrapolatedTrackParticle(0),
      m_hasCombinedMuonTrackParticle(0),
      m_hasCluster(0),
      m_matchChi2(0),
      m_bestMatch(0),
      m_matchNumberDoF(0),
      m_isAlsoFoundByLowPt(0),
      m_isAlsoFoundByCaloMuonId(0),
      m_caloMuonAlsoFoundByMuonReco(0),
      m_isCorrected(false),
      m_allAuthors(0)
  {}

  /** Destructor: 
   */
  ~Muon_p4() = default;

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
  ParticleBase_p1 m_particleBase;

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
  double m_matchChi2;

  /// Low Pt muon stuff
  std::vector<int> m_associatedEtaDigits;
  std::vector<int> m_associatedPhiDigits;
  //<--

  /// link to associated TrackParticle (Inner Detector)
  ElementLinkInt_p1 m_inDetTrackParticle;
 
  /// link to the Trk::Segment
  ElementLinkIntVector_p1 m_muonSegments;
 
  /// link to associated TrackParticle (Muon Spectrometer)
  ElementLinkInt_p1 m_muonSpectrometerTrackParticle;
  
  /// link to associated TrackParticle (Muon extrapolated)
  ElementLinkInt_p1 m_muonExtrapolatedTrackParticle;
  
  /// link to associated TrackParticle (Inner extrapolated)
  ElementLinkInt_p1 m_innerExtrapolatedTrackParticle;
  
  /// links to associated TrackParticle (Combined muon)
  ElementLinkInt_p1 m_combinedMuonTrackParticle;
  
  /// link to CaloCluster
  ElementLinkInt_p1 m_cluster;

  /// muon parameters
  std::vector<float> m_parameters;

  bool m_bestMatch;
  int m_matchNumberDoF;

  bool m_isAlsoFoundByLowPt;
  bool m_isAlsoFoundByCaloMuonId;

  /// energy loss in calorimeter
  ElementLinkInt_p1 m_caloEnergyLoss;

  unsigned short m_caloMuonAlsoFoundByMuonReco;

  bool m_isCorrected;

  uint16_t m_allAuthors;

}; 

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

#endif //> RECTPCNV_MUON_P2_H
