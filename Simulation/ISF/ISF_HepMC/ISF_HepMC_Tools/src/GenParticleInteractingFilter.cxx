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
  declareProperty("AdditionalInteractingParticleTypes", m_additionalInteractingParticleTypes);
}

StatusCode ISF::GenParticleInteractingFilter::initialize()
{
  if(m_additionalInteractingParticleTypes.empty()) {
    ATH_MSG_DEBUG("No additional particle types will be classified as interacting.");
  }
  else {
    ATH_MSG_DEBUG("Will classify particles with the following additional PDG codes as interacting:");
    for(const auto& pdg_id : m_additionalInteractingParticleTypes) {
      ATH_MSG_DEBUG("   " << pdg_id);
    }
  }
  return StatusCode::SUCCESS;
}

/** passes through to the private version of the filter */
bool ISF::GenParticleInteractingFilter::pass(const HepMC::GenParticle& particle) const
{
  const int& pdg_id = particle.pdg_id();
  const bool isInteracting = find(m_additionalInteractingParticleTypes.begin(),
                                m_additionalInteractingParticleTypes.end(),
                                pdg_id) != m_additionalInteractingParticleTypes.end();
  return !MC::isNonInteracting( &particle ) || isInteracting;
}

