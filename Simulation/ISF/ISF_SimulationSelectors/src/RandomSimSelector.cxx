/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// RandomSimSelector.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// class include
#include "RandomSimSelector.h"
// HepMC includes
#include "ISF_Event/ISFParticle.h"
// Units includes
#include "GaudiKernel/SystemOfUnits.h"

#include "TRandom.h"

/** Constructor **/
ISF::RandomSimSelector::RandomSimSelector(const std::string& t, const std::string& n, const IInterface* p)
  : BaseSimulationSelector(t,n,p)
  , m_random(0)
{
  m_random = new TRandom();
}

/** Destructor **/
ISF::RandomSimSelector::~RandomSimSelector()
{
  if (m_random!=NULL) delete m_random;
}

// Athena algtool's Hooks
StatusCode  ISF::RandomSimSelector::initialize()
{
  ATH_MSG_VERBOSE("Initializing ...");
  return StatusCode::SUCCESS;
}

StatusCode  ISF::RandomSimSelector::finalize()
{
  ATH_MSG_VERBOSE("Finalizing ...");
  return StatusCode::SUCCESS;
}

/** check whether given particle passes all cuts -> will be used for routing decision*/
bool  ISF::RandomSimSelector::passSelectorCuts(const ISFParticle& particle) const
{
  bool pass = (m_random->Uniform()<0.5);

  ATH_MSG_VERBOSE("[fatras randomselector] barcode = " << particle.barcode() << " pass = " << pass);

  return pass;
}
