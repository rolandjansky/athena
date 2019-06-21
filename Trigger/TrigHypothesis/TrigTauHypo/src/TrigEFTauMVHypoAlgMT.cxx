/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/Property.h"
#include "TrigEFTauMVHypoAlgMT.h"
#include "DecisionHandling/HLTIdentifier.h"
#include "DecisionHandling/TrigCompositeUtils.h"
#include "AthViews/ViewHelper.h"

using namespace TrigCompositeUtils;

TrigEFTauMVHypoAlgMT::TrigEFTauMVHypoAlgMT( const std::string& name, 
				      ISvcLocator* pSvcLocator ) :
  ::HypoBase( name, pSvcLocator ) {}

TrigEFTauMVHypoAlgMT::~TrigEFTauMVHypoAlgMT() {}

StatusCode TrigEFTauMVHypoAlgMT::initialize() {
  ATH_MSG_INFO ( "Initializing " << name() << "..." );
  ATH_CHECK( m_hypoTools.retrieve() );
  ATH_CHECK( m_tauJetKey.initialize() );
  renounce( m_tauJetKey );// tau candidates are made in views, so they are not in the EvtStore: hide them

  return StatusCode::SUCCESS;
}

StatusCode TrigEFTauMVHypoAlgMT::finalize() {   
  return StatusCode::SUCCESS;
}

StatusCode TrigEFTauMVHypoAlgMT::execute( const EventContext& context ) const {  
  ATH_MSG_DEBUG ( "Executing " << name() << "..." );
  auto previousDecisionsHandle = SG::makeHandle( decisionInput(), context );
  if( not previousDecisionsHandle.isValid() ) {//implicit
    ATH_MSG_DEBUG( "No implicit RH for previous decisions "<<  decisionInput().key()<<": is this expected?" );
    return StatusCode::SUCCESS;      
  }
  
  ATH_MSG_DEBUG( "Running with "<< previousDecisionsHandle->size() <<" previous decisions");

  // new decisions

  // new output decisions
  SG::WriteHandle<DecisionContainer> outputHandle = createAndStore(decisionOutput(), context ); 
  auto decisions = outputHandle.ptr();

  // input for decision
  std::vector<ITrigEFTauMVHypoTool::TauJetInfo> toolInput;

  // loop over previous decisions
  int counter=-1;
  for ( auto previousDecision: *previousDecisionsHandle ) {
    counter++;
    //get RoI
    auto roiELInfo = TrigCompositeUtils::findLink<TrigRoiDescriptorCollection>( previousDecision, "initialRoI");
    ATH_CHECK( roiELInfo.isValid() );
    const TrigRoiDescriptor* roi = *(roiELInfo.link);

    // get View
    auto viewELInfo = TrigCompositeUtils::findLink< ViewContainer >( previousDecision, "view" );
    ATH_CHECK( viewELInfo.isValid() );
    auto tauJetHandle = ViewHelper::makeHandle( *(viewELInfo.link), m_tauJetKey, context);
    
    if( not tauJetHandle.isValid() ) {
      ATH_MSG_WARNING("Something is wrong, missing tau jets, continuing anyways skipping view");
      continue;      
    }
    ATH_MSG_DEBUG ( "TauJet handle size: " << tauJetHandle->size() << "..." );

    if( tauJetHandle->size() != 1 ) {
      ATH_MSG_WARNING("Something is wrong, unexpectd number of tau jets " << tauJetHandle->size() << " is found (expected 1), continuing anyways skipping view");
      continue;
    }

    ATH_MSG_DEBUG( name() << " running with store " << context.getExtension<Atlas::ExtendedEventContext>().proxy()->name() );

    ATH_MSG_DEBUG("Event store dump: " << evtStore()->dump() );

    // create new decision
    auto d = newDecisionIn( decisions, name() );
    TrigCompositeUtils::linkToPrevious( d, decisionInput().key(), counter );
    d->setObjectLink( "roi", roiELInfo.link );

    auto el = ViewHelper::makeLink( *(viewELInfo.link), tauJetHandle, 0 );
    ATH_CHECK( el.isValid() );
    d->setObjectLink( "feature",  el );

    toolInput.emplace_back( d, roi, tauJetHandle.cptr(), previousDecision );

    ATH_MSG_DEBUG( "Added view, roi, cluster, previous decision to new decision " << counter << " for view " << (*viewELInfo.link)->name()  );

  }

  ATH_MSG_DEBUG( "Found " << toolInput.size() << " inputs to tools");

   
  for ( auto& tool: m_hypoTools ) {
    ATH_CHECK( tool->decide( toolInput ) );
  }
 
  {// make output handle and debug
    ATH_MSG_DEBUG ( "Exit with " << outputHandle->size() << " decisions");
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
