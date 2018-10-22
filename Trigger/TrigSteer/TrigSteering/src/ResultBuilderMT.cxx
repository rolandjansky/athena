/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigSteering/ResultBuilderMT.h"

// =============================================================================
// Standard constructor
// =============================================================================
HLT::ResultBuilderMT::ResultBuilderMT(const std::string& type, const std::string& name, const IInterface* parent)
  : AthAlgTool(type, name, parent) {
  declareProperty("HLTResultWHKey", m_hltResultWHKey="HLTResult");
}

// =============================================================================
// Standard destructor
// =============================================================================
HLT::ResultBuilderMT::~ResultBuilderMT() {}

// =============================================================================
// Implementation of IStateful::initialize
// =============================================================================
StatusCode HLT::ResultBuilderMT::initialize() {
  ATH_MSG_VERBOSE("start of " << __FUNCTION__);
  ATH_CHECK(m_hltResultWHKey.initialize());
  ATH_MSG_VERBOSE("end of " << __FUNCTION__);
  return StatusCode::SUCCESS;
}

// =============================================================================
// Implementation of IStateful::finalize
// =============================================================================
StatusCode HLT::ResultBuilderMT::finalize() {
  ATH_MSG_VERBOSE("start of " << __FUNCTION__);
  ATH_MSG_VERBOSE("end of " << __FUNCTION__);
  return StatusCode::SUCCESS;
}

// =============================================================================
// The main method of the tool
// =============================================================================
StatusCode HLT::ResultBuilderMT::buildResult(const EventContext& eventContext) const {
  ATH_MSG_VERBOSE("start of " << __FUNCTION__);
  auto hltResult = SG::makeHandle(m_hltResultWHKey,eventContext);
  ATH_CHECK( hltResult.record(std::make_unique<HLT::HLTResultMT>()) );
  ATH_MSG_DEBUG("Recorded HLTResultMT with key " << m_hltResultWHKey.key());

  // Dummy data for testing
  hltResult->addStreamTag({"DummyStreamTag1",eformat::TagType::PHYSICS_TAG,true});
  hltResult->addStreamTag({"DummyStreamTag2",eformat::TagType::CALIBRATION_TAG,true});
  hltResult->addHltBitsWord(0x00000002);
  hltResult->addHltBitsWord(0x00000020);
  hltResult->addSerialisedData(0,{0x01234567,0x89ABCDEF});
  hltResult->addSerialisedData(5,{0xFEDCBA98,0x76543210});

  ATH_MSG_VERBOSE("end of " << __FUNCTION__);
  return StatusCode::SUCCESS;
}
