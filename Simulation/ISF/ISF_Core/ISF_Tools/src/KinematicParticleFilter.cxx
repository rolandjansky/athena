/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// KinematicParticleFilter.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// class header include
#include "KinematicParticleFilter.h"

// ISF includes
#include "ISF_Event/ISFParticle.h"

/** Constructor **/
ISF::KinematicParticleFilter::KinematicParticleFilter(const std::string& t, const std::string& n, const IInterface* p) :
  base_class(t,n,p),
  KinematicParticleCuts(),
  m_maxEtaSym(3.0f)
{
  declareProperty( "MaxEtaSymmetric",
                   m_maxEtaSym=3.0,
                   "Particle momentum pseudorapidity cut");
  declareProperty( "MinMomentum",
                   m_cut_minMom2=100.,
                   "Momentum cut");
}

/** Destructor **/
ISF::KinematicParticleFilter::~KinematicParticleFilter()
{
}

/** Athena algtool's Hooks */
StatusCode  ISF::KinematicParticleFilter::initialize()
{
  ATH_MSG_INFO("initialize() ...");

  // internally storing the p^2
  m_cut_minMom2  *= m_cut_minMom2;
  // eta cuts
  m_cut_minMomEta = -m_maxEtaSym;
  m_cut_maxMomEta =  m_maxEtaSym;

  ATH_MSG_INFO("initialize() successful");
  return StatusCode::SUCCESS;
}


/** Athena algtool's Hooks */
StatusCode  ISF::KinematicParticleFilter::finalize()
{
  ATH_MSG_INFO("finalize() ...");

  ATH_MSG_INFO("finalize() successful");
  return StatusCode::SUCCESS;
}


/** does the particle pass the this filter? */
bool ISF::KinematicParticleFilter::passFilter(const ISFParticle &p) const {
  return ISF::KinematicParticleCuts::pass(p);
}
