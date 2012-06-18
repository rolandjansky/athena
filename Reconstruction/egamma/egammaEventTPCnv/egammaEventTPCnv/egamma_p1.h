///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// egammaContainer_p1.h 
// Header file for class egammaContainer_p1
// Author: K.Cranmer<cranmer@cern.ch>
// Author: S.Binet<binet@cern.ch>
// Date:   December 2006
/////////////////////////////////////////////////////////////////// 
#ifndef EGAMMAEVENTTPCNV_EGAMMA_P1_H 
#define EGAMMAEVENTTPCNV_EGAMMA_P1_H 

// STL includes
#include <vector>

// DataModelAthenaPool includes
#include "DataModelAthenaPool/ElementLink_p1.h"
#include "DataModelAthenaPool/ElementLinkVector_p1.h"

// EventCommonTPCnv includes
#include "EventCommonTPCnv/P4EEtaPhiM_p1.h"

// ParticleEventTPCnv includes
#include "ParticleEventTPCnv/ParticleBase_p1.h"

// forward declarations
class egammaCnv_p1;

class egamma_p1 
{
  /////////////////////////////////////////////////////////////////// 
  // Friend classes
  /////////////////////////////////////////////////////////////////// 

  // Make the AthenaPoolCnv class our friend
  friend class egammaCnv_p1;

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
public: 

  /** Default constructor: 
   */
  egamma_p1();

  /** Destructor: 
   */
  ~egamma_p1();

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
  P4EEtaPhiM_p1 m_momentum;

  /// the ParticleBase part
  ParticleBase_p1 m_particleBase;

  // the egamma part 

  /// links to clusters
  ElementLinkInt_p1 m_cluster;
  
  /// links to tracks
  ElementLinkInt_p1 m_trackParticle;

  /// links to tracks
  ElementLinkInt_p1 m_conversion;

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

inline egamma_p1::egamma_p1()
  : m_author(0)
{}

#endif //> EGAMMAEVENTTPCNV_EGAMMA_P1_H
