/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// EntryLayerFilter.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// class header include
#include "EntryLayerFilter.h"

// ISF includes
#include "ISF_Event/ISFParticle.h"

// ISF interfaces
#include "ISF_Interfaces/IParticleFilter.h"

/** Constructor **/
ISF::EntryLayerFilter::EntryLayerFilter(const std::string& t, const std::string& n, const IInterface* p) :
  base_class(t,n,p),
  m_ekinCharged(-1),
  m_ekinNeutral(-1)
{
  declareProperty( "MinEkinCharged",
                   m_ekinCharged=-1.,
                   "Minimum kinetic energy cut for charged particles (set -1 to filter out all)");
  declareProperty( "MinEkinNeutral",
                   m_ekinNeutral=-1.,
                   "Minimum kinetic energy cut for neutral particles (set -1 to filter out all)");
}

/** Destructor **/
ISF::EntryLayerFilter::~EntryLayerFilter()
{
}

/** Athena algtool's Hooks */
StatusCode  ISF::EntryLayerFilter::initialize()
{
  ATH_MSG_INFO("initialize() ...");

  ATH_MSG_INFO("initialize() successful");
  return StatusCode::SUCCESS;
}


/** Athena algtool's Hooks */
StatusCode  ISF::EntryLayerFilter::finalize()
{
  ATH_MSG_INFO("finalize() ...");

  ATH_MSG_INFO("finalize() successful");
  return StatusCode::SUCCESS;
}


/** does the particle pass the this filter? */
bool ISF::EntryLayerFilter::passFilter(const ISFParticle &p) const {
  double        ekin = p.ekin();
  bool     isCharged = fabs(p.charge()) > 1e-5;

  double  curEkinCut = isCharged ? m_ekinCharged : m_ekinNeutral;

  // pass if cut is activated and energy is above threshold
  bool pass = ( curEkinCut > 0. ) && ( ekin > curEkinCut );

  return pass;
}
