/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigOutputHandling/HLTResultMTMaker.h"
#include "AthenaMonitoring/Monitored.h"

// =============================================================================
// Standard constructor
// =============================================================================
HLTResultMTMaker::HLTResultMTMaker(const std::string& type, const std::string& name, const IInterface* parent)
  : AthAlgTool(type, name, parent) {}

// =============================================================================
// Standard destructor
// =============================================================================
HLTResultMTMaker::~HLTResultMTMaker() {}

// =============================================================================
// Implementation of IStateful::initialize
// =============================================================================
StatusCode HLTResultMTMaker::initialize() {
  ATH_CHECK(m_hltResultWHKey.initialize());
  ATH_CHECK(m_makerTools.retrieve());
  ATH_CHECK(m_monTool.retrieve());
  return StatusCode::SUCCESS;
}

// =============================================================================
// Implementation of IStateful::finalize
// =============================================================================
StatusCode HLTResultMTMaker::finalize() {
  ATH_CHECK(m_monTool.release());
  return StatusCode::SUCCESS;
}

// =============================================================================
// The main method of the tool
// =============================================================================
StatusCode HLTResultMTMaker::makeResult(const EventContext& eventContext) const {

  // Create and record the HLTResultMT object
  auto hltResult = SG::makeHandle(m_hltResultWHKey,eventContext);
  ATH_CHECK( hltResult.record(std::make_unique<HLT::HLTResultMT>()) );
  ATH_MSG_DEBUG("Recorded HLTResultMT with key " << m_hltResultWHKey.key());

  // Fill the object using the result maker tools
  auto time =  Monitored::Timer("TIME_build" );
  for (auto& maker: m_makerTools) {
    ATH_CHECK(maker->fill(*hltResult));
  }
  time.stop();

  // Fill monitoring histograms
  auto nstreams = Monitored::Scalar("nstreams", hltResult->getStreamTags().size());
  auto bitWords = Monitored::Scalar("bitWords", hltResult->getHltBits().size());
  auto nfrags   = Monitored::Scalar("nfrags",   hltResult->getSerialisedData().size());
  auto sizeMain = Monitored::Scalar("sizeMain", -1.);
  auto iter = hltResult->getSerialisedData().find(0); // this is the main fragment of the HLT result
  if (iter != hltResult->getSerialisedData().end())
    sizeMain = double(iter->second.size()*sizeof(uint32_t))/1024;

  Monitored::Group(m_monTool, time, nstreams, nfrags, sizeMain, bitWords);

  return StatusCode::SUCCESS;
}
