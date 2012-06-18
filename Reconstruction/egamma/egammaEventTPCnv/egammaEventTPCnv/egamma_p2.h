///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// egammaContainer_p2.h 
// Header file for class egammaContainer_p2
/////////////////////////////////////////////////////////////////// 
#ifndef EGAMMAEVENTTPCNV_EGAMMA_P2_H 
#define EGAMMAEVENTTPCNV_EGAMMA_P2_H 1

// STL includes
#include <vector>

// DataModelAthenaPool includes
#include "DataModelAthenaPool/ElementLink_p1.h"
#include "DataModelAthenaPool/ElementLinkVector_p1.h"

// EventCommonTPCnv includes
#include "EventCommonTPCnv/P4EEtaPhiMFloat_p2.h"

// ParticleEventTPCnv includes
#include "ParticleEventTPCnv/ParticleBase_p1.h"

// forward declarations
class egammaCnv_p2;

class egamma_p2 
{
  /////////////////////////////////////////////////////////////////// 
  // Friend classes
  /////////////////////////////////////////////////////////////////// 

  // Make the AthenaPoolCnv class our friend
  friend class egammaCnv_p2;

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
public: 

  /** Default constructor: 
   */
  egamma_p2();

  /** Destructor: 
   */
  ~egamma_p2();

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
  P4EEtaPhiMFloat_p2 m_momentum;

  /// the ParticleBase part
  ParticleBase_p1 m_particleBase;

  // the egamma part 

  /// links to clusters
  ElementLinkInt_p1 m_cluster;
  
  /// links to tracks
  ElementLinkIntVector_p1 m_trackParticle;

  /// links to tracks
  ElementLinkIntVector_p1 m_conversion;

  /// links to egDetails
  ElementLinkIntVector_p1 m_egDetails;

  // authors
  unsigned int m_author;

  /// egamma PID
  std::vector<unsigned int> m_egammaEnumPIDs;
  std::vector<double> m_egammaDblPIDs;
}; 

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

inline egamma_p2::egamma_p2()
  : m_author(0)
{}

#endif //> EGAMMAEVENTTPCNV_EGAMMA_P2_H
