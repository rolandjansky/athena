/*
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
   */

#include <algorithm>
#include "Gaudi/Property.h"
#include "TrigJetTLAHypoAlgMT.h"
#include "TrigCompositeUtils/HLTIdentifier.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"


using namespace TrigCompositeUtils;

using xAOD::JetContainer;

TrigJetTLAHypoAlgMT::TrigJetTLAHypoAlgMT( const std::string& name, 
    ISvcLocator* pSvcLocator ) :
  ::HypoBase( name, pSvcLocator ) {

  }


StatusCode TrigJetTLAHypoAlgMT::initialize() {
  CHECK( m_hypoTools.retrieve() );

  CHECK( m_TLAjetsKey.initialize() );
  return StatusCode::SUCCESS;
}


StatusCode TrigJetTLAHypoAlgMT::execute( const EventContext& context ) const {

  ATH_MSG_DEBUG ( "Executing " << name() << "..." );

  // get the HLT decision container for the previous decisions (one per jet in the original collection)

  ATH_MSG_DEBUG("Retrieving HLT decision \"" << decisionInput().key() << "\"");
  auto previousDecisionsHandle = SG::makeHandle(decisionInput(), context );
  ATH_CHECK(previousDecisionsHandle.isValid());
  const DecisionContainer* prevDecisions = previousDecisionsHandle.get();

  // read in the TLA jets collection, and obtain a bare pointer to it

  ATH_MSG_DEBUG("Retrieving jets from the TLA container  \"" << m_TLAjetsKey << "\""); 
  auto TLAJetsHandle = SG::makeHandle(m_TLAjetsKey, context );
  ATH_CHECK(TLAJetsHandle.isValid());
  const JetContainer* TLAjets = TLAJetsHandle.get();


  //this is to check that we don't have TLA jet containers without any previous associated decision
  bool atLeastOneDecision = false;

  //container for output decisions
  SG::WriteHandle<DecisionContainer> outputHandle = createAndStore(decisionOutput(), context);
  DecisionContainer* outputDecisions = outputHandle.ptr();

  //information to pass to hypoTool: jet pointer and decision
  std::vector<std::pair<const xAOD::Jet*,Decision*>> jetHypoInputs;

  //since the two aren't necessarily index-parallel, we need to match the previous decisions to the new jets
  //loop on the TLA container jets
  for (const xAOD::Jet* TLAjet : *TLAjets) {

    bool associatedWithPreviousStep = false;
    const Decision* previousDecision = nullptr;
    for (const Decision* testDecision : *prevDecisions) {

      //deltaR-match the jet attached from the previous decision 

      TrigCompositeUtils::LinkInfo< xAOD::JetContainer > myFeature = TrigCompositeUtils::findLink< xAOD::JetContainer >( testDecision, TrigCompositeUtils::featureString());
      ATH_CHECK( myFeature.isValid() );
      const xAOD::Jet* testJet = *(myFeature.link);

      //check the deltaR between this jet and the TLA jet we're testing
      //use a tight (?) deltaR = 0.1 matching as they should be the same jet 
      if ( testJet->p4().DeltaR(TLAjet->p4()) < 0.1 ) {
        //if they match, set the bools to True 
        previousDecision = testDecision;
        associatedWithPreviousStep = true;
        atLeastOneDecision = true;

        //prepare the necessary information to the HypoTool:
        // - the new decision
        Decision* newDecision = TrigCompositeUtils::newDecisionIn(outputDecisions, previousDecision, "", context);
        // - the ElementLink to the TLA-jet
        ElementLink<xAOD::JetContainer> jetLink = ElementLink<xAOD::JetContainer>(*TLAjets, TLAjet->index());
        ATH_CHECK( jetLink.isValid() );   
        // - associate the two
        newDecision->setObjectLink<xAOD::JetContainer>(featureString(), jetLink);
        // - put in the vector that will eventually be handed off to the HypoTool
        jetHypoInputs.push_back( std::make_pair(TLAjet, newDecision) );

      } // end check on whether this TLA-jet corresponds to this decision-jet

      //if the jet has found a decision, move onto the next jet
      if (associatedWithPreviousStep) break;

    }//end loop on decisions

  }//end loop on TLA jets

  // check that this jet collection has a previous decision (at least one has to have it, otherwise trigger navigation doesn't work)
  if (!atLeastOneDecision) {

    ATH_MSG_ERROR("Unable to associate a previous decision to any jet in the vector of TLA jets.\
    The chain seeding the TLA selector should have at least one jet contributing to the decision that fulfills the requirements of the TLA jets saved in the stream.\
    If this is not the case, please contact the developers.");
    return StatusCode::FAILURE;

  }//end if on at least one decision in this jet collection

  for (const auto& tool: m_hypoTools) {

    ATH_MSG_DEBUG("Now computing decision for " << tool->name());
    CHECK(tool->decide(jetHypoInputs));

  }//end loop on hypoTools

  //extra run-time checks
  ATH_CHECK( hypoBaseOutputProcessing(outputHandle) );
  
  return StatusCode::SUCCESS;

}//end execute
