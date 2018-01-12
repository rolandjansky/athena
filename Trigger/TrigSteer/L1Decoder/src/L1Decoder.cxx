
/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#include "xAODTrigger/TrigCompositeAuxContainer.h"
#include "L1Decoder.h"

L1Decoder::L1Decoder(const std::string& name, ISvcLocator* pSvcLocator)
  : AthReentrantAlgorithm(name, pSvcLocator)
{
}

StatusCode L1Decoder::initialize() {
  ATH_MSG_INFO( "Reading RoIB infromation from: "<< m_RoIBResultKey.objKey() << " : " << m_RoIBResultKey.fullKey() << " : " << m_RoIBResultKey.key() );

  if (  m_RoIBResultKey.objKey().empty() )
    renounce( m_RoIBResultKey );
  else
    CHECK( m_RoIBResultKey.initialize( ) );
  
  CHECK( m_chainsKey.initialize() );

  CHECK( m_ctpUnpacker.retrieve() );
  CHECK( m_roiUnpackers.retrieve() );
  CHECK( m_prescaler.retrieve() );

  return StatusCode::SUCCESS;
}

StatusCode L1Decoder::start() {

  for ( auto t: m_roiUnpackers )
    CHECK( t->updateConfiguration() );

  return StatusCode::SUCCESS;
}


StatusCode L1Decoder::readConfiguration() {
  return StatusCode::SUCCESS;
}

StatusCode L1Decoder::execute_r (const EventContext& ctx) const {
  using namespace TrigCompositeUtils;
  const ROIB::RoIBResult* roib=0;
  if ( not m_RoIBResultKey.key().empty() ) {
    SG::ReadHandle<ROIB::RoIBResult> roibH( m_RoIBResultKey, ctx );
    roib = roibH.cptr();
    ATH_MSG_DEBUG( "Obtained ROIB result" );
  }
  // this should realy be: const ROIB::RoIBResult* roib = SG::INPUT_PTR (m_RoIBResultKey, ctx);
  // or const ROIB::RoIBResult& roib = SG::INPUT_REF (m_RoIBResultKey, ctx);



  auto chainsInfo = std::make_unique<DecisionContainer>();
  auto chainsAux = std::make_unique<DecisionAuxContainer>();
  chainsInfo->setStore(chainsAux.get());  

  HLT::IDVec l1SeededChains;
  CHECK( m_ctpUnpacker->decode( *roib, l1SeededChains ) );
  sort( l1SeededChains.begin(), l1SeededChains.end() ); // do so that following scaling is reproducible

  HLT::IDVec activeChains;  

  CHECK( m_prescaler->prescaleChains( ctx, l1SeededChains, activeChains ) );    
  CHECK( saveChainsInfo( l1SeededChains, chainsInfo.get(), "l1seeded" ) );
  CHECK( saveChainsInfo( activeChains, chainsInfo.get(), "unprescaled" ) );

  // for now all the chains that were pre-scaled are st to re-run
  HLT::IDVec rerunChains;  
  std::set_difference( l1SeededChains.begin(), l1SeededChains.end(),
		       activeChains.begin(), activeChains.end(),
		       std::back_inserter(rerunChains) );

  CHECK( saveChainsInfo( rerunChains, chainsInfo.get(), "rerun" ) );


  HLT::IDSet activeChainSet( activeChains.begin(), activeChains.end() );
  for ( auto unpacker: m_roiUnpackers ) {
    CHECK( unpacker->unpack( ctx, *roib, activeChainSet ) );
  }
  ATH_MSG_DEBUG("Recording chains");

  auto handle = SG::makeHandle( m_chainsKey, ctx );
  CHECK( handle.record( std::move( chainsInfo ), std::move( chainsAux ) ) );

  return StatusCode::SUCCESS;  
}

StatusCode L1Decoder::finalize() {
  return StatusCode::SUCCESS;
}


StatusCode L1Decoder::saveChainsInfo(const HLT::IDVec& chains, xAOD::TrigCompositeContainer* storage, const std::string& type) const {
  using namespace TrigCompositeUtils;
  Decision* d = newDecisionIn( storage );
  d->setName(type);
  for ( auto c: chains)
    addDecisionID(c.numeric(), d);
  return StatusCode::SUCCESS;
}
