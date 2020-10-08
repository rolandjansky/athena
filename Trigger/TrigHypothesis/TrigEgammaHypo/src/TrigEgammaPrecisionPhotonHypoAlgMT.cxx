/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "Gaudi/Property.h"
#include "TrigEgammaPrecisionPhotonHypoAlgMT.h"
#include "TrigCompositeUtils/HLTIdentifier.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "AthViews/ViewHelper.h"

using namespace TrigCompositeUtils;

TrigEgammaPrecisionPhotonHypoAlgMT::TrigEgammaPrecisionPhotonHypoAlgMT( const std::string& name, 
					  ISvcLocator* pSvcLocator ) :
  ::HypoBase( name, pSvcLocator ) {}

TrigEgammaPrecisionPhotonHypoAlgMT::~TrigEgammaPrecisionPhotonHypoAlgMT() {}

StatusCode TrigEgammaPrecisionPhotonHypoAlgMT::initialize() {
  ATH_MSG_DEBUG ( "Initializing " << name() << "..." );

  
  ATH_CHECK( m_hypoTools.retrieve() );
  
  ATH_CHECK( m_photonsKey.initialize() );
  renounce( m_photonsKey );// photons are made in views, so they are not in the EvtStore: hide them

  return StatusCode::SUCCESS;
}

StatusCode TrigEgammaPrecisionPhotonHypoAlgMT::finalize() {   
  return StatusCode::SUCCESS;
}


StatusCode TrigEgammaPrecisionPhotonHypoAlgMT::execute( const EventContext& context ) const {  
  ATH_MSG_DEBUG ( "Executing " << name() << "..." );
  auto previousDecisionsHandle = SG::makeHandle( decisionInput(), context );
  ATH_CHECK( previousDecisionsHandle.isValid() );  
  ATH_MSG_DEBUG( "Running with "<< previousDecisionsHandle->size() <<" previous decisions");


  // new decisions

  // new output decisions
  SG::WriteHandle<DecisionContainer> outputHandle = createAndStore(decisionOutput(), context ); 
  auto decisions = outputHandle.ptr();

  // input for decision
  std::vector<ITrigEgammaPrecisionPhotonHypoTool::PhotonInfo> toolInput;

  // loop over previous decisions
  size_t counter=0;
  for ( auto previousDecision: *previousDecisionsHandle ) {
    //get RoI  
    auto roiELInfo = findLink<TrigRoiDescriptorCollection>( previousDecision, initialRoIString() );
    
    ATH_CHECK( roiELInfo.isValid() );
    const TrigRoiDescriptor* roi = *(roiELInfo.link);

    // not using View so commenting out following lines
    const auto viewEL = previousDecision->objectLink<ViewContainer>( viewString() );
    ATH_CHECK( viewEL.isValid() );
    auto photonHandle = ViewHelper::makeHandle( *viewEL, m_photonsKey, context);
    ATH_CHECK( photonHandle.isValid() );
    ATH_MSG_DEBUG ( "Photon handle size: " << photonHandle->size() << "..." );
    // Loop over the photonHandles
    size_t validphotons=0;
    for (size_t cl=0; cl< photonHandle->size(); cl++){
	{
	    auto ph = ViewHelper::makeLink( *viewEL, photonHandle, cl );
	    ATH_MSG_DEBUG ( "Checking ph.isValid()...");
	    if( !ph.isValid() ) {
		ATH_MSG_DEBUG ( "PhotonHandle in position " << cl << " -> invalid ElemntLink!. Skipping...");
	    }
	    ATH_CHECK(ph.isValid());

	    ATH_MSG_DEBUG ( "PhotonHandle in position " << cl << " processing...");
	    auto d = newDecisionIn( decisions, name() );
	    d->setObjectLink( "feature",  ph );
	    TrigCompositeUtils::linkToPrevious( d, decisionInput().key(), counter );
	    d->setObjectLink( roiString(), roiELInfo.link );
	    toolInput.emplace_back( d, roi, photonHandle.cptr()->at(cl), previousDecision );
	    validphotons++;


	}
    }
    ATH_MSG_DEBUG( "Photons with valid links: " << validphotons );
    
    ATH_MSG_DEBUG( "roi, photon, previous decision to new decision " << counter << " for roi " );
    counter++;

  }

  ATH_MSG_DEBUG( "Found "<<toolInput.size()<<" inputs to tools");

   
  for ( auto& tool: m_hypoTools ) {
    ATH_CHECK( tool->decide( toolInput ) );
  }
 
  ATH_CHECK( hypoBaseOutputProcessing(outputHandle) );
  return StatusCode::SUCCESS;
}
