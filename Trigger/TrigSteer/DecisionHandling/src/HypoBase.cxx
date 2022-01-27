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

  ATH_CHECK( printDebugInformation(outputHandle, lvl) );

  if (m_runtimeValidation) {
    ATH_CHECK( runtimeValidation(outputHandle, msg()) );
  }

  return StatusCode::SUCCESS;
}


StatusCode HypoBase::runtimeValidation(SG::WriteHandle<DecisionContainer>& outputHandle,
  MsgStream& msg, 
  bool onlyValidateOneStep,
  bool runTwoConversion)
{
  // Detailed checks on the output container of this HypoAlg
  std::set<const Decision*> fullyExploredFrom; // Cache used to avoid exploring regions of the graph more than once
  for (const Decision* d : *outputHandle) {
    const DecisionContainer* dContainer = dynamic_cast<const DecisionContainer*>( d->container() );
    const ElementLink<DecisionContainer> dEL = ElementLink<DecisionContainer>(*dContainer, d->index());
    if (not dEL.isValid()) {
      msg << MSG::ERROR << "Invalid seed element link in recursiveValidateGraph" << endmsg;
      return StatusCode::FAILURE;
    }
    // Check that we can reach L1 along all navigation paths up from each Decision
    // and validate these Decisions on the way up too.
    if (recursiveValidateGraph(dEL, msg, onlyValidateOneStep, runTwoConversion, 0, fullyExploredFrom).isFailure()) {
      return StatusCode::FAILURE;
    } 
  }
  return StatusCode::SUCCESS;
}


StatusCode HypoBase::recursiveValidateGraph(const ElementLink<DecisionContainer>& dEL,
  MsgStream& msg,
  bool onlyValidateOneStep,
  bool runTwoConversion,
  size_t callDepth,
  std::set<const Decision*>& fullyExploredFrom)
{
  if (onlyValidateOneStep && callDepth > 0) {
    if ((*dEL)->name() == hypoAlgNodeName()) {
      // Validation is called from HypoAlg nodes. So if we have reached the _previous_ HypoAlg node, then we have already
      // validated back from here in the past. Can stop at this point.
      return StatusCode::SUCCESS;
    }
  }

  // Check logical flow at this place in the graph 
  if ((*dEL)->name() == hypoAlgNodeName()) {
    // Check that all Hypo Decisions produced here satisfy the more-strict all-parent logical flow 
    if ( validateLogicalFlow(dEL, msg, kRequireAll).isFailure() ) {
      return StatusCode::FAILURE;
    }
  } else {
    // (looser requirement of one-valid-parent-with-decision than we had when we knew that d corresponded to a HypoAlg output)
    if ( validateLogicalFlow(dEL, msg, kRequireOne).isFailure() ) {
      return StatusCode::FAILURE;
    }
  }

  // Check my IDs
  if ( validateDecisionIDs(dEL, msg).isFailure() ) {
    return StatusCode::FAILURE;
  }
  if ( validateDuplicatedDecisionID(dEL, msg).isFailure() ) {
    return StatusCode::FAILURE;
  }

  // Check my linking
  if( validateParentLinking(dEL, msg, runTwoConversion).isFailure() ) {
    return StatusCode::FAILURE;
  }
  if ( validateHasLinks(dEL, msg).isFailure() ) {
    return StatusCode::FAILURE;
  }

  // Continue upstream
  const ElementLinkVector<DecisionContainer> seeds = (*dEL)->objectCollectionLinks<DecisionContainer>(seedString());
  for (const ElementLink<DecisionContainer> seed : seeds) {
    if (fullyExploredFrom.count( (*seed) ) == 1) {
      continue; // Already fully explored from this seed and up
    }
    if ( not seed.isValid() ) {
      msg << MSG::ERROR << "Invalid seed element link in recursiveValidateGraph" << endmsg;
      return StatusCode::FAILURE;
    }
    if ( recursiveValidateGraph(seed, msg, onlyValidateOneStep, runTwoConversion, callDepth + 1, fullyExploredFrom).isFailure() ) {
      return StatusCode::FAILURE;
    }
  }

  fullyExploredFrom.insert( *dEL );
  return StatusCode::SUCCESS;
}


