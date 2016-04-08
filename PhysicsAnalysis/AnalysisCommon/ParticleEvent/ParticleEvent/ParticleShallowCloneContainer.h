///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ParticleShallowCloneContainer.h 
// Header file for class ParticleShallowCloneContainer
// Author: Karsten Koeneke - February 2013
/////////////////////////////////////////////////////////////////// 
#ifndef PARTICLEEVENT_PARTICLESHALLOWCLONECONTAINER_H 
#define PARTICLEEVENT_PARTICLESHALLOWCLONECONTAINER_H 1

// STL includes
#include <iosfwd>

#include "AthContainers/DataVector.h"

// SGTools includes (for persistency)
#include "SGTools/CLASS_DEF.h"

// NavFourMom includes
#include "EventKernel/IParticle.h"
#include "NavFourMom/IParticleContainer.h"

// ParticleEvent includes
#include "ParticleEvent/ParticleShallowClone.h"



// Needed for constructing the mirror container hierarchy
// This tells StoreGate that DV<CP> derives from DV<IP>
DATAVECTOR_BASE( Analysis::ParticleShallowClone, IParticle );

class ParticleShallowCloneContainer : public DataVector<Analysis::ParticleShallowClone>
{
 //  /////////////////////////////////////////////////////////////////// 
 //  // Public methods: 
 //  /////////////////////////////////////////////////////////////////// 
 // public: 

 //  /** Default constructor: 
 //   */
 //  ParticleShallowCloneContainer();

 //  /** Constructor with parameters: to setup the ownership policy
 //   */
 //  ParticleShallowCloneContainer( const SG::OwnershipPolicy own );

 //  /** Destructor: 
 //   */
 //  virtual ~ParticleShallowCloneContainer(); 

}; 

/////////////////////////////////////////////////////////////////// 
/// Inline methods: 
/////////////////////////////////////////////////////////////////// 

CLASS_DEF( ParticleShallowCloneContainer , 1256037999 , 1 )

// Tell StoreGate that ParticleShallowCloneContainer derives from DV<CP>
SG_BASE(ParticleShallowCloneContainer, DataVector<Analysis::ParticleShallowClone>);

#endif //> PARTICLEEVENT_PARTICLESHALLOWCLONECONTAINER_H
