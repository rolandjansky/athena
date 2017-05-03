/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#include "xAODTrigger/TrigCompositeAuxContainer.h"
#include "./L1Decoder.h"

L1Decoder::L1Decoder(const std::string& name, ISvcLocator* pSvcLocator)
  : AthReentrantAlgorithm(name, pSvcLocator) {
  declareProperty("RoIBResult", m_RoIBResultKey="RoIBResult", "Name of RoIBResult");
  declareProperty("Chains", m_chainsKey="HLTChains", "Chains status after L1 and prescaling");
}

StatusCode L1Decoder::initialize() {
  CHECK( m_RoIBResultKey.initialize() );
  CHECK( m_chainsKey.initialize() );
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
  SG::ReadHandle<ROIB::RoIBResult> roibH(m_RoIBResultKey, ctx);

  // this should realy be: const ROIB::RoIBResult* roib = SG::INPUT_PTR (m_RoIBResultKey, ctx);
  // or const ROIB::RoIBResult& roib = SG::INPUT_REF (m_RoIBResultKey, ctx);

  auto chainsInfo = CxxUtils::make_unique<xAOD::TrigCompositeContainer>();
  auto chainsInfoAux = CxxUtils::make_unique<xAOD::TrigCompositeAuxContainer>();
  chainsInfo->setStore(chainsInfoAux.get());
  
  std::vector<HLT::Identifier> l1SeededChains;
  CHECK( m_ctpUnpacker->decode(roibH.get(), m_ctpIDToChain, l1SeededChains) );
  sort(l1SeededChains.begin(), l1SeededChains.end()); // do so that following scaling is reproducable
  std::vector<HLT::Identifier> activeChains;
  activeChains.reserve(l1SeededChains.size()); // an optimisation, max we get as many active chains as were seeded by L1, rarely the condition, but allows to avoid couple of reallocations
  CHECK( prescaleChains(l1SeededChains, activeChains));
  CHECK( saveChainsInfo(l1SeededChains, chainsInfo.get(), "l1seeded") );
  CHECK( saveChainsInfo(activeChains, chainsInfo.get(), "unprescaled") );

  //  for ( auto unpacker: m_roiUnpackers ) {
  //    CHECK( unpacker->unpack(roib, unprescaledChains) );
  //  }

  
  SG::WriteHandle<xAOD::TrigCompositeContainer> chainsH(m_chainsKey, ctx);
  ATH_CHECK( chainsH.record( std::move(chainsInfo), std::move(chainsInfoAux) ));
  // this should realy be as simple as this: ATH_CHECK( SG::OUTPUT(m_chainsKey, ctx) << xAODContainer(chains, chainsAux)); 
  
  // TODO add monitoring
  return StatusCode::SUCCESS;
  
}
StatusCode L1Decoder::finalize() {
  return StatusCode::SUCCESS;
}


StatusCode L1Decoder::prescaleChains(const HLT::IDVec& active,
				     HLT::IDVec& notPrescaled) const {

  // intention is to use the same RNG scalers as in current steering version but it has to be refactored as follows
  // read in the CTP info and get the time
  // auto ScalerState state = HLT::RandomScaler::initState(ctx);
  // 
  
  for ( auto c: active ) {
    auto psInfo = m_prescalingInfo.find(c);
    if ( psInfo == m_prescalingInfo.end() )  {
      ATH_MSG_ERROR("No prescaling information for the chain " << c);
      return StatusCode::FAILURE;
    }
    
    // this code should then work
    //    if ( scaler.decision( state, psInfo.second ) ) {
    //      notPrescaled.push_back(c);
    //      ATH_MSG_DEBUG("Chain " << c << " remained active after the HTL prescaling");
    // but for now
    notPrescaled.push_back(c);
  }
  return StatusCode::SUCCESS;
}

StatusCode L1Decoder::saveChainsInfo(const HLT::IDVec& chains, xAOD::TrigCompositeContainer* storage, const std::string& type) const {
  using namespace TrigCompositeUtils;
  Decision* d = newDecisionIn(storage);
  d->setName(type);
  for ( auto c: chains)
    addDecisionID(c.numeric(), d);
  return StatusCode::SUCCESS;
}
