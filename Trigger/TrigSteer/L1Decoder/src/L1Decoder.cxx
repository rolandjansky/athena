
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
  //  CHECK( m_prescaler.retrieve() );

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
  sort( l1SeededChains.begin(), l1SeededChains.end() ); // do so that following scaling is reproducable

  HLT::IDVec activeChains;
  activeChains.reserve( l1SeededChains.size() ); // an optimisation, max we get as many active chains as were seeded by L1, rarely the condition, but allows to avoid couple of reallocations
  CHECK( prescaleChains( l1SeededChains, activeChains) );
  
  CHECK( saveChainsInfo( l1SeededChains, chainsInfo.get(), "l1seeded" ) );
  CHECK( saveChainsInfo( activeChains, chainsInfo.get(), "unprescaled" ) );

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

StatusCode L1Decoder::prescaleChains( const HLT::IDVec& active,
				      HLT::IDVec& notPrescaled ) const {

  // intention is to use the same RNG scalers as in current steering version but it has to be refactored as follows
  // read in the CTP info and get the time
  // auto ScalerState state = HLT::RandomScaler::initState(ctx);
  // 
  
  for ( auto c: active ) {
    auto psInfo = m_prescalingInfo.find( c );
    if ( psInfo == m_prescalingInfo.end() )  {
      ATH_MSG_INFO("No prescaling information for the chain, enabling it " << c);
      notPrescaled.push_back( c );
    } else {
    
      // this code should then work
      //    if ( scaler.decision( state, psInfo.second ) ) {
      //      notPrescaled.push_back(c);
      //      ATH_MSG_DEBUG("Chain " << c << " remained active after the HTL prescaling");
      // but for now
      notPrescaled.push_back(c);
    }
  }
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
