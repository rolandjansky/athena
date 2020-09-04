/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include <algorithm>
#include "Gaudi/Property.h"
#include "TrigJetHypoAlgMT.h"
#include "TrigCompositeUtils/HLTIdentifier.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"


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
  // The container should have only one such Decision (for L1) as deding
  // on jets is a one step process.

  ATH_MSG_DEBUG("Retrieving L1 decision \"" << decisionInput().key() << "\"");
  auto h_prevDecisions = SG::makeHandle(decisionInput(), context );

  if( not h_prevDecisions.isValid() || h_prevDecisions->size() ==0) {//implicit
    ATH_MSG_DEBUG( "No implicit RH for previous decisions "<<  decisionInput().key()<<": is this expected?" );
    return StatusCode::SUCCESS;      
  }

  if(h_prevDecisions->size() != 1){
    ATH_MSG_ERROR(" Expected one previous decisions (L1 RoIs not used), found "
                  << h_prevDecisions->size());
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

  CHECK(decide(jets, previousDecision, outputDecisions)); 

  // Common debug printing and output checking
  ATH_CHECK(hypoBaseOutputProcessing(outputHandle));

  return StatusCode::SUCCESS;
}

StatusCode
TrigJetHypoAlgMT::decide(const xAOD::JetContainer* jets,
                         const Decision* previousDecision,
                         DecisionContainer* outputDecisions) const{

  // Pair to associate each jet with its corresponding Decision object -
  // this will record the chains which the jet passes.

  std::vector<std::pair<const xAOD::Jet*,Decision*>> jetHypoInputs;

  for (const xAOD::Jet* jet : *jets) {

    // Create a new Decision object to mirror this Jet.
    // Link it to its parent Decision object and attach the jet as a "feature"

    Decision* newDecision =
      TrigCompositeUtils::newDecisionIn(outputDecisions, previousDecision);
    
    ElementLink<xAOD::JetContainer> jetLink =
      ElementLink<xAOD::JetContainer>(*jets, jet->index());

    newDecision->setObjectLink<xAOD::JetContainer>(featureString(), jetLink);
    jetHypoInputs.push_back( std::make_pair(jet, newDecision) );
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

