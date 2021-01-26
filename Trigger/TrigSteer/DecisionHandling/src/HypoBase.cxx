/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "DecisionHandling/HypoBase.h"
#include "TrigCompositeUtils/HLTIdentifier.h"

using namespace TrigCompositeUtils;

HypoBase::HypoBase( const std::string& name, ISvcLocator* pSvcLocator )
  : ::AthReentrantAlgorithm( name, pSvcLocator ) {}

HypoBase::~HypoBase() {}


const SG::ReadHandleKey<TrigCompositeUtils::DecisionContainer>& HypoBase::decisionInput() const{
  return m_input;
}

const SG::WriteHandleKey<TrigCompositeUtils::DecisionContainer>& HypoBase::decisionOutput() const{
  return m_output;
}

StatusCode HypoBase::sysInitialize() {
  CHECK( AthReentrantAlgorithm::sysInitialize() ); // initialise base class
  CHECK( m_input.initialize() );
  ATH_MSG_DEBUG("HypoBase::sysInitialize() Will consume decision: " << m_input.key() );
  CHECK( m_output.initialize() );
  ATH_MSG_DEBUG("HypoBase::sysInitialize() And produce decision: " << m_output.key() );
  return StatusCode::SUCCESS;
}

StatusCode HypoBase::hypoBaseOutputProcessing(SG::WriteHandle<DecisionContainer>& outputHandle, MSG::Level lvl) const {

  // TODO: Tomasz, Francesca - if we wanted to do same-hypo-alg Combo logic we could hook it in here.
  // (This would be the idea to take the output from all the legs HypoTools, check the multiplicity 
  // and *remove* DecisionIDs of the combined chains if they fail. The Decision is still mutable here)
  // for ( auto& tool: m_hypoComboTools ) {
  //   ATH_CHECK( tool->decide( *outputHandle ) );
  // }

  ATH_CHECK( printDebugInformation(outputHandle, lvl) );

  if (m_runtimeValidation) {
    ATH_CHECK( runtimeValidation(outputHandle) );
  }

  return StatusCode::SUCCESS;
}


StatusCode HypoBase::runtimeValidation(SG::WriteHandle<DecisionContainer>& outputHandle) const {
  // Detailed checks on the output container of this HypoAlg
  for (const Decision* d : *outputHandle) {
    const DecisionContainer* dContainer = dynamic_cast<const DecisionContainer*>( d->container() );
    const ElementLink<DecisionContainer> dEL = ElementLink<DecisionContainer>(*dContainer, d->index());
    ATH_CHECK( dEL.isValid() );
    // Check that all Decisions produced here have a feature (HypoAlg specific)
    ATH_CHECK( validateHasFeature(dEL) );
    // Check that all Hypo Decisions produced here satisfy the more-strict all-parent logical flow 
    ATH_CHECK( validateLogicalFlow(dEL, kRequireAll) );
    // Check that we can reach L1 along all navigation paths up from each Decision
    // and validate these Decisions on the way up too.
    ATH_CHECK( recursiveValidateGraph(dEL) ); 
  }
  return StatusCode::SUCCESS;
}


StatusCode HypoBase::recursiveValidateGraph(const ElementLink<DecisionContainer>& dEL) const {
  // Check logical flow at this place in the graph 
  // (looser requirement of one-valid-parent-with-decision than we had when we knew that d corresponded to a HypoAlg output)
  ATH_CHECK( validateLogicalFlow(dEL, kRequireOne) );
  // Check my IDs
  ATH_CHECK( validateDecisionIDs(dEL) );
  ATH_CHECK( validateDuplicatedDecisionID(dEL) );
  // Check my linking
  ATH_CHECK( validateParentLinking(dEL) );
  // Continue upstream
  const ElementLinkVector<DecisionContainer> seeds = (*dEL)->objectCollectionLinks<DecisionContainer>(seedString());
  for (const ElementLink<DecisionContainer> seed : seeds) {
    ATH_CHECK( seed.isValid() );
    ATH_CHECK( recursiveValidateGraph(seed) );
  }
  return StatusCode::SUCCESS;
}


