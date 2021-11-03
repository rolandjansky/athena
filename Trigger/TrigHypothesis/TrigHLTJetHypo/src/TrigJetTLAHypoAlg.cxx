/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
   */

#include <algorithm>
#include "TrigJetTLAHypoAlg.h"
#include "TrigCompositeUtils/HLTIdentifier.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "xAODJet/JetContainer.h"
#include "xAODJet/JetAuxContainer.h"


using namespace TrigCompositeUtils;

using xAOD::JetContainer;

TrigJetTLAHypoAlg::TrigJetTLAHypoAlg( const std::string& name, 
    ISvcLocator* pSvcLocator ) :
  ::HypoBase( name, pSvcLocator ) {

  }


StatusCode TrigJetTLAHypoAlg::initialize() {
  CHECK( m_hypoTools.retrieve() );

  CHECK( m_TLAjetsKey.initialize() );
  return StatusCode::SUCCESS;
}


StatusCode TrigJetTLAHypoAlg::execute( const EventContext& context ) const {

  ATH_MSG_DEBUG ( "Executing " << name() << "..." );

  // create handles for TLA Jets
  SG::WriteHandle<JetContainer> h_TLAJets = SG::makeHandle(m_TLAjetsKey, context);
  //make the output jet container
  ATH_CHECK(h_TLAJets.record(std::make_unique<xAOD::JetContainer>(),
                              std::make_unique<xAOD::JetAuxContainer>()));


  // retrieving previous decisions
  auto previousDecisionHandle = SG::makeHandle(decisionInput(), context);
  ATH_CHECK(previousDecisionHandle.isValid());
  ATH_MSG_DEBUG("Running with " << previousDecisionHandle->size() << " previous decisions");
 

    //container for output decisions
  SG::WriteHandle<DecisionContainer> outputHandle = createAndStore(decisionOutput(), context);
  DecisionContainer* outputDecisions = outputHandle.ptr();

  //information to pass to hypoTool: jet pointer and decision
  std::vector<std::pair<const xAOD::Jet*,Decision*>> jetHypoInputs;

  /// NEW

  int nDecision = 0;
    for (const auto previousDecision : *previousDecisionHandle)
    {
      // get jets from the decision
        const xAOD::Jet *jetPrev = nullptr;
        auto prevJets = TrigCompositeUtils::findLinks<xAOD::JetContainer>(previousDecision, TrigCompositeUtils::featureString(), TrigDefs::lastFeatureOfType);
        ATH_MSG_WARNING("This decision has " << prevJets.size() << " jets");

         //copy all jets into the new TLA collection
        int nSavedJets = 0; 
        for (auto jet : prevJets)
        {
            auto prevJetLink = jet.link;
            ATH_CHECK(prevJetLink.isValid());
            jetPrev = *prevJetLink;
            
            if (jetPrev->pt() < m_jetPtThreshold) continue;
            
            

            xAOD::Jet *copiedJet = new xAOD::Jet();            
            h_TLAJets->push_back(copiedJet);
            *copiedJet = *jetPrev;
            nSavedJets++;

            ATH_MSG_WARNING("Copied jet with pT: " << copiedJet->pt() << " from decision " << nDecision);

            // now create new decision object
            // now go on with the normal Hypo, linking new decision with previous one
            auto newDecision = newDecisionIn( outputDecisions, hypoAlgNodeName() );
            TrigCompositeUtils::linkToPrevious( newDecision, previousDecision, context );
            // do we need to re-link the feature?
            newDecision->setObjectLink<xAOD::JetContainer>(featureString(), prevJetLink);

            jetHypoInputs.push_back( std::make_pair(copiedJet, newDecision) );


        }
        ATH_MSG_WARNING("Copied jets: " << nSavedJets << "/" << prevJets.size() << " with pT threshold " << m_jetPtThreshold/1000. << " GeV");


      nDecision++;
    }







  // END OF NEW ///

  // get the HLT decision container for the previous decisions (one per jet in the original collection)

  // ATH_MSG_DEBUG("Retrieving HLT decision \"" << decisionInput().key() << "\"");
  // auto previousDecisionsHandle = SG::makeHandle(decisionInput(), context );
  // ATH_CHECK(previousDecisionsHandle.isValid());
  // const DecisionContainer* prevDecisions = previousDecisionsHandle.get();

  // // read in the TLA jets collection, and obtain a bare pointer to it

  // ATH_MSG_DEBUG("Retrieving jets from the TLA container  \"" << m_TLAjetsKey << "\""); 
  // auto TLAJetsHandle = SG::makeHandle(m_TLAjetsKey, context );
  // ATH_CHECK(TLAJetsHandle.isValid());
  // const JetContainer* TLAjets = TLAJetsHandle.get();


  // //this is to check that we don't have TLA jet containers without any previous associated decision
  // bool atLeastOneDecision = false;

  // //container for output decisions
  // SG::WriteHandle<DecisionContainer> outputHandle = createAndStore(decisionOutput(), context);
  // DecisionContainer* outputDecisions = outputHandle.ptr();

  // //information to pass to hypoTool: jet pointer and decision
  // std::vector<std::pair<const xAOD::Jet*,Decision*>> jetHypoInputs;

  // //since the two aren't necessarily index-parallel, we need to match the previous decisions to the new jets
  // //loop on the TLA container jets
  // for (const xAOD::Jet* TLAjet : *TLAjets) {

  //   bool associatedWithPreviousStep = false;
  //   const Decision* previousDecision = nullptr;
  //   for (const Decision* testDecision : *prevDecisions) {

  //     //deltaR-match the jet attached from the previous decision 

  //     TrigCompositeUtils::LinkInfo< xAOD::JetContainer > myFeature = TrigCompositeUtils::findLink< xAOD::JetContainer >( testDecision, TrigCompositeUtils::featureString());
  //     ATH_CHECK( myFeature.isValid() );
  //     const xAOD::Jet* testJet = *(myFeature.link);

  //     //check the deltaR between this jet and the TLA jet we're testing
  //     //use a tight (?) deltaR = 0.1 matching as they should be the same jet 
  //     if ( testJet->p4().DeltaR(TLAjet->p4()) < 0.1 ) {
  //       //if they match, set the bools to True 
  //       previousDecision = testDecision;
  //       associatedWithPreviousStep = true;
  //       atLeastOneDecision = true;

  //       //prepare the necessary information to the HypoTool:
  //       // - the new decision
  //       Decision* newDecision = TrigCompositeUtils::newDecisionIn(outputDecisions, previousDecision, hypoAlgNodeName(), context);
  //       // - the ElementLink to the TLA-jet
  //       ElementLink<xAOD::JetContainer> jetLink = ElementLink<xAOD::JetContainer>(*TLAjets, TLAjet->index());
  //       ATH_CHECK( jetLink.isValid() );   
  //       // - associate the two
  //       newDecision->setObjectLink<xAOD::JetContainer>(featureString(), jetLink);
  //       // - put in the vector that will eventually be handed off to the HypoTool
  //       jetHypoInputs.push_back( std::make_pair(TLAjet, newDecision) );

  //     } // end check on whether this TLA-jet corresponds to this decision-jet

  //     //if the jet has found a decision, move onto the next jet
  //     if (associatedWithPreviousStep) break;

  //   }//end loop on decisions

  // }//end loop on TLA jets

  // check that this jet collection has a previous decision (at least one has to have it, otherwise trigger navigation doesn't work)
  // if (!atLeastOneDecision) {

  //   ATH_MSG_ERROR("Unable to associate a previous decision to any jet in the vector of TLA jets.\
  //   The chain seeding the TLA selector should have at least one jet contributing to the decision that fulfills the requirements of the TLA jets saved in the stream.\
  //   If this is not the case, please contact the developers.");
  //   return StatusCode::FAILURE;

  // }//end if on at least one decision in this jet collection

  for (const auto& tool: m_hypoTools) {

    ATH_MSG_DEBUG("Now computing decision for " << tool->name());
    CHECK(tool->decide(jetHypoInputs));

  }//end loop on hypoTools

  //extra run-time checks
  ATH_CHECK( hypoBaseOutputProcessing(outputHandle) );
  
  return StatusCode::SUCCESS;

}//end execute
