/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TrigUpgradeTest includes
#include "TestHypoAlg.h"

namespace HLTTest {

  TestHypoAlg::TestHypoAlg( const std::string& name, ISvcLocator* pSvcLocator ) 
  : ::HypoBase( name, pSvcLocator ) {}

  TestHypoAlg::~TestHypoAlg() {}
  
  StatusCode TestHypoAlg::initialize() {
    ATH_MSG_INFO ("Initializing " << name() << "...");
    ATH_MSG_DEBUG("Link name is "<<m_linkName.value());
    if ( not m_recoInput.key().empty() )
      CHECK( m_recoInput.initialize() );
    CHECK( m_tools.retrieve() );  
    return StatusCode::SUCCESS;
  }

  StatusCode TestHypoAlg::finalize() {
    ATH_MSG_INFO( "Finalizing " << name() << "..." );
    return StatusCode::SUCCESS;
  }


  StatusCode TestHypoAlg::execute( const EventContext& context ) const {  
    ATH_MSG_DEBUG( "Executing " << name() << "..." );
    if ( m_recoInput.key().empty() ) {
      ATH_MSG_DEBUG( "No input configured, not producing the output" );
      return StatusCode::SUCCESS;      
    }

    auto previousDecisionsHandle = SG::makeHandle( decisionInput(), context );
    if( not previousDecisionsHandle.isValid() ) {//implicit
      ATH_MSG_DEBUG( "No implicit RH for previous decisions "<<  decisionInput().key()<<": is this expected?" );
      return StatusCode::SUCCESS;      
    }
    
    ATH_MSG_DEBUG( "Running with "<< previousDecisionsHandle->size() <<" implicit ReadHandles for previous decisions");

    auto recoInput = SG::makeHandle(m_recoInput, context);
    ATH_MSG_DEBUG( "and with "<< recoInput->size() <<" reco inputs");
    
    // new output decisions
    SG::WriteHandle<DecisionContainer> outputHandle = createAndStore(decisionOutput(), context ); 
    auto decisions = outputHandle.ptr();

    // find features:
    std::vector<const FeatureOBJ*> featureFromDecision;
    for ( const auto previousDecision: *previousDecisionsHandle ) {
      const auto linkInfo = TrigCompositeUtils::findLink<FeatureContainer>(previousDecision, m_linkName.value());
      const auto featureLink = linkInfo.link;
      CHECK( featureLink.isValid() );
      const FeatureOBJ* feature = *featureLink;
      featureFromDecision.push_back( feature);
    }       
    ATH_MSG_DEBUG("Found "<<featureFromDecision.size()<<" features "<<m_linkName.value() <<" mapped from previous decisions");
    
    //map reco object and decision: find in reco obejct the initial RoI and map it to the correct decision
    size_t reco_counter = 0;
    for (const auto recoobj: *recoInput){
      const auto roiInfo = TrigCompositeUtils::findLink<TrigRoiDescriptorCollection>( recoobj, "initialRoI"  );
      const auto roiEL = roiInfo.link;
      CHECK( roiEL.isValid() );
      
      const auto featureInfo = TrigCompositeUtils::findLink<FeatureContainer>( recoobj, m_linkName.value()  );
      const auto featurelink = featureInfo.link;
      CHECK( featurelink.isValid() );
      if ( not featurelink.isValid() )  {
        ATH_MSG_ERROR( " Can not find reference to " + m_linkName.value() + " from the decision" );
        return StatusCode::FAILURE;
      }
      
      ATH_MSG_DEBUG(" Found link from the reco object to feature "<<m_linkName.value() );
      const FeatureOBJ* feature = *featurelink;
      // find the same roi in the previous decisions
      bool foundFeatureInDecision=false;
      size_t pos=distance(featureFromDecision.begin(), find(featureFromDecision.begin(), featureFromDecision.end(), feature));
      if (pos < featureFromDecision.size()){
        foundFeatureInDecision=true;  
      }

      if (foundFeatureInDecision){
        ATH_MSG_DEBUG(" Found link from the reco object to the previous decision at position "<<pos);
        auto d = newDecisionIn(decisions);
        d->setObjectLink( "feature", ElementLink<xAOD::TrigCompositeContainer>(m_recoInput.key(), reco_counter) );// feature used by the Tool
        d->setObjectLink( m_linkName.value(), featurelink );
        linkToPrevious( d, decisionInput().key(), pos );
      }
      else{
        ATH_MSG_ERROR( " Can not find reference to previous decision from feature " + m_linkName.value() + " from reco object " << reco_counter );
        return StatusCode::FAILURE;
      }
      reco_counter++;
    }

    if (decisions->size()>0){
      for ( auto tool: m_tools ) {
        CHECK( tool->decide( decisions ) );
      }
    }

    ATH_MSG_DEBUG( "Exiting with "<< outputHandle->size() <<" decisions");

    //debug
    for (auto outh: *outputHandle){
      TrigCompositeUtils::DecisionIDContainer objDecisions;      
      TrigCompositeUtils::decisionIDs( outh, objDecisions );

      ATH_MSG_DEBUG("Number of positive decisions for this input: " << objDecisions.size() );

      for ( TrigCompositeUtils::DecisionID id : objDecisions ) {
        ATH_MSG_DEBUG( " --- found new decision " << HLT::Identifier( id ) );
      }  
    }
  
    return StatusCode::SUCCESS;
  }

} //> end namespace HLTTest
