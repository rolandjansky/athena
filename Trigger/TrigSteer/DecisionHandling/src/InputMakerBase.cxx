/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "DecisionHandling/InputMakerBase.h"
#include "TrigCompositeUtils/HLTIdentifier.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"

using namespace TrigCompositeUtils;
#define dodebug true

InputMakerBase::InputMakerBase( const std::string& name, ISvcLocator* pSvcLocator )
: ::AthReentrantAlgorithm( name, pSvcLocator ) {}

InputMakerBase::~InputMakerBase() {}

const SG::ReadHandleKeyArray<TrigCompositeUtils::DecisionContainer>& InputMakerBase::decisionInputs() const{
  return m_inputs;
}

const SG::WriteHandleKey<TrigCompositeUtils::DecisionContainer>& InputMakerBase::decisionOutputs() const{
  return m_outputs;
}

StatusCode InputMakerBase::sysInitialize() {
  CHECK( AthReentrantAlgorithm::sysInitialize() ); // initialise base class
  ATH_MSG_DEBUG(m_isEmptyStep);
  ATH_MSG_DEBUG(m_mergeUsingFeature);
  ATH_MSG_DEBUG("roisLink="<<m_roisLink);
  CHECK( m_inputs.initialize() );
  renounceArray(m_inputs); // make inputs implicit, i.e. not required by scheduler
  ATH_MSG_DEBUG("Will consume implicit decisions:" );
  for (auto& input: m_inputs){  
    ATH_MSG_DEBUG( " "<<input.key() );
  }
  CHECK( m_outputs.initialize() );
  ATH_MSG_DEBUG(" and produce decisions: " << m_outputs.key());
  return StatusCode::SUCCESS; 
}

// Create one output container and merge all inputs into this, using heuristics to identify equal Decision objects.
// If the input is invalid or empty, the output is not created, resulting as invalid
StatusCode InputMakerBase::decisionInputToOutput(const EventContext& context, SG::WriteHandle<TrigCompositeUtils::DecisionContainer>& outputHandle) const{
  
  ATH_MSG_DEBUG("Creating one merged output per Decision object, IsEmptyStep flag = " << m_isEmptyStep);
  ATH_CHECK( outputHandle.isValid() );
  TrigCompositeUtils::DecisionContainer* outDecisions = outputHandle.ptr();

  // Empty IMs have a dynamic (runtime) matching configuration. But any single IM should find itself using exclusively-feature or exclusively-ROI based matching
  size_t usedROIMatching = 0, usedFeatureMatching = 0; 

  MatchingCache matchingCache; // Used to remember temporarily which hash is associated with each Decision object when performing the de-duplication matching 
  size_t totalInput = 0;
  for ( const auto& inputKey: decisionInputs() ) {
    auto inputHandle = SG::makeHandle( inputKey, context );

    if( not inputHandle.isValid() ) {
      ATH_MSG_DEBUG( "Got no decisions from input "<< inputKey.key() << " because implicit handle not valid");
      continue;
    }
    if( inputHandle->size() == 0){
      ATH_MSG_DEBUG( "Got no decisions from input "<< inputKey.key()<<": implicit handle is valid but container is empty.");
      continue;
    }
    ATH_MSG_DEBUG( "Running on input "<< inputKey.key()<<" with " << inputHandle->size() << " elements" );
    

    size_t input_counter = 0;
    for (const TrigCompositeUtils::Decision* inputDecision : *inputHandle){
      ATH_MSG_DEBUG( " -- Input Decision " << input_counter <<": has " <<TrigCompositeUtils::getLinkToPrevious(inputDecision).size()<<" previous links");

      bool usedROIMatchingFlag = false;
      size_t alreadyAddedIndex = std::numeric_limits<std::size_t>::max();
      const bool alreadyAdded = matchInCollection(outDecisions, inputDecision, alreadyAddedIndex, usedROIMatchingFlag, matchingCache);      

      TrigCompositeUtils::Decision* outputDecision = nullptr;
      if (alreadyAdded) { // Already added, at alreadyAddedIndex
        if (usedROIMatchingFlag) {
         ++usedROIMatching;
        } else {
          ++usedFeatureMatching;
        }
        outputDecision = outDecisions->at( alreadyAddedIndex );
        ATH_MSG_DEBUG(  "  -- Matched to existing, " << inputKey.key() << " index " << input_counter << " is merged into existing output index " << alreadyAddedIndex << (usedROIMatchingFlag ? " Matched using ROI" : " Matched using Feature"));
      } else { // Not already added, make new
        outputDecision = TrigCompositeUtils::newDecisionIn( outDecisions );
        outputDecision->setName(inputMakerNodeName());
        outputDecision->setDetail<int32_t>("isEmpty", static_cast<int32_t>(m_isEmptyStep)); // Transient detail (only used by runtime navigation validation, never persisted)
        matchingCache.linkOutputToInput(outputDecision, inputDecision);
        ATH_MSG_DEBUG( "  -- Did not match to existing, " << inputKey.key() << " index " << input_counter << " creates output index " << outDecisions->size()-1);
      }

      // Note: We will call linkToPrevious and insertDecisionIDs N times on a single "outputDecision", where N is the number of inputDecision which checkExisting determines are the same object.
      TrigCompositeUtils::linkToPrevious( outputDecision, inputDecision, context ); // Link inputDecision object as the 'seed' of outputDecision
      TrigCompositeUtils::insertDecisionIDs( inputDecision, outputDecision ); // Copy decision IDs from inputDecision into outputDecision
      ATH_MSG_DEBUG("      -- This output decision now has " << TrigCompositeUtils::getLinkToPrevious(outputDecision).size() << " seeds and "<< outputDecision->decisions().size() << " decisionIds");           
      input_counter++;  
    } // loop over input decisions
    totalInput+=input_counter;
  } // end of: for ( auto inputKey: decisionInputs() )

  if (usedROIMatching and usedFeatureMatching) {
    // downgrade ERROR to WARNING temporarely, since we have one case of tau+jet+tauprobe chain interaction, which meets this condition
    // stay with WARNING to monitor such cases, then remove if they seem to be ok
    ATH_MSG_WARNING("This input maker used Feature-based mapping on " << usedFeatureMatching << " inputs and ROI-based mapping on " << usedROIMatching
      << " inputs. The isEmptyIM flag is " << m_isEmptyStep);
    //return StatusCode::FAILURE;
  }

  // Print some debug messages summarising the content of the outputHandles.
  if (msgLvl(MSG::DEBUG)) {
    debugPrintOut(context, outputHandle);
  }
  ATH_MSG_DEBUG("Merging complete: from "<<totalInput<<" input decision to "<<outputHandle->size()<<" output decisions");

  return StatusCode::SUCCESS;
}


