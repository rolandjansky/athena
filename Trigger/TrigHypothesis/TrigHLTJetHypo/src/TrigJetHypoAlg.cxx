/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include <algorithm>
#include "TrigJetHypoAlg.h"
#include "TrigCompositeUtils/HLTIdentifier.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"

using namespace TrigCompositeUtils;

using xAOD::JetContainer;

TrigJetHypoAlg::TrigJetHypoAlg( const std::string& name, 
				      ISvcLocator* pSvcLocator ) :
  ::HypoBase( name, pSvcLocator ) {}


StatusCode TrigJetHypoAlg::initialize() {

  // Jet special case: only one jet HypoTool is provided per chain, even if the chain has multiple jet legs.
  // Suppose HLT_e5_2e10_j10_j20_mu40. Only the leg002 jet HypoTool will be provided, and this will
  // compute the result for both the leg002 and leg003 here.
  for (auto& tool : m_hypoTools) {
    if (not tool.empty()) {
      ATH_CHECK(tool.retrieve());
    }
  }
  CHECK( m_jetsKey.initialize() );
  return StatusCode::SUCCESS;
}


StatusCode TrigJetHypoAlg::execute( const EventContext& context ) const {  
  ATH_MSG_DEBUG ( "Executing " << name() << "..." );


  // read in the previous Decisions made before running this hypo Alg.
  // The container should have only one such Decision (for L1) as deciding
  // on jets is a one step process.

  ATH_MSG_DEBUG("Retrieving L1 decision \"" << decisionInput().key() << "\"");
  auto h_prevDecisions = SG::makeHandle(decisionInput(), context );

  if( not h_prevDecisions.isValid() || h_prevDecisions->size() ==0) {//implicit
    ATH_MSG_DEBUG( "No implicit RH for previous decisions "<<  decisionInput().key()<<": is this expected?" );
    return StatusCode::SUCCESS;      
  }

  if(h_prevDecisions->size() != 1){
    ATH_MSG_ERROR(" Expected one previous decisions in " << decisionInput().key()
		  << " (L1 RoIs not used), found " << h_prevDecisions->size());
    return StatusCode::FAILURE;      
  }

  const DecisionContainer* prevDecisions = h_prevDecisions.get();
  const Decision* previousDecision = prevDecisions->at(0);
  
  // previousDecision contains all active jet chains (i.e. seeded by L1,
  // passed prescale)

  // Create container to write output Decision objects - there will be one per
  // jet
  SG::WriteHandle<DecisionContainer> outputHandle =
    createAndStore(decisionOutput(), context);

  DecisionContainer* outputDecisions = outputHandle.ptr();

  // read in a jets collection, and obtain a bare pointer to it

  auto h_jets = SG::makeHandle(m_jetsKey, context );
  ATH_MSG_DEBUG("Retrieving jets from: " << h_jets.key());
  ATH_CHECK(h_jets.isValid());
    
  const JetContainer* jets = h_jets.get();

  CHECK(decide(jets, previousDecision, outputDecisions, context)); 

  // Common debug printing and output checking
  ATH_CHECK(hypoBaseOutputProcessing(outputHandle));

  return StatusCode::SUCCESS;
}

