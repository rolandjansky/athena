/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include <algorithm>
#include "TrigJetHypoAlgMT.h"
#include "TrigCompositeUtils/HLTIdentifier.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"

using namespace TrigCompositeUtils;

using xAOD::JetContainer;

TrigJetHypoAlgMT::TrigJetHypoAlgMT( const std::string& name, 
				      ISvcLocator* pSvcLocator ) :
  ::HypoBase( name, pSvcLocator ) {}


StatusCode TrigJetHypoAlgMT::initialize() {

  CHECK( m_hypoTools.retrieve() );
  CHECK( m_jetsKey.initialize() );
  return StatusCode::SUCCESS;
}


StatusCode TrigJetHypoAlgMT::execute( const EventContext& context ) const {  
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
TrigJetHypoAlgMT::decide(const xAOD::JetContainer* jets,
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
    // Needs a dummy feature link -- we will specify the input RoI
    if(!newDecision->hasObjectLink(TrigCompositeUtils::featureString())) {
      newDecision->setObjectLink<TrigRoiDescriptorCollection>(TrigCompositeUtils::featureString(), 
							      TrigCompositeUtils::findLink<TrigRoiDescriptorCollection>(newDecision, TrigCompositeUtils::initialRoIString()).link);
      }
    // We need to fill the jetHypoInputs vector, pairing each jet with
    // the same newDecision object, such that it is updated if the hypo
    // tool passes the jet.
    for (const xAOD::Jet* jet : *jets) {
      jetHypoInputs.push_back( std::make_pair(jet, newDecision) );
    }
  } else {
    // When operating as a terminal hypo selection, we create one DecisionObject
    // per jet, which is later used to identify which jets contributed to an
    // event passing.
    for (const xAOD::Jet* jet : *jets) {

      Decision* newDecision = nullptr;
      // Create a new Decision object to mirror this Jet.
      // Link it to its parent Decision object and attach the jet as a "feature"
      newDecision = TrigCompositeUtils::newDecisionIn(outputDecisions, previousDecision, TrigCompositeUtils::hypoAlgNodeName(), context);

      const xAOD::JetContainer* jetCont = static_cast<const xAOD::JetContainer*>(jet->container());
      ElementLink<xAOD::JetContainer> jetLink =
	ElementLink<xAOD::JetContainer>(*jetCont, jet->index());

      newDecision->setObjectLink<xAOD::JetContainer>(TrigCompositeUtils::featureString(), jetLink);
      jetHypoInputs.push_back( std::make_pair(jet, newDecision) );
    }
  }

  // Extract the IDs of the jet chains which are active.
  // previousDecisionIDs is a std::set<uint32_t>.
  
  const TrigCompositeUtils::DecisionIDContainer previousDecisionIDs{
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
  
  for (const auto& tool: m_hypoTools) {
    ATH_MSG_DEBUG("Now computing decision for " << tool->name());
    CHECK(tool->decide(jets, leglessPreviousDecisionIDs, jetHypoInputs));
  }
  
  return StatusCode::SUCCESS;
}

