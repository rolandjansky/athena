///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// CompositeContainer.h 
// Header file for class CompositeContainer
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef PARTICLEEVENT_COMPOSITEPARTICLECONTAINER_H 
#define PARTICLEEVENT_COMPOSITEPARTICLECONTAINER_H 1

// STL includes
#include <iosfwd>

#include "AthContainers/DataVector.h"

// SGTools includes (for persistency)
#include "SGTools/CLASS_DEF.h"

// NavFourMom includes
#include "NavFourMom/INavigable4MomentumCollection.h"
#include "EventKernel/IParticle.h"
#include "NavFourMom/IParticleContainer.h"

// ParticleEvent includes
#include "ParticleEvent/CompositeParticle.h"

// Forward declaration

// Needed for constructing the mirror container hierarchy
// This tells StoreGate that DV<CP> derives from DV<IP>
DATAVECTOR_BASE( CompositeParticle, IParticle );

class CompositeParticleContainer : public DataVector<CompositeParticle>
{ 
  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** Default constructor: 
   */
  CompositeParticleContainer();

  /** Copy constructor: compiler generated one is good enough
   */
  CompositeParticleContainer( const CompositeParticleContainer& rhs );

  /** Assignment operator: compiler generated one is good enough
   */
  CompositeParticleContainer& 
  operator=( const CompositeParticleContainer& rhs ); 

  /** Constructor with parameters: to setup the ownership policy
   */
  CompositeParticleContainer( const SG::OwnershipPolicy own );

  /** Destructor: 
   */
  virtual ~CompositeParticleContainer(); 

}; 

/////////////////////////////////////////////////////////////////// 
/// Inline methods: 
/////////////////////////////////////////////////////////////////// 

CLASS_DEF( CompositeParticleContainer, 1070450393, 1)

// Tell StoreGate that CompositeParticleContainer derives from DV<CP>
SG_BASE(CompositeParticleContainer, DataVector<CompositeParticle>);

#endif //> PARTICLEEVENT_COMPOSITEPARTICLECONTAINER_H
