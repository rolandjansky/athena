/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// DefaultSimSelector.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// class include
#include "DefaultSimSelector.h"
// HepMC includes
#include "ISF_Event/ISFParticle.h"
// Units includes
#include "GaudiKernel/SystemOfUnits.h"

/** Constructor **/
ISF::DefaultSimSelector::DefaultSimSelector(const std::string& t, const std::string& n, const IInterface* p)
  : ISimulationSelector(t,n,p)
{
    declareInterface<ISF::ISimulationSelector>(this);
}

/** Destructor **/
ISF::DefaultSimSelector::~DefaultSimSelector()
{
}

// Athena algtool's Hooks
StatusCode  ISF::DefaultSimSelector::initialize()
{
    ATH_MSG_VERBOSE("Initializing ...");
    return StatusCode::SUCCESS;
}

StatusCode  ISF::DefaultSimSelector::finalize()
{
    ATH_MSG_VERBOSE("Finalizing ...");
    return StatusCode::SUCCESS;
}

/** check whether given particle passes all cuts -> will be used for routing decision*/
bool  ISF::DefaultSimSelector::passSelectorCuts(const ISFParticle& /*particle*/) const
{
  return true;
}

