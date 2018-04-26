/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// GenParticleFinalStateFilter.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// class header include
#include "GenParticleFinalStateFilter.h"

// TruthUtils include
#include "TruthUtils/HepMCHelpers.h"

// HepMC includes
#include "HepMC/GenParticle.h"


/** Constructor **/
ISF::GenParticleFinalStateFilter::GenParticleFinalStateFilter( const std::string& t,
                                                               const std::string& n,
                                                               const IInterface* p )
  : base_class(t,n,p),
    m_checkGenSimStable(true),
    m_checkGenInteracting(true)
{
    // different options
    declareProperty("CheckGenSimStable",        m_checkGenSimStable);
    declareProperty("CheckGenInteracting",      m_checkGenInteracting);
}


// Athena algtool's Hooks
StatusCode  ISF::GenParticleFinalStateFilter::initialize()
{
    ATH_MSG_VERBOSE("Initializing ...");
    return StatusCode::SUCCESS;
}


/** returns true if the the particle is considered stable */
bool ISF::GenParticleFinalStateFilter::pass(const HepMC::GenParticle& particle) const
{
  bool passFilter = true;
  passFilter &= isFinalState(particle);
  passFilter &= (!m_checkGenSimStable)   || MC::isSimStable(&particle);
  passFilter &= (!m_checkGenInteracting) || MC::isSimInteracting(&particle);

  return passFilter;
}


StatusCode  ISF::GenParticleFinalStateFilter::finalize()
{
    ATH_MSG_VERBOSE("Finalizing ...");
    return StatusCode::SUCCESS;
}

/** checks if the particle is in its final state (no end vertex) */
bool ISF::GenParticleFinalStateFilter::isFinalState(const HepMC::GenParticle &p) const {
  // particle is in its final state if both:
  //  * no end_vertex
  //  * status==1
  return ( !p.end_vertex() && p.status()==1 );
}
