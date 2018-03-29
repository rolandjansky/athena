/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// TruthCollectionMakerTau.cxx
// Create truth tau collection decorated with tau decay specific variables

// Class header file
#include "DerivationFrameworkMCTruth/TruthCollectionMakerTau.h"

// Tool header file
#include "tauRecTools/IBuildTruthTaus.h"

// Constructor
DerivationFramework::TruthCollectionMakerTau::TruthCollectionMakerTau(const std::string& t,
								      const std::string& n,
								      const IInterface* p )
  : AthAlgTool(t,n,p)
  , m_buildTruthTaus("tauRecTools::BuildTruthTaus/BuildTruthTaus")
{
  declareInterface<DerivationFramework::IAugmentationTool>(this);
  declareProperty("BuildTruthTaus", m_buildTruthTaus);
}

// Destructor
DerivationFramework::TruthCollectionMakerTau::~TruthCollectionMakerTau() {
}

// Athena initialize and finalize
StatusCode DerivationFramework::TruthCollectionMakerTau::initialize()
{
  ATH_MSG_VERBOSE("initialize() ...");
  ATH_CHECK( m_buildTruthTaus.retrieve() );
  return StatusCode::SUCCESS;
}

// Selection and collection creation
StatusCode DerivationFramework::TruthCollectionMakerTau::addBranches() const
{
  ATH_MSG_VERBOSE("addBranches() ...");
  // One call to build the truth tau collection
  ATH_CHECK( m_buildTruthTaus->retrieveTruthTaus() );
  // All done!
  return StatusCode::SUCCESS;
}

