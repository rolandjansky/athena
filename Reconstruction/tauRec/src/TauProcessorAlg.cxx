/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/ListItem.h"

#include "tauRec/TauProcessorAlg.h"

//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
TauProcessorAlg::TauProcessorAlg(const std::string &name,
    ISvcLocator * pSvcLocator) :
AthAlgorithm(name, pSvcLocator),
m_tools(this) //make tools private
{
  declareProperty("Tools", m_tools);
}

//-----------------------------------------------------------------------------
// Destructor
//-----------------------------------------------------------------------------
TauProcessorAlg::~TauProcessorAlg() {
}

//-----------------------------------------------------------------------------
// Initializer
//-----------------------------------------------------------------------------
StatusCode TauProcessorAlg::initialize() {


    //ATH_MSG_INFO("FF::TauProcessorAlg :: initialize()");

    //-------------------------------------------------------------------------
    // No tools allocated!
    //-------------------------------------------------------------------------
    if (m_tools.size() == 0) {
        ATH_MSG_ERROR("no tools given!");
        return StatusCode::FAILURE;
    }

    StatusCode sc;
    
    //-------------------------------------------------------------------------
    // Allocate tools
    //-------------------------------------------------------------------------
    ToolHandleArray<ITauToolExecBase> ::iterator itT = m_tools.begin();
    ToolHandleArray<ITauToolExecBase> ::iterator itTE = m_tools.end();
    ATH_MSG_INFO("List of tools in execution sequence:");
    ATH_MSG_INFO("------------------------------------");

    unsigned int tool_count = 0;

    for (; itT != itTE; ++itT) {
        sc = itT->retrieve();
        if (sc.isFailure()) {
            ATH_MSG_WARNING("Cannot find tool named <" << *itT << ">");
	    return StatusCode::FAILURE;
        } else {
            ++tool_count;
            ATH_MSG_INFO((*itT)->type() << " - " << (*itT)->name());
        }
    }
    ATH_MSG_INFO(" ");
    ATH_MSG_INFO("------------------------------------");

    if (tool_count == 0) {
        ATH_MSG_ERROR("could not allocate any tool!");
        return StatusCode::FAILURE;
    }

    ///////////////////////////////////////////////////////////////////////////

    return StatusCode::SUCCESS;
}

//-----------------------------------------------------------------------------
// Finalizer
//-----------------------------------------------------------------------------
StatusCode TauProcessorAlg::finalize() {

  StatusCode sc;

  //-----------------------------------------------------------------
  // Loop stops when Failure indicated by one of the tools
  //-----------------------------------------------------------------
  ToolHandleArray<ITauToolExecBase> ::iterator itT = m_tools.begin();
  ToolHandleArray<ITauToolExecBase> ::iterator itTE = m_tools.end();
  for (; itT != itTE; ++itT) {
    ATH_MSG_VERBOSE("Invoking tool " << (*itT)->name());
    sc = (*itT)->finalize();
    if (sc.isFailure())
      break;
  }

  if (sc.isSuccess()) {
    ATH_MSG_VERBOSE("The tau candidate container has been modified");
  } else if (!sc.isSuccess()) {
  } else  {
  }


  return StatusCode::SUCCESS;

}

//-----------------------------------------------------------------------------
// Execution
//-----------------------------------------------------------------------------
StatusCode TauProcessorAlg::execute() {
  
  StatusCode sc;

  //-----------------------------------------------------------------
  // Loop stops when Failure indicated by one of the tools
  //-----------------------------------------------------------------
  ToolHandleArray<ITauToolExecBase> ::iterator itT = m_tools.begin();
  ToolHandleArray<ITauToolExecBase> ::iterator itTE = m_tools.end();
  for (; itT != itTE; ++itT) {
    ATH_MSG_VERBOSE("Invoking tool " << (*itT)->name());
    sc = (*itT)->execute();
    if (sc.isFailure())
      break;
  }

  if (sc.isSuccess()) {
    ATH_MSG_VERBOSE("The tau candidate container has been modified");
  } else if (!sc.isSuccess()) {
  } else  {
  }


  return StatusCode::SUCCESS;
}
