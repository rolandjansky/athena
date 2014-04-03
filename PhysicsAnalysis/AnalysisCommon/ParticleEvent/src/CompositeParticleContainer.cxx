/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////// 
// CompositeParticleContainer.cxx 
// Implementation file for class CompositeParticleContainer
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 


// STL includes
#include <iostream>

// ParticleEvent includes
#include "ParticleEvent/CompositeParticleContainer.h"

/////////////////////////////////////////////////////////////////// 
/// Public methods: 
/////////////////////////////////////////////////////////////////// 

/// Constructors
////////////////

CompositeParticleContainer::CompositeParticleContainer() :
  DataVector<CompositeParticle>( )
{}


CompositeParticleContainer::CompositeParticleContainer( const CompositeParticleContainer& rhs ) :
  //DataVector<I4Momentum>   ( rhs ),
  DataVector<INavigable4Momentum>( rhs ),
  //DataVector<IParticle>( rhs ),
  DataVector<CompositeParticle>( rhs )
{}

CompositeParticleContainer& 
CompositeParticleContainer::operator=( const CompositeParticleContainer& rhs ) 
{
  if ( this != &rhs ) {
    DataVector<CompositeParticle>::operator=(rhs);
  }

  return *this;
}


CompositeParticleContainer::CompositeParticleContainer( const SG::OwnershipPolicy own ) :
  DataVector<CompositeParticle>(own)
{}

// Destructor
///////////////
CompositeParticleContainer::~CompositeParticleContainer()
{}

/////////////////////////////////////////////////////////////////// 
/// Const methods: 
///////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////// 
/// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
/// Protected methods: 
/////////////////////////////////////////////////////////////////// 

