/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "./L1Decoder.h"

L1Decoder::L1Decoder(const std::string& name, ISvcLocator* pSvcLocator)
  : AthReentrantAlgorithm(name, pSvcLocator) {}

StatusCode L1Decoder::initialize() {
  //  CHECK( m_RoIBResult.retrieve() );
  CHECK( m_ctpUnpacker.retrieve() );
  //  CHECK( m_roisUnpacker.retrieve() );
  //  CHECK( m_prescaler.retrieve() );

  
  CHECK(readConfiguration());
  return StatusCode::SUCCESS;
}

StatusCode L1Decoder::readConfiguration() {
  return StatusCode::SUCCESS;
}


StatusCode L1Decoder::execute_r (const EventContext& ctx) const {
  // this is the implementation draft
  //  obtain RoIB result
  //  if ( not m_RoIBResult.isValid() ) {
  //    ATH_MSG_WARNING("No L1 result");
  //    return StatusCode::RECOVERABLE;
  //  }
  //  std::vector<HLT::Identifier> activeChains;
  //  CHECK( m_ctpUnpacker->decode(roib.get(), m_ctpIDToChain, activeChains) );
  // m_prescaler->applyPrescaling(activated_chains);

  //  for ( auto unpacker: m_roiUnpackers ) {
  //    CHECK( unpacker->unpack(roib, activated_chains) );
  //  }

  //  OUTPUT(m_chains) <<;
  // TODO add monitoring
  return StatusCode::SUCCESS;
  
}
StatusCode L1Decoder::finalize() {
  return StatusCode::SUCCESS;
}


