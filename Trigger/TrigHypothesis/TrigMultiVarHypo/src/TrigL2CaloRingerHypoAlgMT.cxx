/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#include <map>
#include "GaudiKernel/Property.h"
#include "TrigL2CaloRingerHypoAlgMT.h"
#include "AthViews/ViewHelper.h"
#include "DecisionHandling/HLTIdentifier.h"
#include "DecisionHandling/TrigCompositeUtils.h"
#include "AthViews/ViewHelper.h"


using TrigCompositeUtils::DecisionContainer;
using TrigCompositeUtils::DecisionAuxContainer;
using TrigCompositeUtils::DecisionIDContainer;
using TrigCompositeUtils::decisionIDs;
using TrigCompositeUtils::newDecisionIn;
using TrigCompositeUtils::linkToPrevious;
using TrigCompositeUtils::createAndStore; 

TrigL2CaloRingerHypoAlgMT::TrigL2CaloRingerHypoAlgMT( const std::string& name, 
              ISvcLocator* pSvcLocator ) : 
  ::HypoBase( name, pSvcLocator ) {}


TrigL2CaloRingerHypoAlgMT::~TrigL2CaloRingerHypoAlgMT() {}


StatusCode TrigL2CaloRingerHypoAlgMT::initialize() {
  
  ATH_MSG_INFO ( "Initializing " << name() << "..." );
  ATH_CHECK( m_hypoTools.retrieve() );  
  ATH_CHECK( m_ringsKey.initialize());
  ATH_CHECK( m_clustersKey.initialize());
  renounce(m_clustersKey);
  renounce(m_ringsKey);

  return StatusCode::SUCCESS;
}

StatusCode TrigL2CaloRingerHypoAlgMT::finalize() {
  return StatusCode::SUCCESS;
}

StatusCode TrigL2CaloRingerHypoAlgMT::execute( const EventContext& context ) const {  
  
  ATH_MSG_DEBUG ( "Executing " << name() << "..." );
  auto previousDecisionsHandle = SG::makeHandle( decisionInput(), context );
  if( not previousDecisionsHandle.isValid() ) {//implicit
    ATH_MSG_DEBUG( "No implicit RH for previous decisions "<<  decisionInput().key()<<": is this expected?" );
    return StatusCode::SUCCESS;      
  }
  
  ATH_MSG_DEBUG( "Running with "<< previousDecisionsHandle->size() <<" implicit ReadHandles for previous decisions");
  // new decisions
  // new output decisions
  SG::WriteHandle<DecisionContainer> outputHandle = createAndStore(decisionOutput(), context ); 
  auto decisions = outputHandle.ptr();
  // input for decision
  std::vector<TrigL2CaloRingerHypoToolMT::RingerInfo> hypoToolInput;
  
  // loop over previous decisions
  size_t counter=0;
  for ( auto previousDecision: *previousDecisionsHandle ) {
    
    // get View
    auto view = TrigCompositeUtils::findLink< ViewContainer >( previousDecision, "view" );
    ATH_CHECK( view.isValid() );
    auto ringerShapeHandle = ViewHelper::makeHandle( *(view.link), m_ringsKey, context);
    ATH_CHECK( ringerShapeHandle.isValid() );
    ATH_MSG_DEBUG ( "Ringer handle size: " << ringerShapeHandle->size() << "..." );
    // create new decision
    auto d = newDecisionIn( decisions,  name() );
    hypoToolInput.emplace_back( TrigL2CaloRingerHypoToolMT::RingerInfo{ d, ringerShapeHandle.cptr()->at(0) } );
    
    {
      auto el = ViewHelper::makeLink( *(view.link), ringerShapeHandle, 0 );
      ATH_CHECK( el.isValid() );
      d->setObjectLink( "feature",  el );
    }
    
    TrigCompositeUtils::linkToPrevious( d, decisionInput().key(), counter );
    counter++;
  }

  ATH_MSG_DEBUG( "Found "<<hypoToolInput.size()<<" inputs to tools");
   
  for ( auto& tool: m_hypoTools ) {
    ATH_CHECK( tool->decide( hypoToolInput ) );
  }
 

  {// make output handle and debug
    ATH_MSG_DEBUG ( "Exit with "<<outputHandle->size() <<" decisions");
    TrigCompositeUtils::DecisionIDContainer allPassingIDs;
    if ( outputHandle.isValid() ) {
      for ( auto decisionObject: *outputHandle )  {
        TrigCompositeUtils::decisionIDs( decisionObject, allPassingIDs );
      }
      for ( TrigCompositeUtils::DecisionID id : allPassingIDs ) {
        ATH_MSG_DEBUG( " +++ " << HLT::Identifier( id ) );
      }
    }
  }

  return StatusCode::SUCCESS;
}
