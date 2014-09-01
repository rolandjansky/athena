/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ValidationTruthStrategy.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// class header include
#include "ISF_HepMC_Tools/ValidationTruthStrategy.h"

// ISF includes
#include "ISF_Event/ITruthIncident.h"
#include "ISF_Event/ISFParticle.h"

/** Constructor **/
ISF::ValidationTruthStrategy::ValidationTruthStrategy(const std::string& t, const std::string& n, const IInterface* p) : 
  AthAlgTool(t,n,p),
  m_minPrimaryP2(0.)
{
    declareInterface<ISF::ITruthStrategy>(this);

    // primary particle minimum momentum
    declareProperty("PrimaryMinP"        , m_minPrimaryP2     );
}

/** Destructor **/
ISF::ValidationTruthStrategy::~ValidationTruthStrategy()
{
}

// Athena algtool's Hooks
StatusCode  ISF::ValidationTruthStrategy::initialize()
{
    ATH_MSG_VERBOSE("Initializing ...");

    // (*) setup primary particle cuts
    // -> compute p^2 for fast comparison
    m_minPrimaryP2 *= m_minPrimaryP2;

    return StatusCode::SUCCESS;
}

StatusCode  ISF::ValidationTruthStrategy::finalize()
{
    ATH_MSG_VERBOSE("Finalizing ...");
    return StatusCode::SUCCESS;
}

bool ISF::ValidationTruthStrategy::pass( const ITruthIncident& ti) const {

  // primary particle check
  bool pass =  ( ti.primaryP2() >= m_minPrimaryP2 );

  return pass;
}