StatusCode HypoBase::validateParentLinking(const ElementLink<DecisionContainer>& dEL) const {
  const ElementLinkVector<DecisionContainer> seeds = (*dEL)->objectCollectionLinks<DecisionContainer>(seedString());
  // All Decision object must have at least one parent, unless they are the initial set of objects created by the L1 decoder
  if (seeds.size() == 0 && (*dEL)->name() != "L1") {
    printErrorHeader(dEL);
    ATH_MSG_ERROR("! Decision has zero parents. This is only allowed for the initial Decisions created by the L1Decoder.");
    ATH_MSG_ERROR("! SOLUTION: Attach parent Decision(s) with TrigCompositeUtils::linkToPrevious");
    printBangs();
      return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}


StatusCode HypoBase::validateDecisionIDs(const ElementLink<DecisionContainer>& dEL) const {
  // All numeric IDs must correspond to a know, configured, HLT chain
  DecisionIDContainer decisionIDSet;
  decisionIDs(*dEL, decisionIDSet);
  for (const DecisionID id : decisionIDSet) {
    if (HLT::Identifier( id ).name() == "UNKNOWN HASH ID") {
      printErrorHeader(dEL);
      ATH_MSG_ERROR("! Decision contains an ID which does not correspond to a configured chain: " << HLT::Identifier( id ));
      ATH_MSG_ERROR("! SOLUTION: Locate the producer of the collection, investigate how this bad ID could have been added.");
      printBangs();
      return StatusCode::FAILURE;
    }
  }
  return StatusCode::SUCCESS;
}


StatusCode HypoBase::validateDuplicatedDecisionID(const ElementLink<DecisionContainer>& dEL) const {
  // Persistent vector storage does not guarantee against duplicate entries
  DecisionIDContainer decisionIDSet;
  decisionIDs(*dEL, decisionIDSet);
  if (decisionIDSet.size() != (*dEL)->decisions().size()) {
    printErrorHeader(dEL);
    ATH_MSG_ERROR("! Decision contains duplicate DecisionIDs.");
    ATH_MSG_ERROR("! SOLUTION: If combining DecisionIDs from multiple parents,"
      "de-duplicate the internal std::vector<DecisionID> of 'Decision* d' with:");
    ATH_MSG_ERROR("!   TrigCompositeUtils::insertDecisionIDs(DecisionIDContainer(), d);");
    printBangs();
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}


StatusCode HypoBase::validateLogicalFlow(const ElementLink<DecisionContainer>& dEL, const LogicalFlowCheckMode mode) const {
  // Do not need to validate for L1 Decisions as these have no parents
  if ((*dEL)->name() == "L1") {
    return StatusCode::SUCCESS;
  }
  // Get all my passed DecisionIDs
  DecisionIDContainer decisionIDSet;
  decisionIDs(*dEL, decisionIDSet);
  const ElementLinkVector<DecisionContainer> seeds = (*dEL)->objectCollectionLinks<DecisionContainer>(seedString());
  for (const DecisionID id : decisionIDSet) {
    // For each chain that I'm passing, check how many of my parents were also passing the chain
    size_t parentsWithDecision = 0;  
    for (const ElementLink<DecisionContainer> seed : seeds) {
      ATH_CHECK( seed.isValid() );
      DecisionIDContainer seedIDSet;
      decisionIDs(*seed, seedIDSet);
      // Id may be a chain-ID (represetns a whole chain) or a leg-ID (represents just a single leg of a multi-leg chain)
      // Is ID is in this parent's set of passed IDs?
      // Or, (if ID is a leg-ID) is the chain-ID of leg-ID in the parent's set of passed IDs?
      if (passed(id, seedIDSet) or passed(getIDFromLeg(id).numeric(), seedIDSet)) {
        ++parentsWithDecision;
      }
      else{ // Or, for each of the seed IDs, if the seed ID is a leg-ID, is the seed chain-ID of the seed leg-ID the same as ID?  
        for (auto sid: seedIDSet){
          if (getIDFromLeg(sid).numeric() == id){
            ++parentsWithDecision;
            break;
          }
        }
      }
    }
    if (mode == kRequireOne && parentsWithDecision == 0) {
      // InputMakers may merge multiple of their input collections in order to run reconstruction on a common set of ROI (for example)
      // So the DecisionIDs may have come from any one or more of the inputs. But zero is not allowed.
      printErrorHeader(dEL);
      ATH_MSG_ERROR("! This Decision object is not respecting logical flow of DecisionIDs for chain: " << HLT::Identifier( id ));
      ATH_MSG_ERROR("! This chain's DecisionID can not be found in any parents of this Decision object:");
      for (const ElementLink<DecisionContainer> seed : seeds) {
        ATH_MSG_ERROR("! Index:" << (*seed)->index() << " from collection:" << seed.dataID());
        ATH_MSG_ERROR("! " << **seed);
      }
      ATH_MSG_ERROR("! SOLUTION: Ensure that the producer of this Decision object only adds DecisionIDs"
        " which were present in at least one of its parents.");
      printBangs();
      return StatusCode::FAILURE;
    } else if (mode == kRequireAll && parentsWithDecision != seeds.size()) {
      // HypoAlgs may form a new physics object from multiple objects in the previous step
      // (think a BPhysics object whose parents are two Decisions which each correspond to a different L1 MU RoI, 
      // both ROI need to be in active state for the chain, if the chain's HypoTool considers the BPhysics object)
      // This case requires *all* of the physics objects which are being combined together to be active for the chain
      // in order to preserve logical flow
      printErrorHeader(dEL);
      ATH_MSG_ERROR("! This Decision object is not respecting logical flow of DecisionIDs for chain: " << HLT::Identifier( id ));
      ATH_MSG_ERROR("! As this Decision object represents the output of a HypoAlg, it must respect logical flow on all " 
        << seeds.size() << " of its parent(s):");
      for (const ElementLink<DecisionContainer> seed : seeds) {
        ATH_MSG_ERROR("! Index:" << (*seed)->index() << " from collection:" << seed.dataID());
        ATH_MSG_ERROR("! " << **seed);
      }
      ATH_MSG_ERROR("! SOLUTION: Ensure that the HypoTool responsible for " << HLT::Identifier( id ) 
        << " in this HypoAlg only runs if this ID is present in all parent decisions.");
      printBangs();
      return StatusCode::FAILURE;
    }
  }
  return StatusCode::SUCCESS;
}


StatusCode HypoBase::validateHasFeature(const ElementLink<DecisionContainer>& dEL) const {
  // Check that I have a "feature"
  if ((*dEL)->hasObjectLink( featureString() )) {
    return StatusCode::SUCCESS;
  }
  // I might be a multi-slice Combo Hypo, if so, my immediate parents must all have features
  const ElementLinkVector<DecisionContainer> seeds = (*dEL)->objectCollectionLinks<DecisionContainer>(seedString());
  // The case of no-seeds is a separate validation check
  for (const ElementLink<DecisionContainer> seed : seeds) {
    ATH_CHECK( seed.isValid() );
    if ((*seed)->hasObjectLink( featureString() )) {
      continue; // Good
    }
    printErrorHeader(dEL);
    ATH_MSG_ERROR("! Decision has no '" << featureString() << "' ElementLink. Nor does its immediate parents (Combo case).");
    ATH_MSG_ERROR("! Every Decision created by a HypoAlg must correspond to some physics object, and be linked to the object.");
    ATH_MSG_ERROR("! (For steering controlled multi-slice ComboHypo algs, it is the earlier slice-controlled HypoAlgs which attach the features)");
    ATH_MSG_ERROR("! SOLUTION: If this is a ComboHypo, ensure all parent Decision objects are from HypoAlg, and hence have a '" << featureString() << "' ElementLink.");
    ATH_MSG_ERROR("! SOLUTION: If this is not a ComboHypo, ensure that all produced Decision objects are assigned their feature:");
    ATH_MSG_ERROR("! SOLUTION:    decision->setObjectLink<MY_FEATURE_CONTANER_TYPE>(featureString(), MY_FEATURE_ELEMENT_LINK);");
    printBangs();
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}


void HypoBase::printBangs() const {
  ATH_MSG_ERROR("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
}


void HypoBase::printErrorHeader(const ElementLink<DecisionContainer>& dEL) const  {
  printBangs();
  ATH_MSG_ERROR("! RUNTIME TRIGGER NAVIGATION VALIDATION ERROR");
  ATH_MSG_ERROR("! Caused by Decision with index:" << (*dEL)->index());
  ATH_MSG_ERROR("! From collection:" << dEL.dataID());
  ATH_MSG_ERROR("! " << **dEL);

}


StatusCode HypoBase::printDebugInformation(SG::WriteHandle<DecisionContainer>& outputHandle, MSG::Level lvl) const {
  if (msgLvl(lvl)) {
    msg() << lvl;
    msg() << "Exiting with " << outputHandle->size() <<" Decision objects" << endmsg;
    size_t count = 0;
    for (const Decision* d : *outputHandle){
      DecisionIDContainer objDecisions;      
      decisionIDs( d, objDecisions );
      msg() << "Number of positive decisions for Decision object #" << count++ << ": " << objDecisions.size() << endmsg;
      for (const TrigCompositeUtils::DecisionID id : objDecisions ) {
        msg() << " --- Passes: " << HLT::Identifier( id ) << endmsg;
      }  
    }
  }
  return StatusCode::SUCCESS;
}
