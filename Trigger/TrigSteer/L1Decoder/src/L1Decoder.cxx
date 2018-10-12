
/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#include "xAODTrigger/TrigCompositeAuxContainer.h"
#include "TrigConfHLTData/HLTUtils.h"
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
    ATH_CHECK( m_RoIBResultKey.initialize( ) );
  
  ATH_CHECK( m_chainsKey.initialize() );
  ATH_CHECK( m_startStampKey.initialize() );

  ATH_CHECK( m_ctpUnpacker.retrieve() );
  ATH_CHECK( m_roiUnpackers.retrieve() );
  ATH_CHECK( m_prescaler.retrieve() );
  ATH_CHECK( m_rerunRoiUnpackers.retrieve() );

  ServiceHandle<IIncidentSvc> incidentSvc("IncidentSvc", "CTPSimulation");
  ATH_CHECK(incidentSvc.retrieve());
  incidentSvc->addListener(this,"BeginRun", 200);
  incidentSvc.release().ignore();

  if (m_enableCostMonitoring) {
    ATH_CHECK( m_trigCostSvcHandle.retrieve() );
  }

  return StatusCode::SUCCESS;
}


void L1Decoder::handle(const Incident& incident) {
  if (incident.type()!="BeginRun") return;
  ATH_MSG_DEBUG( "In L1Decoder BeginRun incident" );

  for ( auto t: m_roiUnpackers )
    if ( t->updateConfiguration( m_chainToCTPProperty ).isFailure() ) {
      ATH_MSG_ERROR( "Problem in configuring " << t->name() );
    }
  if ( m_ctpUnpacker->updateConfiguration( m_chainToCTPProperty ).isFailure() ) {
    ATH_MSG_ERROR( "Problem in configuring CTP unpacker tool" );
  }
}


StatusCode L1Decoder::readConfiguration() {
  return StatusCode::SUCCESS;
}

StatusCode L1Decoder::execute_r (const EventContext& ctx) const {
  {
    auto timeStampHandle = SG::makeHandle( m_startStampKey, ctx );
    ATH_CHECK( timeStampHandle.record( std::make_unique<TrigTimeStamp>() ) );
  }
  using namespace TrigCompositeUtils;
  const ROIB::RoIBResult* roib=0;
  if ( not m_RoIBResultKey.key().empty() ) {
    SG::ReadHandle<ROIB::RoIBResult> roibH( m_RoIBResultKey, ctx );
    roib = roibH.cptr();
    ATH_MSG_DEBUG( "Obtained ROIB result" );
  }
  // this should really be: const ROIB::RoIBResult* roib = SG::INPUT_PTR (m_RoIBResultKey, ctx);
  // or const ROIB::RoIBResult& roib = SG::INPUT_REF (m_RoIBResultKey, ctx);



  auto chainsInfo = std::make_unique<DecisionContainer>();
  auto chainsAux = std::make_unique<DecisionAuxContainer>();
  chainsInfo->setStore(chainsAux.get());  

  HLT::IDVec l1SeededChains;
  ATH_CHECK( m_ctpUnpacker->decode( *roib, l1SeededChains ) );
  sort( l1SeededChains.begin(), l1SeededChains.end() ); // do so that following scaling is reproducible

  HLT::IDVec activeChains;  

  ATH_CHECK( m_prescaler->prescaleChains( ctx, l1SeededChains, activeChains ) );    
  ATH_CHECK( saveChainsInfo( l1SeededChains, chainsInfo.get(), "l1seeded" ) );
  ATH_CHECK( saveChainsInfo( activeChains, chainsInfo.get(), "unprescaled" ) );

  // for now all the chains that were pre-scaled are st to re-run
  HLT::IDVec rerunChains;  
  std::set_difference( l1SeededChains.begin(), l1SeededChains.end(),
		       activeChains.begin(), activeChains.end(),
		       std::back_inserter(rerunChains) );

  ATH_CHECK( saveChainsInfo( rerunChains, chainsInfo.get(), "rerun" ) );

  // Do cost monitoring, this utilises the HLT_costmonitor chain
  if (m_enableCostMonitoring) {
    const static HLT::Identifier costMonitorChain(m_costMonitoringChain);
    const auto activeCostMonIt = std::find(activeChains.begin(), activeChains.end(), costMonitorChain);
    const bool doCostMonitoring = (activeCostMonIt != activeChains.end());
    ATH_CHECK(m_trigCostSvcHandle->startEvent(ctx, doCostMonitoring));
  }

  ATH_MSG_DEBUG( "Unpacking RoIs" );  
  HLT::IDSet activeChainSet( activeChains.begin(), activeChains.end() );
  for ( auto unpacker: m_roiUnpackers ) {
    ATH_CHECK( unpacker->unpack( ctx, *roib, activeChainSet ) );
  }
  

  ATH_MSG_DEBUG( "Unpacking RoIs for re-running" );      
  HLT::IDSet rerunChainSet( rerunChains.begin(), rerunChains.end() );
  for ( auto unpacker: m_rerunRoiUnpackers ) {
    ATH_CHECK( unpacker->unpack( ctx, *roib, rerunChainSet ) );
  }
  

  ATH_MSG_DEBUG("Recording chains");

  auto handle = SG::makeHandle( m_chainsKey, ctx );
  ATH_CHECK( handle.record( std::move( chainsInfo ), std::move( chainsAux ) ) );

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