bool InputMakerBase::matchInCollection(const DecisionContainer* outDecisions, const Decision* toMatch, size_t& matchIndex, bool& usedROIMatchingFlag, MatchingCache& matchingCache) const {
  std::set<const Decision*> cache; //!< Used to accelerate the recursive typelessFindLinks.
  std::vector<uint32_t> keys;
  std::vector<uint32_t> clids;
  std::vector<uint16_t> indicies;
  std::vector<const Decision*> sources; // Unused
  TrigCompositeUtils::typelessFindLinks(toMatch, featureString(), keys, clids, indicies, sources, TrigDefs::lastFeatureOfType, &cache);
  bool hasFeature = false;
  if (keys.size() != 0) hasFeature=true;
  ATH_MSG_DEBUG("This decision hasFeature="<< hasFeature);
  // Do feature based matching if configured to do so, or do it tentatively for empty step IMs (will not generate an ERROR if it fails for the empty step case)
  if ( m_mergeUsingFeature == true or (m_isEmptyStep and hasFeature) ) {
    matchIndex = matchDecision(outDecisions, toMatch, featureString(), matchingCache);
    ATH_MSG_DEBUG("matchDecision in features: "<< matchIndex);
  }
  // Do ROI based batching if configured to do so, or if feature matching failed for an empty step IM.
  else {
    matchIndex = matchDecision(outDecisions, toMatch, m_roisLink.value(), matchingCache);
    usedROIMatchingFlag = true;
  }
  return (matchIndex != std::numeric_limits<std::size_t>::max());
}

size_t InputMakerBase::matchDecision(const DecisionContainer* outDecisions, const Decision* toMatch, const std::string& linkNameToMatch, MatchingCache& matchingCache) const {  
  const uint64_t matchingHash = getMatchingHashForDecision(toMatch, linkNameToMatch);
  ATH_MSG_DEBUG("matchDecision "<<linkNameToMatch<<" with matchingHash="<<matchingHash);
  // Cache this matching hash, so that in subsequent calls we can compare other incoming decision object's matching hash against this one
  matchingCache.setMatchingHash(toMatch, matchingHash);

  // Look for match
  for (size_t index = 0; index < outDecisions->size(); ++index) {
    const TrigCompositeUtils::Decision* checkDecision = outDecisions->at(index);
    const uint64_t checkHash = matchingCache.getMatchingHash(checkDecision);
    ATH_MSG_DEBUG("matchDecision checkHash="<<checkHash<<" index="<<index);
    if (checkHash == matchingHash) {
      return index;
    }
  }

  return std::numeric_limits<std::size_t>::max();
}