StatusCode HypoBase::validateParentLinking(const ElementLink<DecisionContainer>& dEL,
  MsgStream& msg,
  bool runTwoConversion)
{
  const ElementLinkVector<DecisionContainer> seeds = (*dEL)->objectCollectionLinks<DecisionContainer>(seedString());
  // All Decision object must have at least one parent, unless they are the initial set of objects created by the HLTSeeding
  const std::string& name = (*dEL)->name();
  if (seeds.size() == 0 && name != hltSeedingNodeName()) {
    printErrorHeader(dEL, msg);
    msg << MSG::ERROR << "! Decision has zero parents. This is only allowed for the initial Decisions created by the HLTSeeding." << endmsg;
    msg << MSG::ERROR << "! SOLUTION: Attach parent Decision(s) with TrigCompositeUtils::linkToPrevious" << endmsg;
    printBangs(msg);
    return StatusCode::FAILURE;
  }

  if (name == hltSeedingNodeName()) {
    if (seeds.size() > 0) {
      printErrorHeader(dEL, msg);
       msg << MSG::ERROR << "! Decision has parents. This is not allowed for the initial Decisions created by the HLTSeeding." << endmsg;
       msg << MSG::ERROR << "! SOLUTION: Check HLTSeeding, no where should it be adding a parent link." << endmsg;
      printBangs(msg);
      return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;
  }

  static const std::set<std::string> expectedParentsFilter = {hypoAlgNodeName(), comboHypoAlgNodeName(), hltSeedingNodeName()};
  static const std::set<std::string> expectedParentsInputMaker = {filterNodeName()};
  static const std::set<std::string> expectedParentsHypoAlg = {inputMakerNodeName()};
  static const std::set<std::string> expectedParentsComboHypoAlg = {hypoAlgNodeName(), inputMakerNodeName(), hltSeedingNodeName()}; // TODO check hltSeedingNodeName(), needed for newJO
  static const std::set<std::string> expectedParentsSummaryFilter = {hypoAlgNodeName(), comboHypoAlgNodeName(), hltSeedingNodeName()};
  static const std::set<std::string> expectedParentsSummaryPassed = {"SF"}; // TODO change to summaryFilterNodeName() when merged

  const std::set<std::string>* expectedParentsPtr = nullptr;
  if (name == filterNodeName()) {
    expectedParentsPtr = &expectedParentsFilter;
  } else if (name == inputMakerNodeName() and !runTwoConversion) {
    expectedParentsPtr = &expectedParentsInputMaker;
  } else if (name == inputMakerNodeName() and runTwoConversion) {
    expectedParentsPtr = &expectedParentsFilter; // We don't have Filter nodes in the R2->R3 conversion
  } else if (name == hypoAlgNodeName()) {
    expectedParentsPtr = &expectedParentsHypoAlg;
  } else if (name == comboHypoAlgNodeName()) {
    expectedParentsPtr = &expectedParentsComboHypoAlg;
  } else if (name == "SF") { // TODO change to summaryFilterNodeName() when merged
    expectedParentsPtr = &expectedParentsSummaryFilter;
  } else if (name == "HLTPassRaw") { // TODO change to summaryPassNodeNameString() when merged
    expectedParentsPtr = &expectedParentsSummaryPassed;
  } else {
    printErrorHeader(dEL, msg);
    msg << MSG::ERROR << "! Invalid Node name '" << name << "'." << endmsg;
    msg << MSG::ERROR << "! SOLUTION: Find the alg which made a node with this name. Allowed named may be found in TrigCompositeUtils.h, See:'Constant string literals used within the HLT'." << endmsg;
    printBangs(msg);
    return StatusCode::FAILURE;
  }

  for (const ElementLink<DecisionContainer> seed : seeds) {
    if (expectedParentsPtr->count( (*seed)->name() ) == 0) {
      printErrorHeader(dEL, msg);
      msg << MSG::ERROR << "! Invalid linking from node with name '" << name << "' to one with name '"<< (*seed)->name() << "'." << endmsg;
      msg << MSG::ERROR << "! Allowed seed names are:" << endmsg;
      for (const std::string& allowed : *expectedParentsPtr) {
        msg << MSG::ERROR << "! " << allowed << endmsg;
      }
      msg << MSG::ERROR << "! SOLUTION: Find where this invalid parent was added and correct it." << endmsg;
      printBangs(msg);
      return StatusCode::FAILURE;
    }
  }

  return StatusCode::SUCCESS;
}


StatusCode HypoBase::validateDecisionIDs(const ElementLink<DecisionContainer>& dEL,
  MsgStream& msg)
{
  // All numeric IDs must correspond to a know, configured, HLT chain
  DecisionIDContainer decisionIDSet;
  decisionIDs(*dEL, decisionIDSet);
  for (const DecisionID id : decisionIDSet) {
    const std::string chain = HLT::Identifier( id ).name();
    if (!isChainId(chain) and !isLegId(chain)) {
      printErrorHeader(dEL, msg);
      msg << MSG::ERROR << "! Decision contains an ID which does not correspond to a configured chain or a configured chain-leg: " << HLT::Identifier( id ) << endmsg;
      msg << MSG::ERROR << "! SOLUTION: Locate the producer of the collection, investigate how this bad ID could have been added." << endmsg;
      printBangs(msg);
      return StatusCode::FAILURE;
    }
  }
  return StatusCode::SUCCESS;
}


StatusCode HypoBase::validateDuplicatedDecisionID(const ElementLink<DecisionContainer>& dEL,
  MsgStream& msg)
{
  // Persistent vector storage does not guarantee against duplicate entries
  DecisionIDContainer decisionIDSet;
  decisionIDs(*dEL, decisionIDSet);
  if (decisionIDSet.size() != (*dEL)->decisions().size()) {
    printErrorHeader(dEL, msg);
    msg << MSG::ERROR << "! Decision contains duplicate DecisionIDs." << endmsg;
    msg << MSG::ERROR << "! SOLUTION: If combining DecisionIDs from multiple parents, de-duplicate the internal std::vector<DecisionID> of 'Decision* d' with:" << endmsg;
    msg << MSG::ERROR << "!   TrigCompositeUtils::insertDecisionIDs(DecisionIDContainer(), d);"  << endmsg;
    printBangs(msg);
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}


StatusCode HypoBase::validateLogicalFlow(const ElementLink<DecisionContainer>& dEL, 
  MsgStream& msg,
  const LogicalFlowCheckMode mode)
{
  // Do not need to validate for L1 Decisions as these have no parents
  if ((*dEL)->name() == hltSeedingNodeName()) {
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
      if ( not seed.isValid() ) {
        msg << MSG::ERROR << "Invalid seed element link in recursiveValidateGraph" << endmsg;
        return StatusCode::FAILURE;
      }
      DecisionIDContainer seedIDSet;
      decisionIDs(*seed, seedIDSet);
      // Id may be a chain-ID (represents a whole chain) or a leg-ID (represents just a single leg of a multi-leg chain)
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
      printErrorHeader(dEL, msg);
      msg << MSG::ERROR << "! This Decision object is not respecting logical flow of DecisionIDs for chain: " << HLT::Identifier( id ) << endmsg;
      msg << MSG::ERROR << "! This chain's DecisionID can not be found in any parents of this Decision object:" << endmsg;
      size_t seed_n = 0;
      for (const ElementLink<DecisionContainer> seed : seeds) {
        msg << MSG::ERROR << "! Index:" << (*seed)->index() << " from collection:" << seed.dataID() << endmsg;
        msg << MSG::ERROR << "! " << **seed << endmsg;
        DecisionIDContainer objDecisions;      
        decisionIDs(*seed, objDecisions);
        for (const TrigCompositeUtils::DecisionID id : objDecisions ) {
          msg << "! --- Passing in parent #" << seed_n << ": " << HLT::Identifier( id ) << endmsg;
        }
        ++seed_n;
      }
      msg << MSG::ERROR << "! SOLUTION: Ensure that the producer of this Decision object only adds DecisionIDs"
        " which were present in at least one of its parents." << endmsg;
      printBangs(msg);
      return StatusCode::FAILURE;
    } else if (mode == kRequireAll && parentsWithDecision != seeds.size()) {
      // HypoAlgs may form a new physics object from multiple objects in the previous step
      // (think a BPhysics object whose parents are two Decisions which each correspond to a different L1 MU RoI, 
      // both ROI need to be in active state for the chain, if the chain's HypoTool considers the BPhysics object)
      // This case requires *all* of the physics objects which are being combined together to be active for the chain
      // in order to preserve logical flow
      printErrorHeader(dEL, msg);
      msg << MSG::ERROR << "! This Decision object is not respecting logical flow of DecisionIDs for chain: " << HLT::Identifier( id ) << endmsg;
      msg << MSG::ERROR << "! As this Decision object represents the output of a HypoAlg, it must respect logical flow on all " 
        << seeds.size() << " of its parent(s):" << endmsg;
      size_t seed_n = 0;
      for (const ElementLink<DecisionContainer> seed : seeds) {
        msg << MSG::ERROR << "! Index:" << (*seed)->index() << " from collection:" << seed.dataID() << endmsg;
        msg << MSG::ERROR << "! " << **seed << endmsg;
        DecisionIDContainer objDecisions;      
        decisionIDs(*seed, objDecisions);
        for (const TrigCompositeUtils::DecisionID id : objDecisions ) {
          msg << "! --- Passing in parent #" << seed_n << ": " << HLT::Identifier( id ) << endmsg;
        }
        ++seed_n;
      }
      msg << MSG::ERROR << "! SOLUTION: Ensure that the HypoTool responsible for " << HLT::Identifier( id ) 
        << " in this HypoAlg only runs if this ID is present in all parent decisions." << endmsg;
      printBangs(msg);
      return StatusCode::FAILURE;
    }
  }
  return StatusCode::SUCCESS;
}


StatusCode HypoBase::validateHasLinks(const ElementLink<DecisionContainer>& dEL,
  MsgStream& msg)
{
  const std::string& name = (*dEL)->name();
  if (name == hypoAlgNodeName()) {

    // Check that I have a "feature"
    if ((*dEL)->hasObjectLink( featureString() )) {
      return StatusCode::SUCCESS;
    }
    printErrorHeader(dEL, msg);
    msg << MSG::ERROR << "! Decision has no '" << featureString() << "' ElementLink." << endmsg;
    msg << MSG::ERROR << "! Every Decision created by a HypoAlg must correspond to some physics object, and be linked to the object." << endmsg;
    msg << MSG::ERROR << "! SOLUTION: Ensure that all produced Decision objects are assigned their feature:" << endmsg;
    msg << MSG::ERROR << "! SOLUTION:    decision->setObjectLink<MY_FEATURE_CONTANER_TYPE>(featureString(), MY_FEATURE_ELEMENT_LINK);" << endmsg;
    printBangs(msg);
    return StatusCode::FAILURE;

  } else if (name == inputMakerNodeName()) {

    // This requirement is dropped for empty input makers to avoid unnecessary graph clutter.
    bool exempt = false;
    if ((*dEL)->hasDetail<int32_t>("isEmpty") and (*dEL)->getDetail<int32_t>("isEmpty") == 1) {
      exempt = true;
    }

    if (not (*dEL)->hasObjectLink( roiString() ) and not exempt) {
      printErrorHeader(dEL, msg);
      msg << MSG::ERROR << "! Decision has no '" << roiString() << "' ElementLink." << endmsg;
      msg << MSG::ERROR << "! Every Decision created by a InputMaker must link to the ROI which reconstruction will run on for that Decision object in this Step." << endmsg;
      msg << MSG::ERROR << "! It can be the FullScan ROI created by the HLTSeeding (FSNOSEED) if no other suitable ROI exists." << endmsg;
      msg << MSG::ERROR << "! SOLUTION: Check the configuration of InputMakerForRoI or EventViewCreatorAlgorithm responsible for creating this Decision Object" << endmsg;
      msg << MSG::ERROR << "! SOLUTION: The algorithm must have an ROITool which must attach an '"<< roiString() <<"' link to each Decision Object" << endmsg;
      printBangs(msg);
    }

  } else if (name == hltSeedingNodeName()) {

    if (not (*dEL)->hasObjectLink( initialRoIString() )) {
      printErrorHeader(dEL, msg);
      msg << MSG::ERROR << "! Decision has no '" << initialRoIString() << "' ElementLink." << endmsg;
      msg << MSG::ERROR << "! Every Decision created by the HLTSeeding must link to the initial ROI which caused it to be created." << endmsg;
      msg << MSG::ERROR << "! This includes the Decision Object created to represent the Full-Scan/NoSeed (FSNOSEED) ROI." << endmsg;
      msg << MSG::ERROR << "! SOLUTION: Check the configuration of the HLTSeeding tool responsible for creating this Decision Object" << endmsg;
      printBangs(msg);
    }

  }
  return StatusCode::SUCCESS;
}


void HypoBase::printBangs(MsgStream& msg) {
  msg << MSG::ERROR << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endmsg;
}


void HypoBase::printErrorHeader(const ElementLink<DecisionContainer>& dEL,
  MsgStream& msg)
{
  printBangs(msg);
  msg << MSG::ERROR << "! RUNTIME TRIGGER NAVIGATION VALIDATION ERROR" << endmsg;
  msg << MSG::ERROR << "! Caused by Decision with index:" << (*dEL)->index() << endmsg;
  msg << MSG::ERROR << "! From collection:" << dEL.dataID() << endmsg;
  msg << MSG::ERROR << "! " << **dEL << endmsg;
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
