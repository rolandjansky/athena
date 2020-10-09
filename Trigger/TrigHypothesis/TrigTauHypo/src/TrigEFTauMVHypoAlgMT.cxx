/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "Gaudi/Property.h"
#include "TrigEFTauMVHypoAlgMT.h"
#include "TrigCompositeUtils/HLTIdentifier.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "AthViews/ViewHelper.h"

using namespace TrigCompositeUtils;

TrigEFTauMVHypoAlgMT::TrigEFTauMVHypoAlgMT( const std::string& name, 
				      ISvcLocator* pSvcLocator ) :
  ::HypoBase( name, pSvcLocator ) {}


StatusCode TrigEFTauMVHypoAlgMT::initialize() {

  ATH_CHECK( m_hypoTools.retrieve() );
  ATH_CHECK( m_tauJetKey.initialize() );
  renounce( m_tauJetKey );// tau candidates are made in views, so they are not in the EvtStore: hide them

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
    auto roiELInfo = findLink<TrigRoiDescriptorCollection>( previousDecision, roiString());
    ATH_CHECK( roiELInfo.isValid() );
    const TrigRoiDescriptor* roi = *(roiELInfo.link);

    // get View
    const auto viewEL = previousDecision->objectLink<ViewContainer>( viewString() );
    ATH_CHECK( viewEL.isValid() );
    auto tauJetHandle = ViewHelper::makeHandle( *viewEL, m_tauJetKey, context);
    
    if( not tauJetHandle.isValid() ) {
      ATH_MSG_WARNING("Something is wrong, missing tau jets, continuing anyways skipping view");
      continue;      
    }
    ATH_MSG_DEBUG ( "TauJet handle size: " << tauJetHandle->size() << "..." );

    if( tauJetHandle->size() != 1 ) {
      ATH_MSG_WARNING("Something is wrong, unexpectd number of tau jets " << tauJetHandle->size() << " is found (expected 1), continuing anyways skipping view");
      continue;
    }

    // create new decision
    auto d = newDecisionIn( decisions, name() );
    TrigCompositeUtils::linkToPrevious( d, decisionInput().key(), counter );
    d->setObjectLink( roiString(), roiELInfo.link );

    auto el = ViewHelper::makeLink( *viewEL, tauJetHandle, 0 );
    ATH_CHECK( el.isValid() );
    d->setObjectLink( featureString(),  el );

    toolInput.emplace_back( d, roi, tauJetHandle.cptr(), previousDecision );

    ATH_MSG_DEBUG( "Added view, roi, cluster, previous decision to new decision " << counter << " for view " << (*viewEL)->name()  );

  }

  ATH_MSG_DEBUG( "Found " << toolInput.size() << " inputs to tools");

   
  for ( auto& tool: m_hypoTools ) {
    ATH_CHECK( tool->decide( toolInput ) );
  }
 
  ATH_CHECK( hypoBaseOutputProcessing(outputHandle) );

  return StatusCode::SUCCESS;
}
