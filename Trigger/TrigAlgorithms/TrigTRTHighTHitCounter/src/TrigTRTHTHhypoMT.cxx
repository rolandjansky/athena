/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigTRTHTHhypoMT.h"

#include "AthViews/ViewHelper.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include "xAODTrigger/TrigCompositeContainer.h"

namespace TCU = TrigCompositeUtils;

TrigTRTHTHhypoMT::TrigTRTHTHhypoMT( const std::string& name, 
					  ISvcLocator* pSvcLocator ) :
  ::HypoBase( name, pSvcLocator ) {}

StatusCode TrigTRTHTHhypoMT::initialize() {

  ATH_CHECK( m_hypoTools.retrieve() );
  
  ATH_CHECK( m_trigRNNOutputKey.initialize() );

  renounce( m_trigRNNOutputKey );// clusters are made in views, so they are not in the EvtStore: hide them

  return StatusCode::SUCCESS;
}


StatusCode TrigTRTHTHhypoMT::execute( const EventContext& context ) const {  
  ATH_MSG_DEBUG ( "Executing " << name() << "..." );
  auto previousDecisionsHandle = SG::makeHandle( decisionInput(), context );
  ATH_CHECK( previousDecisionsHandle.isValid() );
  ATH_MSG_DEBUG( "Running with "<< previousDecisionsHandle->size() <<" previous decisions");


  // new decisions

  // new output decisions
  SG::WriteHandle<TCU::DecisionContainer> outputHandle = TCU::createAndStore(decisionOutput(), context );
  auto decisions = outputHandle.ptr();

  // input for decision
  std::vector<ITrigTRTHTHhypoTool::RNNOutputInfo> toolInput;


  // loop over previous decisions
  size_t counter=0;
  for ( const auto previousDecision: *previousDecisionsHandle ) {
   
    auto d = TCU::newDecisionIn( decisions, name() );
    //get RoI  
    auto roiELInfo = TCU::findLink<TrigRoiDescriptorCollection>( previousDecision, TCU::initialRoIString() );
    ATH_CHECK( roiELInfo.isValid() );
    d->setObjectLink( TCU::roiString(), roiELInfo.link );

    // get View
    const auto viewEL = previousDecision->objectLink<ViewContainer>( TCU::viewString() );
    ATH_CHECK( viewEL.isValid() );

    // get rnnOutput
    auto rnnOutputHandle = ViewHelper::makeHandle( *viewEL, m_trigRNNOutputKey, context);
    ATH_CHECK( rnnOutputHandle.isValid() );

    // link the rnnOutput      
    auto rnn = ViewHelper::makeLink( *viewEL, rnnOutputHandle, 0 );
    ATH_CHECK( rnn.isValid() );
    d->setObjectLink( "rnnOutput", rnn );

    // create new decision
    toolInput.emplace_back( d, rnnOutputHandle.cptr()->at(0), previousDecision );

    TCU::linkToPrevious( d, previousDecision, context );
    ATH_MSG_DEBUG( "Added view, roi, cluster, previous decision to new decision " << counter << " for view " << (*viewEL)->name()  );
    counter++;
  }
  
  ATH_MSG_DEBUG( "Found "<<toolInput.size()<<" inputs to tools");

   
  for ( auto& tool: m_hypoTools ) {
    ATH_CHECK( tool->decide( toolInput ) );
  }
 
  ATH_CHECK( hypoBaseOutputProcessing(outputHandle) );

  return StatusCode::SUCCESS;
}

