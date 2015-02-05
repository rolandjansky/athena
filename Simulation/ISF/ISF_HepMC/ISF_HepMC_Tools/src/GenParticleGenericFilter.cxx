/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// GenParticleGenericFilter.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// class header include
#include "GenParticleGenericFilter.h"

// HepMC includes
#include "HepMC/GenParticle.h"

// stl includes
#include <limits>

/** Constructor **/
ISF::GenParticleGenericFilter::GenParticleGenericFilter( const std::string& t,
                                                         const std::string& n,
                                                         const IInterface* p )
  : AthAlgTool(t,n,p),
    m_minEta(-std::numeric_limits<double>::max()),
    m_maxEta(std::numeric_limits<double>::max()),
    m_minPhi(-M_PI),
    m_maxPhi( M_PI),
    m_minMom(-1.),
    m_maxMom(-1.),
    m_pdg()
{
    declareInterface<ISF::IGenParticleFilter>(this);

    // different cut parameters
    declareProperty("MinEta",
                    m_minEta,
                    "Minimum Particle Pseudorapidity" );
    declareProperty("MaxEta",
                    m_maxEta,
                    "Maximum Particle Pseudorapidity" );
    declareProperty("MinPhi",
                    m_minPhi,
                    "Minimum Particle Phi" );
    declareProperty("MaxPhi",
                    m_maxPhi,
                    "Maximum Particle Phi" );
    declareProperty("MinMom",
                    m_minMom,
                    "Minimum Particle Momentum"       );
    declareProperty("MaxMom",
                    m_maxMom,
                    "Maximum Particle Moemntum"       );
    declareProperty("ParticlePDG",
                    m_pdg,
                    "Particle PDG Code"               );
}


/** Athena algtool's Hooks */
StatusCode  ISF::GenParticleGenericFilter::initialize()
{
    ATH_MSG_VERBOSE("initialize() ...");
    ATH_MSG_VERBOSE("initialize() successful");
    return StatusCode::SUCCESS;
}


/** Athena algtool's Hooks */
StatusCode  ISF::GenParticleGenericFilter::finalize()
{
    ATH_MSG_VERBOSE("finalize() ...");
    ATH_MSG_VERBOSE("finalize() successful");
    return StatusCode::SUCCESS;
}


/** Returns the Particle Stack, should register truth */
bool ISF::GenParticleGenericFilter::pass(const HepMC::GenParticle& particle) const
{
  double mom = particle.momentum().rho();
  double eta = particle.momentum().eta();
  double phi = particle.momentum().phi();
  int    pdg = particle.pdg_id();

  bool pass = true;

  // check the particle pdg code
  if ( m_pdg.size()) {
    bool foundPDG = false;
    PDGCodes::const_iterator pdgIt  = m_pdg.begin();
    PDGCodes::const_iterator pdgEnd = m_pdg.end();
    for ( ; (!foundPDG) && (pdgIt!=pdgEnd); ++pdgIt)
      foundPDG = (pdg == (*pdgIt));
    pass = foundPDG;
  }

  // check the momentum cuts
  if (m_minMom>=0.) pass &= (mom >= m_minMom);
  if (m_maxMom>=0.) pass &= (mom <= m_maxMom);

  // check the eta cuts
  pass &= (eta >= m_minEta) && (eta <= m_maxEta);

  // check the phi cuts
  pass &= (phi >= m_minPhi) && (phi <= m_maxPhi);

  ATH_MSG_VERBOSE( "GenParticle '" << particle << "' with eta=" << eta
                   << " phi=" << phi << " did " << (pass ? "" : "not ")
                   << "pass the cuts.");

  return pass;
}

