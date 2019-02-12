///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ParticleJet_p1.h 
// Header file for class ParticleJet_p1
// Author: K.Cranmer<cranmer@cern.ch>
// Author: S.Binet<binet@cern.ch>
// Date:   December 2006
/////////////////////////////////////////////////////////////////// 
#ifndef PARTICLEEVENTTPCNV_PARTICLEJET_P1_H 
#define PARTICLEEVENTTPCNV_PARTICLEJET_P1_H 

// STL includes
#include <vector>

// EventCommonTPCnv includes
#include "EventCommonTPCnv/P4PxPyPzE_p1.h"

// ParticleEventTPCnv includes
#include "ParticleEventTPCnv/ParticleBase_p1.h"

// forward declarations
class JetTagInfoBase;
namespace Analysis {
  class ITagInfo;
  class IConstituent;
}

class ParticleJetCnv_p1;

class ParticleJet_p1 
{
  /////////////////////////////////////////////////////////////////// 
  // Friend classes
  /////////////////////////////////////////////////////////////////// 

  // Make the AthenaPoolCnv class our friend
  friend class ParticleJetCnv_p1;

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
public: 

  /** Default constructor: 
   */
  ParticleJet_p1();

  /** Destructor: 
   */
  ~ParticleJet_p1() = default;

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
  P4PxPyPzE_p1 m_momentum;

  /// the ParticleBase part
  ParticleBase_p1 m_particleBase;

  // the ParticleJet part 

  /// combined likelihoods
  std::vector<double> m_combinedLikelihood;
  
  /// authors
  std::string m_author; // TODO: optimize with an int-to-string id

  /// ITagInfo objects
  std::vector<Analysis::ITagInfo*> m_tagJetInfo;

  /// IConstituents objects
  std::vector<Analysis::IConstituent*> m_constituents;
}; 

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

inline ParticleJet_p1::ParticleJet_p1()
{}

#endif //> PARTICLEEVENTTPCNV_PARTICLEJET_P1_H
