/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//#ifdef XAOD_STANDALONE
#include "TauDiscriminant/TauDiscriminantProcessor.h"
#include "xAODTau/TauDefs.h"
#include <stdlib.h>

TauDiscriminantProcessor::TauDiscriminantProcessor( const std::string& name )
  : asg::AsgTool(name),
    m_tools()
{
  declareProperty("tools", m_tools);
}

TauDiscriminantProcessor::~TauDiscriminantProcessor()
{

}

StatusCode TauDiscriminantProcessor::initialize()
{

  ATH_MSG_INFO("TauDiscriminantProcessor initializing...");
  
  if( m_tools.size() == 0 ){
    ATH_MSG_ERROR("No TauDiscriminant tools defined");
    return StatusCode::FAILURE;
  }

  ATH_MSG_INFO("------------------------------------");
  ATH_MSG_INFO("List of tools in execution sequence:");
  unsigned int tool_count(0);
  
  for(auto tool : m_tools){
    if( tool.retrieve().isFailure() ){
      ATH_MSG_WARNING("Cannot find tool named <" << tool->name() << ">");
    }else{
      ++tool_count;
      ATH_MSG_INFO(tool->name());
      if( tool->initialize().isFailure() ){
	ATH_MSG_FATAL("Initialization failed in tool " << tool->name());
	return StatusCode::FAILURE;
      }
    }
  }
  ATH_MSG_INFO("------------------------------------");
  
  if(tool_count == 0){
    ATH_MSG_ERROR("Did not allocate any tool!");
    return StatusCode::FAILURE;
  }
	
  return StatusCode::SUCCESS;
}


StatusCode TauDiscriminantProcessor::eventInitialize(){
  for(auto tool : m_tools){
    // if( tool.retrieve().isFailure() ){
    //   ATH_MSG_WARNING("Cannot find tool named <" << tool->name() << ">");
    // }else{
      if( tool->eventInitialize().isFailure() ){
	ATH_MSG_FATAL("Event Initialization failed in tool " << tool->name());
	return StatusCode::FAILURE;
      }
      //}
  }
  return StatusCode::SUCCESS;
}


CP::CorrectionCode TauDiscriminantProcessor::applyDiscriminant(xAOD::TauJet& tau)
{
  for(auto tool : m_tools){
    if( tool->execute(tau).isFailure() ){
      ATH_MSG_FATAL("Execute failed in tool " << tool->name());
      return CP::CorrectionCode::Error;
    }
  }
  return CP::CorrectionCode::Ok;
}

//#endif //XAOD_STANDALONE
