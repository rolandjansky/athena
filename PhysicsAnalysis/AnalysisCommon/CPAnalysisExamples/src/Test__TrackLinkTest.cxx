/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// CPAnalysisExamples includes
#include "Test__TrackLinkTest.h"

#include "xAODEgamma/ElectronContainer.h"
#include "xAODTracking/TrackParticleContainer.h"

namespace Test {

TrackLinkTest::TrackLinkTest( const std::string& name, ISvcLocator* pSvcLocator ) : AthAnalysisAlgorithm( name, pSvcLocator ){}


TrackLinkTest::~TrackLinkTest() {}


StatusCode TrackLinkTest::initialize() {
  ATH_MSG_INFO ("Initializing " << name() << "...");

  return StatusCode::SUCCESS;
}

StatusCode TrackLinkTest::finalize() {
  ATH_MSG_INFO ("Finalizing " << name() << "...");

  return StatusCode::SUCCESS;
}

StatusCode TrackLinkTest::execute() {  
  ATH_MSG_DEBUG ("Executing " << name() << "...");
  setFilterPassed(false); //optional: start with algorithm not passed

  const xAOD::ElectronContainer *inCont(0);
  ATH_CHECK( evtStore()->retrieve( inCont, "Electrons" ) );

  for ( const xAOD::Electron* electron : *inCont){
    // Get the TrackParticle
	const xAOD::TrackParticle* trackPart(0);
	trackPart = electron->trackParticle();
	if ( !trackPart ) {
      ATH_MSG_ERROR("Couldn't get the TrackParticle for this Electron");
      return StatusCode::FAILURE;
    }
  }

  setFilterPassed(true); //if got here, assume that means algorithm passed
  return StatusCode::SUCCESS;
}

StatusCode TrackLinkTest::beginInputFile() { 

  return StatusCode::SUCCESS;
}

} //> end namespace Test
