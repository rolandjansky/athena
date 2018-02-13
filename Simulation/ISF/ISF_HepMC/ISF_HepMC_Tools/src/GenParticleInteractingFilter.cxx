/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// GenParticleInteractingFilter.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// class header include
#include "GenParticleInteractingFilter.h"

// HepMC includes
#include "HepMC/GenParticle.h"

// Helper function
#include "TruthUtils/HepMCHelpers.h"

/** Constructor **/
ISF::GenParticleInteractingFilter::GenParticleInteractingFilter( const std::string& t,
                                                                 const std::string& n,
                                                                 const IInterface* p )
  : base_class(t,n,p)
{
}

/** passes through to the private version of the filter */
bool ISF::GenParticleInteractingFilter::pass(const HepMC::GenParticle& particle) const
{
  return !MC::isNonInteracting( &particle );
}

