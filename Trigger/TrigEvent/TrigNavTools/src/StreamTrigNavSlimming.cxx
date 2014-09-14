/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigNavTools/TrigNavigationSlimmingHdlr.h"
#include "TrigNavTools/TrigNavigationSlimmingTool.h"
#include "TrigSteeringEvent/HLTResult.h"

#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"

#include "StreamTrigNavSlimming.h"

HLT::StreamTrigNavSlimming::StreamTrigNavSlimming(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  m_thinningSvc("IThinningSvc/ThinningSvc", name),
  m_slimmingTool("HLT::TrigNavigationSlimmingTool/TrigNavigationSlimmingTool", this)
{
  declareProperty("ThinningSvc", m_thinningSvc, "Synchronize feature indexes with this instance of ThinningSvc");
  declareProperty("SlimmingTool", m_slimmingTool, "Tool instance which does the actual slimming");
  declareProperty("ResultKey", m_resultKey = "HLTResult_HLT");
}

HLT::StreamTrigNavSlimming::~StreamTrigNavSlimming() {
}

StatusCode HLT::StreamTrigNavSlimming::initialize() {  
  ATH_MSG_DEBUG( "StreamTrigNavSlimming::initialize()" );
  CHECK( m_thinningSvc.retrieve() ); 
  CHECK( m_slimmingTool.retrieve() );
  return  StatusCode::SUCCESS;
}

StatusCode HLT::StreamTrigNavSlimming::finalize() {
  return StatusCode::SUCCESS;
}

StatusCode HLT::StreamTrigNavSlimming::execute() {
  ATH_MSG_DEBUG( "StreamTrigNavSlimming::execute()" );

  // check if the result is there (do nothing if is not)
  if ( not evtStore()->contains<HLTResult>( m_resultKey ) ) {
    ATH_MSG_DEBUG( "Absent HLTResult of key " << m_resultKey << " dooing nothing");
    return StatusCode::SUCCESS;
  }

  // load the HLT Result
  const HLT::HLTResult *constResult = nullptr;  
  if ( evtStore()->retrieve( constResult,  m_resultKey ).isFailure() || constResult == nullptr ) {
    ATH_MSG_WARNING( "Unable to load HLTResult using key " << m_resultKey );
    return StatusCode::SUCCESS;
  }

  HLT::HLTResult *result = const_cast<HLT::HLTResult *>(constResult);

  ATH_MSG_DEBUG( "Scheduling trigger navigation slimming in the HLT reesult " << m_resultKey );
  ATH_MSG_DEBUG( "Navigation payload size " << constResult->getNavigationResult().size() << " words" );

  HLT::TrigNavigationSlimmingHdlr *hdlr = new HLT::TrigNavigationSlimmingHdlr(result, this);
  hdlr->setSlimmer(m_slimmingTool.operator->());  

  CHECK(m_thinningSvc->register_slimmer(hdlr));

  return StatusCode::SUCCESS;
}

