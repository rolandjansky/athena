/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigOutputHandling/HLTResultMTMaker.h"

// =============================================================================
// Standard constructor
// =============================================================================
HLTResultMTMaker::HLTResultMTMaker(const std::string& type, const std::string& name, const IInterface* parent)
  : AthAlgTool(type, name, parent) {
  declareProperty("HLTResultWHKey", m_hltResultWHKey="HLTResult");
}

// =============================================================================
// Standard destructor
// =============================================================================
HLTResultMTMaker::~HLTResultMTMaker() {}

// =============================================================================
// Implementation of IStateful::initialize
// =============================================================================
StatusCode HLTResultMTMaker::initialize() {
  ATH_MSG_VERBOSE("start of " << __FUNCTION__);
  ATH_CHECK(m_hltResultWHKey.initialize());
  ATH_MSG_VERBOSE("end of " << __FUNCTION__);
  return StatusCode::SUCCESS;
}

// =============================================================================
// Implementation of IStateful::finalize
// =============================================================================
StatusCode HLTResultMTMaker::finalize() {
  ATH_MSG_VERBOSE("start of " << __FUNCTION__);
  ATH_MSG_VERBOSE("end of " << __FUNCTION__);
  return StatusCode::SUCCESS;
}

// =============================================================================
// The main method of the tool
// =============================================================================
StatusCode HLTResultMTMaker::makeResult(const EventContext& eventContext) const {
  ATH_MSG_VERBOSE("start of " << __FUNCTION__);
  auto hltResult = SG::makeHandle(m_hltResultWHKey,eventContext);
  ATH_CHECK( hltResult.record(std::make_unique<HLTResultMT>()) );
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
