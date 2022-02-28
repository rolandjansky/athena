/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
   */

#include <algorithm>
#include "TrigJetTLAHypoAlg.h"
#include "TrigCompositeUtils/HLTIdentifier.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "xAODJet/JetContainer.h"
#include "xAODJet/JetAuxContainer.h"
#include "xAODBase/IParticleHelpers.h" //for getOriginalObjectLink


using namespace TrigCompositeUtils;

using xAOD::JetContainer;

TrigJetTLAHypoAlg::TrigJetTLAHypoAlg( const std::string& name, 
    ISvcLocator* pSvcLocator ) :
  ::HypoBase( name, pSvcLocator ) {

  }


StatusCode TrigJetTLAHypoAlg::initialize() {
  CHECK( m_hypoTools.retrieve() );
  CHECK( m_TLAjetsKey.initialize() );
  if (m_attach_btag) CHECK(m_btag_record_tool.retrieve());
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

  int nDecision = 0;
  int nSavedJets = 0; 
  for (const auto previousDecision : *previousDecisionHandle)
  {
    // get jets from the decision
      const xAOD::Jet *jetPrev = nullptr;
      auto prevJets = TrigCompositeUtils::findLinks<xAOD::JetContainer>(previousDecision, TrigCompositeUtils::featureString(), TrigDefs::lastFeatureOfType);
      ATH_MSG_DEBUG("This decision has " << prevJets.size() << " jets");

       //copy all jets into the new TLA collection
      for (auto jet : prevJets)
      {
          auto prevJetLink = jet.link;
          ATH_CHECK(prevJetLink.isValid());
          jetPrev = *prevJetLink;

          // create new decision object for all previous decisions
          // linking new decision with previous one
          auto newDecision = newDecisionIn( outputDecisions, hypoAlgNodeName() );
          TrigCompositeUtils::linkToPrevious( newDecision, previousDecision, context );

          // Now we ensure not saving jet copies but still linking a TLA jet to each decision
          ElementLink<xAOD::IParticleContainer>::ElementConstPointer original_jet_link = getOriginalObjectLink(*(jetPrev));
          bool duplicate_found(false);
          for (unsigned idx_tla_jet = 0; idx_tla_jet!=h_TLAJets->size(); idx_tla_jet++){
              const xAOD::Jet* tla_jet = h_TLAJets->at(idx_tla_jet);
              if (*original_jet_link == *getOriginalObjectLink(*tla_jet)) {
                ATH_MSG_DEBUG("Jet already found! Adding to decision but not to TLA jet container...");
                newDecision->setObjectLink(featureString(), ElementLink<xAOD::JetContainer>(*h_TLAJets, idx_tla_jet, context)); 
                duplicate_found = true;
              } 
          }
          if (duplicate_found) continue;
          
          xAOD::Jet *copiedJet = new xAOD::Jet();            
          h_TLAJets->push_back(copiedJet);
          *copiedJet = *jetPrev;
          nSavedJets++;

          ATH_MSG_DEBUG("Copied jet with pT: " << copiedJet->pt() << " and eta "<<copiedJet->eta()<<" from decision " << nDecision);

          // link TLA jets to decision
          newDecision->setObjectLink(featureString(), ElementLink<xAOD::JetContainer>(*h_TLAJets, h_TLAJets->size() - 1, context)); 
          

      }
    nDecision++;
  }

  ATH_MSG_DEBUG("Saved "<<nSavedJets<<" TLA jets from "<<nDecision<<" input decisions.");
  
  if(m_attach_btag){
      ATH_MSG_DEBUG("Attaching any available b-tag information to TLA  jets.");
      const DecisionContainer* previousDecisions = previousDecisionHandle.get();
      // ** Calling record_btag() on this tool will create a new TLA HLT Btagging container 
      //    which will be filled with copies of BTagging elements attached to the previous decision
      //    and linked to matching jets in h_TLAJets. **
      CHECK(m_btag_record_tool->record_btag(previousDecisions, *h_TLAJets, context));
  }

  for (const auto& tool: m_hypoTools) {

    ATH_MSG_DEBUG("Now computing decision for " << tool->name());
    CHECK(tool->decide(outputDecisions));

  }//end loop on hypoTools

  //extra run-time checks
  ATH_CHECK( hypoBaseOutputProcessing(outputHandle) );
  
  return StatusCode::SUCCESS;

}//end execute
