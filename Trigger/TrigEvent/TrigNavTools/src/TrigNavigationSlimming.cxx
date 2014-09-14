/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/MsgStream.h"

#include "TrigNavigation/Navigation.h"
#include "TrigNavigation/TriggerElement.h"

#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"

#include "TrigNavigationSlimming.h"

using namespace HLT;

HLT::TrigNavigationSlimming::TrigNavigationSlimming(const std::string& name, ISvcLocator *pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  m_slimmingTool("HLT::TrigNavigationSlimmingTool/TrigNavigationSlimmingTool", this) {

  declareProperty("SlimmingTool", m_slimmingTool, "Tool instance which does the actual slimming");
}

StatusCode HLT::TrigNavigationSlimming::initialize() {

  
  ATH_MSG_DEBUG ( "TrigNavigationSlimming::Intialize()" );


  if (  m_slimmingTool.retrieve().isFailure()) {
    ATH_MSG_FATAL ( "Unable to retrieve the TrigNavigationSlimmingTool!" );
    ATH_MSG_FATAL ( "Please check your job options files." );
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO ( "Successfully retrived the TrigNavigationSlimmingTool!" );
  return StatusCode::SUCCESS;
}


StatusCode HLT::TrigNavigationSlimming::execute() {
  ATH_MSG_DEBUG("Executing slimming tool " << m_slimmingTool->name());

  

  ATH_CHECK( m_slimmingTool->doSlimming() );
  return StatusCode::SUCCESS;
}

StatusCode HLT::TrigNavigationSlimming::finalize() {
  return StatusCode::SUCCESS;
}
