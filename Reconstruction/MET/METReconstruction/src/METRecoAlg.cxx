/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// METRecoAlg.cxx

#include "METRecoAlg.h"
#include "METRecoInterface/IMETRecoTool.h"

using std::string;

namespace met {

  //**********************************************************************

  METRecoAlg::METRecoAlg(const std::string& name,
			 ISvcLocator* pSvcLocator )
    : ::AthAlgorithm( name, pSvcLocator ),
      m_recotools (this)
  {
    declareProperty( "RecoTools", m_recotools);
  }

  //**********************************************************************

  METRecoAlg::~METRecoAlg() { }

  //**********************************************************************

  StatusCode METRecoAlg::initialize() {
    ATH_MSG_VERBOSE("Initializing " << name() << "...");

    ATH_CHECK( m_recotools.retrieve() );
  
    return StatusCode::SUCCESS;
  }

  //**********************************************************************

  StatusCode METRecoAlg::finalize() {
    ATH_MSG_VERBOSE ("Finalizing " << name() << "...");
    return StatusCode::SUCCESS;
  }

  //**********************************************************************

  StatusCode METRecoAlg::execute() { 
    ATH_MSG_VERBOSE("Executing " << name() << "...");
    // Loop over tools.

    // Run the top-level MET tools in sequence
    for(ToolHandleArray<IMETRecoTool>::const_iterator iTool=m_recotools.begin();
	iTool != m_recotools.end(); ++iTool) {
      ToolHandle<IMETRecoTool> tool = *iTool;
      ATH_MSG_VERBOSE("Running tool: " << tool->name() );
      if( tool->execute().isFailure() ) {
	ATH_MSG_ERROR("Failed to execute tool: " << tool->name());
	return StatusCode::FAILURE;
      }
    }

    return StatusCode::SUCCESS;
  }

  //**********************************************************************

}

