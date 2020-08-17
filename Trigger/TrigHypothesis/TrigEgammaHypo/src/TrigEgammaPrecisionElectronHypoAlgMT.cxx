/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "Gaudi/Property.h"
#include "TrigEgammaPrecisionElectronHypoAlgMT.h"
#include "TrigCompositeUtils/HLTIdentifier.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "AthViews/ViewHelper.h"

using namespace TrigCompositeUtils;

TrigEgammaPrecisionElectronHypoAlgMT::TrigEgammaPrecisionElectronHypoAlgMT( const std::string& name, 
					  ISvcLocator* pSvcLocator ) :
  ::HypoBase( name, pSvcLocator ) {}

TrigEgammaPrecisionElectronHypoAlgMT::~TrigEgammaPrecisionElectronHypoAlgMT() {}

StatusCode TrigEgammaPrecisionElectronHypoAlgMT::initialize() {
  ATH_MSG_DEBUG ( "Initializing " << name() << "..." );

  
  ATH_CHECK( m_hypoTools.retrieve() );
  
  ATH_CHECK( m_electronsKey.initialize() );
  renounce( m_electronsKey );// electrons are made in views, so they are not in the EvtStore: hide them

  return StatusCode::SUCCESS;
}

StatusCode TrigEgammaPrecisionElectronHypoAlgMT::finalize() {   
  return StatusCode::SUCCESS;
}


StatusCode TrigEgammaPrecisionElectronHypoAlgMT::execute( const EventContext& context ) const {  
  ATH_MSG_DEBUG ( "Executing " << name() << "..." );
  auto previousDecisionsHandle = SG::makeHandle( decisionInput(), context );
  ATH_CHECK( previousDecisionsHandle.isValid() );
  ATH_MSG_DEBUG( "Running with "<< previousDecisionsHandle->size() <<" previous decisions");


  // new decisions

  // new output decisions
  SG::WriteHandle<DecisionContainer> outputHandle = createAndStore(decisionOutput(), context ); 
  auto decisions = outputHandle.ptr();

  // input for decision
  std::vector<ITrigEgammaPrecisionElectronHypoTool::ElectronInfo> toolInput;

  // loop over previous decisions
  size_t counter=0;
  for ( auto previousDecision: *previousDecisionsHandle ) {
    //get RoI  
    auto roiELInfo = findLink<TrigRoiDescriptorCollection>( previousDecision, initialRoIString() );
    
    ATH_CHECK( roiELInfo.isValid() );
    const TrigRoiDescriptor* roi = *(roiELInfo.link);
    const auto viewEL = previousDecision->objectLink<ViewContainer>( viewString() );
    ATH_CHECK( viewEL.isValid() );
    auto electronHandle = ViewHelper::makeHandle( *viewEL, m_electronsKey, context);
    ATH_CHECK( electronHandle.isValid() );
    ATH_MSG_DEBUG ( "Electron handle size: " << electronHandle->size() << "..." );
    // Loop over the electronHandles
    size_t validelectrons=0;
    for (size_t cl=0; cl< electronHandle->size(); cl++){
	{
	    auto ph = ViewHelper::makeLink( *viewEL, electronHandle, cl );
	    ATH_MSG_DEBUG ( "Checking ph.isValid()...");
	    if( !ph.isValid() ) {
		ATH_MSG_DEBUG ( "ElectronHandle in position " << cl << " -> invalid ElemntLink!. Skipping...");
	    }
	    ATH_CHECK(ph.isValid());

	    ATH_MSG_DEBUG ( "ElectronHandle in position " << cl << " processing...");
	    auto d = newDecisionIn( decisions, name() );
	    d->setObjectLink( "feature",  ph );
	    TrigCompositeUtils::linkToPrevious( d, decisionInput().key(), counter );
	    d->setObjectLink( roiString(), roiELInfo.link );
	    toolInput.emplace_back( d, roi, electronHandle.cptr()->at(cl), previousDecision );
	    validelectrons++;


	}
    }
    ATH_MSG_DEBUG( "Electrons with valid links: " << validelectrons );
    
    ATH_MSG_DEBUG( "roi, electron, previous decision to new decision " << counter << " for roi " );
    counter++;

  }

  ATH_MSG_DEBUG( "Found "<<toolInput.size()<<" inputs to tools");

   
  for ( auto& tool: m_hypoTools ) {
    ATH_CHECK( tool->decide( toolInput, context ) );
  }
 
  ATH_CHECK( hypoBaseOutputProcessing(outputHandle) );
  return StatusCode::SUCCESS;
}
