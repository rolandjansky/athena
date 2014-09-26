/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigSteeringEvent/HLTResult.h"
#include "TrigNavigationThinningTool.h"
#include "TrigNavTools/TrigNavigationSlimmingHdlr.h"

TrigNavigationThinningTool::TrigNavigationThinningTool(const std::string& type, const std::string& name, const IInterface* parent) 
    : AthAlgTool(type, name, parent), 
      m_thinningSvc("IThinningSvc/ThinningSvc", name),
      m_slimmingTool("", this)  
      //      m_slimmingTool("HLT::TrigNavigationSlimmingTool/TrigNavigationSlimmingTool", this)  // a private instansce
{
  declareInterface<DerivationFramework::IThinningTool>(this);
  declareProperty("ThinningSvc", m_thinningSvc, "Synchronize feature indexes with this instance of ThinningSvc");
  declareProperty("SlimmingTool", m_slimmingTool, "Tool instance which does the actual slimming");
  declareProperty("ResultKey", m_resultKey = "HLTResult_HLT");
  declareProperty("ActInPlace", m_actInPlace = false, "Run slimming on the spot (creates messes when multiple output streams!)");
}

TrigNavigationThinningTool::~TrigNavigationThinningTool(){
}

StatusCode TrigNavigationThinningTool::initialize() {
  ATH_MSG_DEBUG( "TrigNavigationThinningTool::initialize()" );
  CHECK( m_thinningSvc.retrieve() ); 
  CHECK( m_slimmingTool.retrieve() );
  CHECK( m_slimmingTool->initialize() );
  return  StatusCode::SUCCESS;
}
StatusCode TrigNavigationThinningTool::finalize() {
  return  StatusCode::SUCCESS;
}

StatusCode TrigNavigationThinningTool::doThinning() const {
  ATH_MSG_DEBUG( "StreamTrigNavSlimming::execute()" );

  // check if the result is there (do nothing if is not)
  if ( not evtStore()->contains<HLT::HLTResult>( m_resultKey ) ) {
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
  
  if ( m_actInPlace ) {
    result->getNavigationResultCuts().clear();
    CHECK(m_slimmingTool->doSlimming(result->getNavigationResult()));
  } else {
    ATH_MSG_DEBUG( "Scheduling trigger navigation slimming in the HLT result " << m_resultKey );
    ATH_MSG_DEBUG( "Navigation payload size " << constResult->getNavigationResult().size() << " words" );  
    HLT::TrigNavigationSlimmingHdlr *hdlr 
      = new HLT::TrigNavigationSlimmingHdlr(result, this, m_slimmingTool.operator->());
    
    CHECK(m_thinningSvc->register_slimmer(hdlr));
  }
  return StatusCode::SUCCESS;
}


