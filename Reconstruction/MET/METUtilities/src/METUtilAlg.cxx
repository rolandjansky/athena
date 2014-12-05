/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// METUtilAlg.cxx

#include "METUtilAlg.h"
#include "METInterface/IMETRebuilder.h"

using std::string;

namespace met {

  //**********************************************************************

  METUtilAlg::METUtilAlg(const std::string& name,
			 ISvcLocator* pSvcLocator )
  : ::AthAlgorithm( name, pSvcLocator ) {
    declareProperty( "Rebuilders", m_rebuilders);
  }

  //**********************************************************************

  METUtilAlg::~METUtilAlg() { }

  //**********************************************************************

  StatusCode METUtilAlg::initialize() {
    ATH_MSG_INFO("Initializing " << name() << "...");
    ATH_MSG_INFO("Retrieving tools...");

    // retrieve tools
    for(ToolHandleArray<IMETRebuilder>::const_iterator iTool=m_rebuilders.begin();
	iTool != m_rebuilders.end(); ++iTool) {
      ToolHandle<IMETRebuilder> tool = *iTool;
      if( tool.retrieve().isFailure() ) {
	ATH_MSG_ERROR("Failed to retrieve tool: " << tool->name());
	return StatusCode::FAILURE;
      };
      ATH_MSG_INFO("Retrieved tool: " << tool->name() );
    }
  
    return StatusCode::SUCCESS;
  }

  //**********************************************************************

  StatusCode METUtilAlg::finalize() {
    ATH_MSG_INFO ("Finalizing " << name() << "...");
    return StatusCode::SUCCESS;
  }

  //**********************************************************************

  StatusCode METUtilAlg::execute() { 
    ATH_MSG_VERBOSE("Executing " << name() << "...");
    // Loop over tools.

    // Run the top-level MET tools in sequence
    for(ToolHandleArray<IMETRebuilder>::const_iterator iTool=m_rebuilders.begin();
	iTool != m_rebuilders.end(); ++iTool) {
      ToolHandle<IMETRebuilder> tool = *iTool;
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