StatusCode
TrigJetHypoAlg::decide(const xAOD::JetContainer* jets,
                         const Decision* previousDecision,
                         DecisionContainer* outputDecisions,
                         const EventContext& context) const{

  // Pair to associate each jet with its corresponding Decision object -
  // this will record the chains which the jet passes.

  std::vector<std::pair<const xAOD::Jet*,Decision*>> jetHypoInputs;

  if(m_doPresel) {
    // In the preselection case, we create only one DecisionObject, which links
    // all jets -- this is so as to avoid making spurious DecisionObject
    // chains where the objects in one step have no unambiguous relationship
    // with those in the preceding step.
    Decision* newDecision = nullptr;
    newDecision = TrigCompositeUtils::newDecisionIn(outputDecisions, previousDecision, TrigCompositeUtils::hypoAlgNodeName(), context);
    // Needs a dummy feature link -- we will specify the input RoI which triggers special behaviour in the ComboHypo, equivilant to the "noCombo" below
    if(!newDecision->hasObjectLink(TrigCompositeUtils::featureString())) {
      newDecision->setObjectLink<TrigRoiDescriptorCollection>(TrigCompositeUtils::featureString(), 
							      TrigCompositeUtils::findLink<TrigRoiDescriptorCollection>(newDecision, TrigCompositeUtils::initialRoIString()).link);
      }
    // We need to fill the jetHypoInputs vector, pairing each jet with
    // the same newDecision object, such that it is updated if the hypo
    // tool passes the jet.
    // In the opposite direction, we link the jets from the object for
    // retrieval subsequently.
    // *Linked jets are not implied to be responsible for a passing decision*
    //
    bool first=true;
    for (const xAOD::Jet* jet : *jets) {
      jetHypoInputs.push_back( std::make_pair(jet, newDecision) );
      if(first) {
        // We receive a VIEW container with filtered jets, so need to retrieve the original
        const xAOD::JetContainer* jetCont = static_cast<const xAOD::JetContainer*>(jet->container());
        ATH_MSG_VERBOSE("Creating element link to presel jet " << jet->index() << " of " << jetCont->size());
        newDecision->setObjectLink<xAOD::JetContainer>("LeadingPreselJet",ElementLink<xAOD::JetContainer>(*jetCont, jet->index()));
      }
    }

    ATH_MSG_DEBUG("Output decision object:");
    ATH_MSG_DEBUG( *newDecision );

  } else {
    // When operating as a terminal hypo selection, we create one DecisionObject
    // per jet, which is later used to identify which jets contributed to an
    // event passing.
    for (const xAOD::Jet* jet : *jets) {

      Decision* newDecision = nullptr;
      // Create a new Decision object to mirror this Jet.
      // Link it to its parent Decision object and attach the jet as a "feature"
      newDecision = TrigCompositeUtils::newDecisionIn(outputDecisions, previousDecision, TrigCompositeUtils::hypoAlgNodeName(), context);

      // We receive a VIEW container with filtered jets, so need to retrieve the original
      const xAOD::JetContainer* jetCont = static_cast<const xAOD::JetContainer*>(jet->container());
      ElementLink<xAOD::JetContainer> jetLink =
        ElementLink<xAOD::JetContainer>(*jetCont, jet->index());

      // Create a decoration. This is used to comminicate to the following ComboHypo that the
      // DecisionObject should be excluded from downstream multiplicity checks.
      newDecision->setDetail<int32_t>("noCombo", 1);

      newDecision->setObjectLink<xAOD::JetContainer>(TrigCompositeUtils::featureString(), jetLink);
      jetHypoInputs.push_back( std::make_pair(jet, newDecision) );
    }
  }

  // Extract the IDs of the jet chains which are active.
  // previousDecisionIDs is a std::set<uint32_t>.
  
  TrigCompositeUtils::DecisionIDContainer previousDecisionIDs{
    TrigCompositeUtils::decisionIDs(previousDecision).begin(),
      TrigCompositeUtils::decisionIDs(previousDecision).end()
      };

  if (msgLvl(MSG::DEBUG)) {
    msg() << "IDs of active legs:" << endmsg;
    for(auto decisionID: previousDecisionIDs) {
      msg() << "    " << decisionID << endmsg;
    }
  }
  // Some of these may be leg IDs, convert those to the chain ID
  TrigCompositeUtils::DecisionIDContainer leglessPreviousDecisionIDs;
  std::transform(previousDecisionIDs.begin(), previousDecisionIDs.end(),
		 std::inserter(leglessPreviousDecisionIDs,leglessPreviousDecisionIDs.begin()),
		 [] (TrigCompositeUtils::DecisionID id) -> TrigCompositeUtils::DecisionID
		 {return TrigCompositeUtils::getIDFromLeg(HLT::Identifier(id)).numeric();}
		 );
  if (msgLvl(MSG::DEBUG)) {
    msg() << "IDs of active chains:" << endmsg;
    for(auto decisionID: leglessPreviousDecisionIDs) {
      msg() << "    " << decisionID << endmsg;
    }
  }
  
  // If this is a jet-only chain use the chain IDs
  // Otherwise, we need to keep the leg
  // The two options are combined into a single list to make this simpler
  
  previousDecisionIDs.insert(leglessPreviousDecisionIDs.begin(), leglessPreviousDecisionIDs.end());
  
  for (const auto& tool: m_hypoTools) {
    if (tool.empty()) {
      continue;
    }
    ATH_MSG_DEBUG("Now computing decision for " << tool->name());
    CHECK(tool->decide(jets, previousDecisionIDs, jetHypoInputs));
  }
  
  return StatusCode::SUCCESS;
}