uint64_t InputMakerBase::getMatchingHashForDecision(const Decision* toMatch, const std::string& linkNameToMatch) const {
  std::set<const Decision*> cache; //!< Used to accelerate the recursive typelessFindLinks.
  std::vector<uint32_t> keys;
  std::vector<uint32_t> clids;
  std::vector<uint16_t> indicies;
  std::vector<const Decision*> sources; // Unused
  TrigCompositeUtils::typelessFindLinks(toMatch, linkNameToMatch, keys, clids, indicies, sources, TrigDefs::lastFeatureOfType, &cache);
  ATH_MSG_DEBUG("getMatchingHashForDecision keys.size()="<<keys.size());
  if (keys.size() != 1) {
    bool suppressError = false;
    // We silence this ERROR when the IM isEmptyStep and we are tentatively testing at runtime if there is an available "feature" link.
    if (m_isEmptyStep and linkNameToMatch == featureString()) {
      suppressError = true;
    }
    // However... we _always_ want to print the ERROR if we got here not because keys.size() == 0 but because keys.size() > 1!
    // This is an ambiguous situation, and such things are always bad.
    if ( keys.size() > 1) {
      suppressError = false;
    }
    if (not suppressError) {
      ATH_MSG_ERROR("InputMakerBase::getMatchingHashForDecision Did not locate exactly one object having searched for a link named '" << linkNameToMatch 
        << "', found " << keys.size() << ". Unable to match this Decision object.");
      for (size_t i = 0; i < keys.size(); ++i) {
        const std::string* sgKeyStr = evtStore()->keyToString(keys.at(i));
        ATH_MSG_ERROR("  -- Key:" << keys.at(i) << " KeyStr:" << (sgKeyStr ? *sgKeyStr : "UNKNOWN") << " Index:" << indicies.at(i) << " CLID:" << clids.at(i)); 
      }
      // Super verbose output
      ATH_MSG_ERROR(" -- -- TRACKING FULL HISTORY ");
      NavGraph navGraph;
      recursiveGetDecisions(toMatch, navGraph);
      navGraph.printAllPaths(msg(), MSG::ERROR); 
    }
    return std::numeric_limits<std::size_t>::max();
  }

  // Construct a new hash by combining the SGkey (a hash), the CLID (a hash) and the index (an offset)
  const uint64_t matchingHash = keys.at(0) + clids.at(0) + indicies.at(0);
  return matchingHash;
}


void InputMakerBase::debugPrintOut(const EventContext& context, SG::WriteHandle<TrigCompositeUtils::DecisionContainer>& outputHandle) const{
  size_t validInput=0;
  for ( const auto& inputKey: decisionInputs() ) {
    auto inputHandle = SG::makeHandle( inputKey, context );
    ATH_MSG_DEBUG(" " << inputKey.key() << " " << (inputHandle.isValid()? "valid": "not valid" ) );
    if (inputHandle.isValid()) {
      if (inputHandle->size() > 0) {
        validInput++;
      } else {
        ATH_MSG_DEBUG("   " << inputKey.key() << " actually NOT valid due to size() == 0");
      }
    }
  }
  ATH_MSG_DEBUG( "Number of implicit ReadHandles for input decisions is " << decisionInputs().size() << ", " << validInput << " are valid/not-empty" );
  
  ATH_MSG_DEBUG("Output " << outputHandle.key() <<" with "<< outputHandle->size() <<" decisions:");
  for (const auto outdecision : *outputHandle){
    TrigCompositeUtils::DecisionIDContainer objDecisions;      
    TrigCompositeUtils::decisionIDs( outdecision, objDecisions );    
    ATH_MSG_DEBUG("Number of positive decisions for this output: " << objDecisions.size() );
    for ( TrigCompositeUtils::DecisionID id : objDecisions ) {
      ATH_MSG_DEBUG( " ---  decision " << HLT::Identifier( id ) );
    }  
  }
}


void InputMakerBase::MatchingCache::setMatchingHash(const Decision* inputDecision, const uint64_t matchingHash) {
  m_inputDecToMatchingHash[inputDecision] = matchingHash;
}


void InputMakerBase::MatchingCache::linkOutputToInput(const Decision* outputDecision, const Decision* inputDecision) {
  m_outputDecToInputDec[outputDecision] = inputDecision;
}


uint64_t InputMakerBase::MatchingCache::getMatchingHash(const Decision* outputDecision) const {
  if (!outputDecision) {
    throw std::runtime_error("InputMakerBase::MatchingCache::getMatchingHash: Called with nullptr.");
  }

  auto it_inDec = m_outputDecToInputDec.find(outputDecision);
  if (it_inDec == m_outputDecToInputDec.end()) {
    throw std::runtime_error("InputMakerBase::MatchingCache::getMatchingHash: No matching input Decision* for the supplied output Decision*");
  }
  const Decision* inputDecision = it_inDec->second;

  auto it_hash = m_inputDecToMatchingHash.find(inputDecision);
  if (it_hash == m_inputDecToMatchingHash.end()) {
    throw std::runtime_error("InputMakerBase::MatchingCache::getMatchingHash: No matching hash for this input Decision*");
  }
  const uint64_t hash = it_hash->second;

  return hash;
}
