/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////// 
// TruthEtIsolationsContainer.cxx 
// Implementation file for class TruthEtIsolationsContainer
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 


// STL includes
#include <iostream>

// McParticleEvent includes
#include "McParticleEvent/TruthEtIsolationsContainer.h"

/////////////////////////////////////////////////////////////////// 
/// Public methods: 
/////////////////////////////////////////////////////////////////// 

/// Constructors
////////////////

TruthEtIsolationsContainer::TruthEtIsolationsContainer() :
  DataVector<TruthEtIsolations>( SG::OWN_ELEMENTS )
{}

TruthEtIsolationsContainer::TruthEtIsolationsContainer( const TruthEtIsolationsContainer& rhs ) :
  DataVector<TruthEtIsolations>( rhs )
{}

TruthEtIsolationsContainer& 
TruthEtIsolationsContainer::operator=( const TruthEtIsolationsContainer& rhs ) 
{
  if ( this != &rhs ) {
    DataVector<TruthEtIsolations>::operator=(rhs);
  }

  return *this;
}

// Destructor
///////////////

TruthEtIsolationsContainer::~TruthEtIsolationsContainer()
{}
