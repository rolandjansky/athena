
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#include "StoreGate/WriteHandle.h"
#include "GaudiKernel/EventContext.h"
#include "xAODTrigger/TrigCompositeAuxContainer.h"
#include "TrigConfHLTData/HLTUtils.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "TrigConfxAOD/KeyWriterTool.h"
#include "L1Decoder.h"

L1Decoder::L1Decoder(const std::string& name, ISvcLocator* pSvcLocator)
  : AthReentrantAlgorithm(name, pSvcLocator)
{
}


StatusCode L1Decoder::initialize() {
  ATH_MSG_INFO( "Reading RoIB infromation from: "<< m_RoIBResultKey.objKey() << " : " << m_RoIBResultKey.fullKey() << " : " << m_RoIBResultKey.key() );

  if ( m_RoIBResultKey.empty() )
    ATH_MSG_INFO( "RoIBResultKey empty: assume we're running with CTP emulation" );

  ATH_CHECK( m_RoIBResultKey.initialize(!m_RoIBResultKey.empty()) );
  ATH_CHECK( m_l1TriggerResultKey.initialize(!m_l1TriggerResultKey.empty()) );

  ATH_CHECK( m_summaryKey.initialize() );
  ATH_CHECK( m_startStampKey.initialize() );

  ATH_CHECK( m_ctpUnpacker.retrieve() );
  ATH_CHECK( m_roiUnpackers_roib.retrieve() );
  ATH_CHECK( m_roiUnpackers_xaod.retrieve() );
  ATH_CHECK( m_prescaler.retrieve() );

  if ( !m_keyWriterTool.empty() ) {
    ATH_CHECK( m_keyWriterTool.retrieve() );
  }

  ServiceHandle<IIncidentSvc> incidentSvc("IncidentSvc", "CTPSimulation");
  ATH_CHECK(incidentSvc.retrieve());
  incidentSvc->addListener(this,"BeginRun", 200);
  incidentSvc.release().ignore();

  if (m_doCostMonitoring) {
    ATH_CHECK( m_trigCostSvcHandle.retrieve() );
  }

  return StatusCode::SUCCESS;
}


void L1Decoder::handle(const Incident& incident) {
  if (incident.type()!="BeginRun") return;
  ATH_MSG_DEBUG( "In L1Decoder BeginRun incident" );
    
  for ( auto t: m_roiUnpackers_roib ) {
    if ( t->updateConfiguration().isFailure() ) {
      ATH_MSG_ERROR( "Problem in configuring " << t->name() );
    }
  }

  for ( auto t: m_roiUnpackers_xaod ) {
    if ( t->updateConfiguration( ).isFailure() ) {
      ATH_MSG_ERROR( "Problem in configuring " << t->name() );
    }
  }
}


StatusCode L1Decoder::readConfiguration() {
  return StatusCode::SUCCESS;
}


