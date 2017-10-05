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
  ATH_MSG_INFO ("Initializing " << name() << "...");
  // processing of the chains mapping

  CHECK( m_l1decisionKey.initialize() );

  renounceArray( m_finalDecisionKeys );
  CHECK( m_finalDecisionKeys.initialize() );

  CHECK( m_summaryKey.initialize() );

  return StatusCode::SUCCESS;
}

StatusCode TriggerSummaryAlg::execute_r(const EventContext& context) const
{  
  ATH_MSG_DEBUG ("Executing " << name() << "...");

  // that is certain input
  auto l1DecisionHandle = SG::makeHandle( m_l1decisionKey, context );
  auto inputHandles( m_finalDecisionKeys.makeHandles() );
  TrigCompositeUtils::DecisionIDContainer allPassingIDs;
  for ( auto input: inputHandles ) {
    if ( input.isValid() ) {
      ATH_MSG_DEBUG( "Partial result " << input.key() << " present" );
      for ( auto decisionObject: *input )  {
	TrigCompositeUtils::decisionIDs( decisionObject, allPassingIDs );
      }
    } else {
      ATH_MSG_DEBUG( "Missing input " << input.key() << " which may be perfectly correct" );
    }

  }
  ATH_MSG_DEBUG( "In summary " << allPassingIDs.size() << " chains passed" );

  // check for an evident error, this is HLT chain not mentioned at the L1
  // that is the only reason we pull the L1 here
 

  

  auto summaryCont = std::make_unique<TrigCompositeUtils::DecisionContainer>();
  auto summaryAuxCont = std::make_unique<TrigCompositeUtils::DecisionAuxContainer>();
  summaryCont->setStore( summaryAuxCont.get() );

  auto summaryObj = TrigCompositeUtils::newDecisionIn( summaryCont.get() );
  summaryObj->setName( "passing" );
  for ( auto id: allPassingIDs ) {
    TrigCompositeUtils::addDecisionID( id, summaryObj );
  }
  


  
  // if ( ! m_hltResultKey.empty() ) {
  //   auto result = std::make_unique<HLT::HLTResult>();
  //   CHECK( buildHLTResult( result ) );
  // }
  
  auto summaryHandle = SG::makeHandle( m_summaryKey, context );
  CHECK( summaryHandle.record( std::move( summaryCont ), std::move( summaryAuxCont ) ) );

  return StatusCode::SUCCESS;
}



