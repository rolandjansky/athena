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
        


      nDecision++;
    }



  for (const auto& tool: m_hypoTools) {

    ATH_MSG_DEBUG("Now computing decision for " << tool->name());
    CHECK(tool->decide(jetHypoInputs));

  }//end loop on hypoTools

  //extra run-time checks
  ATH_CHECK( hypoBaseOutputProcessing(outputHandle) );
  
  return StatusCode::SUCCESS;

}//end execute