StatusCode L1Decoder::execute (const EventContext& ctx) const {
  {
    auto timeStampHandle = SG::makeHandle( m_startStampKey, ctx );
    ATH_CHECK( timeStampHandle.record( std::make_unique<TrigTimeStamp>() ) );
  }
  using namespace TrigCompositeUtils;
  const bool decodeRoIB = !m_RoIBResultKey.empty();
  const bool decodexAOD = !m_l1TriggerResultKey.empty();

  const ROIB::RoIBResult* roib = nullptr;
  if (decodeRoIB) {
    SG::ReadHandle<ROIB::RoIBResult> roibH( m_RoIBResultKey, ctx );
    roib = roibH.cptr();
    ATH_MSG_DEBUG( "Obtained ROIB result" );
  }

  const xAOD::TrigComposite* l1TriggerResult = nullptr;
  if (decodexAOD) {
    auto l1TriggerResultCont = SG::makeHandle(m_l1TriggerResultKey, ctx);
    if (!l1TriggerResultCont.isValid()) {
      ATH_MSG_ERROR("Failed to retrieve L1TriggerResult with key " << m_l1TriggerResultKey.key());
      return StatusCode::FAILURE;
    }
    if (l1TriggerResultCont->size() != 1) {
      ATH_MSG_ERROR("Size of the L1TriggerResultContainer is " << l1TriggerResultCont->size() << " but 1 expected");
      return StatusCode::FAILURE;
    }
    l1TriggerResult = l1TriggerResultCont->at(0);
    if (msgLvl(MSG::DEBUG)) {
      const std::vector<std::string>& linkNames = l1TriggerResult->linkColNames();
      const std::vector<uint32_t>& linkClids = l1TriggerResult->linkColClids();
      ATH_MSG_DEBUG("L1TriggerResult has " << linkNames.size() << " links:");
      for (size_t i=0; i<linkNames.size(); ++i) {
        ATH_MSG_DEBUG("--> " << linkNames.at(i) << " CLID: " << linkClids.at(i));
      }
    }
  }

  SG::WriteHandle<DecisionContainer> handle = TrigCompositeUtils::createAndStore( m_summaryKey, ctx );
  auto chainsInfo = handle.ptr();

  /*
  auto chainsInfo = std::make_unique<DecisionContainer>();
  auto chainsAux = std::make_unique<DecisionAuxContainer>();
  chainsInfo->setStore(chainsAux.get());
  ATH_MSG_DEBUG("Recording chains");
  auto handle = SG::makeHandle( m_chainsKey, ctx );
  ATH_CHECK( handle.record( std::move( chainsInfo ), std::move( chainsAux ) ) );
  */

  HLT::IDVec l1SeededChains;
  if (decodeRoIB) {
    ATH_CHECK( m_ctpUnpacker->decode( *roib, l1SeededChains ) );
  } else if (m_ctpUnpacker->isEmulated()) {
    ATH_CHECK( m_ctpUnpacker->decode( ROIB::RoIBResult{}, l1SeededChains ) );
  }
  sort( l1SeededChains.begin(), l1SeededChains.end() ); // do so that following scaling is reproducible

  HLT::IDVec activeChains; // Chains which are activated to run in the first pass (seeded and pass prescale)
  HLT::IDVec prescaledChains; // Chains which are activated but do not run in the first pass (seeded but prescaled out)

  std::set_difference( activeChains.begin(), activeChains.end(),
		       l1SeededChains.begin(), l1SeededChains.end(),
		       std::back_inserter(prescaledChains) );

  ATH_CHECK( m_prescaler->prescaleChains( ctx, l1SeededChains, activeChains ) );    
  ATH_CHECK( saveChainsInfo( l1SeededChains, chainsInfo, "l1seeded" ) );
  ATH_CHECK( saveChainsInfo( activeChains, chainsInfo, "unprescaled" ) );
  ATH_CHECK( saveChainsInfo( prescaledChains, chainsInfo, "prescaled" ) );
  //Note: 'prescaled' can be deduced from 'l1seeded' and 'unprescaled'. This non-persistent collection is provided for convenience.

  // Do cost monitoring, this utilises the HLT_costmonitor chain
  if (m_doCostMonitoring) {
    const static HLT::Identifier costMonitorChain(m_costMonitoringChain);
    const auto activeCostMonIt = std::find(activeChains.begin(), activeChains.end(), costMonitorChain);
    const bool doCostMonitoring = (activeCostMonIt != activeChains.end());
    ATH_CHECK(m_trigCostSvcHandle->startEvent(ctx, doCostMonitoring));
  }

  ATH_MSG_DEBUG( "Unpacking RoIs" );
  HLT::IDSet activeChainSet( activeChains.begin(), activeChains.end() );
  if (decodeRoIB) {
    for ( auto unpacker: m_roiUnpackers_roib ) {
      ATH_CHECK( unpacker->unpack( ctx, *roib, activeChainSet ) );
    }
  } else if (m_ctpUnpacker->isEmulated()) {
    ROIB::RoIBResult roib{};
    for ( auto unpacker: m_roiUnpackers_roib ) {
      ATH_CHECK( unpacker->unpack( ctx, roib, activeChainSet ) );
    }
  }
  if (decodexAOD) {
    for ( auto unpacker: m_roiUnpackers_xaod ) {
      ATH_CHECK( unpacker->unpack( ctx, *l1TriggerResult, activeChainSet ) );
    }
  }

  if ( !m_keyWriterTool.empty() ) {
    ATH_CHECK( m_keyWriterTool->writeKeys(ctx) );
  }

  return StatusCode::SUCCESS;  
}

StatusCode L1Decoder::finalize() {
  return StatusCode::SUCCESS;
}


StatusCode L1Decoder::saveChainsInfo(const HLT::IDVec& chains, xAOD::TrigCompositeContainer* storage, const std::string& type) const {
  using namespace TrigCompositeUtils;
  Decision* d = newDecisionIn( storage, type );
  for ( auto c: chains)
    addDecisionID(c.numeric(), d);
  return StatusCode::SUCCESS;
}
