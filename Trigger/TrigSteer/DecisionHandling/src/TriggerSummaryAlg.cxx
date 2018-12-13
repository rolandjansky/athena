/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/Property.h"
#include "TriggerSummaryAlg.h"
//#include "TrigSteerEvent/Chain.h"




TriggerSummaryAlg::TriggerSummaryAlg( const std::string& name, 
			  ISvcLocator* pSvcLocator ) : 
  ::AthReentrantAlgorithm( name, pSvcLocator ) {}

TriggerSummaryAlg::~TriggerSummaryAlg()
{}

StatusCode TriggerSummaryAlg::initialize()
{

  // processing of the chains mapping

  CHECK(  m_inputDecisionKey.initialize() );

  renounceArray( m_finalDecisionKeys );
  CHECK( m_finalDecisionKeys.initialize() );

  ATH_MSG_DEBUG("Will consume implicit decisions:" );
  for (auto& input: m_finalDecisionKeys){  
    ATH_MSG_DEBUG( " "<<input.key() );
  }

  CHECK( m_summaryKey.initialize() );

  CHECK( m_startStampKey.initialize() );

  CHECK( m_outputTools.retrieve() );

  if (m_enableCostMonitoring) {
    CHECK( m_trigCostSvcHandle.retrieve() );
    CHECK( m_costWriteHandleKey.initialize() );
  }

  return StatusCode::SUCCESS;
}

StatusCode TriggerSummaryAlg::execute(const EventContext& context) const
{
  
  // that is certain input
  auto l1DecisionHandle = SG::makeHandle(  m_inputDecisionKey, context );
  auto inputHandles( m_finalDecisionKeys.makeHandles() );
  TrigCompositeUtils::DecisionIDContainer allPassingIDs;
  for ( auto input: inputHandles ) {
    if ( input.isValid() ) {
      for ( auto decisionObject: *input )  {
        TrigCompositeUtils::decisionIDs( decisionObject, allPassingIDs );
      }
      ATH_MSG_DEBUG( "Found "<<input->size()<<" Decisions for " << input.key() );
    } else {
      ATH_MSG_DEBUG( "Missing decisions for " << input.key() << " which may be perfectly correct" );
    }
  }
  
  ATH_MSG_DEBUG( "In summary " << allPassingIDs.size() << " chains passed:" );
  for ( TrigCompositeUtils::DecisionID id : allPassingIDs ) {
    ATH_MSG_DEBUG( " +++ " << HLT::Identifier( id ) );
  }  

  // check for an evident error, this is HLT chain not mentioned at the L1
  // that is the only reason we pull the L1 here

  SG::WriteHandle<TrigCompositeUtils::DecisionContainer> summaryHandle = TrigCompositeUtils::createAndStore(m_summaryKey, context ); 
  auto summaryCont = summaryHandle.ptr();

  auto summaryObj = TrigCompositeUtils::newDecisionIn( summaryCont );
  summaryObj->setName( "passing" );
  for ( auto id: allPassingIDs ) {
    TrigCompositeUtils::addDecisionID( id, summaryObj );
  }

  // if ( ! m_hltResultKey.empty() ) {
  //   auto result = std::make_unique<HLT::HLTResult>();
  //   CHECK( buildHLTResult( result ) );
  // }
  

  for ( auto& tool: m_outputTools ) {
    CHECK( tool->createOutput( context ) );
  }

  auto timeStampHandle = SG::makeHandle( m_startStampKey, context );
  ATH_MSG_DEBUG( "Time since the start of L1 decoding " << timeStampHandle.cptr()->millisecondsSince()  << " ms" );

  // Do cost monitoring
  if (m_enableCostMonitoring) {
    SG::WriteHandle<xAOD::TrigCompositeContainer> costMonOutput = TrigCompositeUtils::createAndStore(m_costWriteHandleKey, context);
    // Populate collection (assuming monitored event, otherwise collection will remain empty)
    ATH_CHECK(m_trigCostSvcHandle->endEvent(context, costMonOutput));
  }

  return StatusCode::SUCCESS;
}
