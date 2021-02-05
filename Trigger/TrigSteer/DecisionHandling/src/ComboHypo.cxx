/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "DecisionHandling/ComboHypo.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "TrigCompositeUtils/HLTIdentifier.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include "AthViews/View.h"
#include <sstream>

using namespace TrigCompositeUtils;

ComboHypo::ComboHypo(const std::string& name, ISvcLocator* pSvcLocator) 
: ::AthReentrantAlgorithm(name, pSvcLocator)
{}


ComboHypo::~ComboHypo()
{}


StatusCode ComboHypo::initialize() {

  ATH_CHECK( m_outputs.initialize() );
  ATH_CHECK( m_inputs.initialize() );
  ATH_CHECK( m_inputs.size() == m_outputs.size() );
  ATH_MSG_INFO( "with these inputs: ");
  for (auto inp:m_inputs){
    ATH_MSG_INFO("-- "<< inp.key());
  }
  
  if (m_hypoTools.size()>0)
    ATH_CHECK(m_hypoTools.retrieve());
  
  // find max inputs size
  auto maxMultEl = std::max_element( m_multiplicitiesReqMap.begin(), m_multiplicitiesReqMap.end(),  
    []( MultiplicityReqMap::value_type a, MultiplicityReqMap::value_type b ){ return a.second.size() < b.second.size(); }
    ); 
  
  const size_t maxMult = maxMultEl->second.size();

  if (msgLvl(MSG::INFO)){ 
    ATH_MSG_INFO( "with this multiplicity map: ");
    for ( const auto& m : m_multiplicitiesReqMap ) {
      std::stringstream msgSS;
      msgSS << "[";
      for (const int mult : m.second){
        msgSS << mult << ", ";
      }
      msgSS.seekp(-2,msgSS.cur); // remove comma
      msgSS << "]";
      ATH_MSG_INFO("-- " << m.first << " multiplicities: " << msgSS.str() );
    }
  }

  ATH_CHECK( m_multiplicitiesReqMap.size() != 0 );

  bool errorOccured = false;
  if (m_checkMultiplicityMap) {
    for ( const auto& m : m_multiplicitiesReqMap ) {
      if ( m.second.size() != maxMult )  {
        errorOccured =  true;
        ATH_MSG_ERROR( "Chain " << m.first
          << " configured with input multiplicity " << m.second.size() << " like this: " << m.second
          << " which is lower than for this chain " << maxMultEl->first << " " << maxMult);
      }
    }
  }

  return ( errorOccured ? StatusCode::FAILURE : StatusCode::SUCCESS );
}


StatusCode ComboHypo::finalize() {
  return StatusCode::SUCCESS;
}


StatusCode ComboHypo::copyDecisions(  const LegDecisionsMap & passingLegs, const EventContext& context ) const {
 DecisionIDContainer passing;
  for (auto const& element : passingLegs) {
    passing.insert(element.first);
  }
  
  ATH_MSG_DEBUG( "Copying "<<passing.size()<<" positive decision IDs to outputs");

  for ( size_t input_counter = 0; input_counter < m_inputs.size(); ++input_counter ) {
    // new output decisions
    SG::WriteHandle<DecisionContainer> outputHandle = createAndStore(m_outputs.at(input_counter), context ); 
    auto outDecisions = outputHandle.ptr();    
    auto inputHandle = SG::makeHandle( m_inputs.at(input_counter), context );
    if ( inputHandle.isValid() ) {

      for (const Decision* inputDecision : *inputHandle) {
        auto thisEL = TrigCompositeUtils::decisionToElementLink(inputDecision, context);
        DecisionIDContainer inputDecisionIDs;
        decisionIDs( inputDecision, inputDecisionIDs );

        // from all positive decision in the input only the ones that survived counting are passed over
        DecisionIDContainer common;      
        std::set_intersection( inputDecisionIDs.begin(), inputDecisionIDs.end(), passing.begin(), passing.end(),
          std::inserter( common, common.end() ) );

        // check if this EL is in the combination map for the passing decIDs:
        ATH_MSG_DEBUG("Searching this element in the map: ("<<thisEL.dataID() << " , " << thisEL.index()<<")");
        DecisionIDContainer finalIds;   
        for (const DecisionID c : common){
          const HLT::Identifier cID = HLT::Identifier(c);
          // add the decID only if this candidate passed the combination selection
          const ElementLinkVector<DecisionContainer>& Comb=passingLegs.at(c);
          if(std::find(Comb.begin(), Comb.end(), thisEL) == Comb.end()) {
            continue;
          }
          ATH_MSG_DEBUG("  Adding "<< cID <<" because EL is found in the passingLegs map");
          finalIds.insert( cID.numeric() ); // all Ids used by the Filter, including legs
          if (TrigCompositeUtils::isLegId ( cID )){
            const HLT::Identifier mainChain = TrigCompositeUtils::getIDFromLeg( cID );
            finalIds.insert( mainChain.numeric() );
            ATH_MSG_DEBUG("  Adding "<< mainChain <<" consequently");
          }
        }

        Decision* newDec = newDecisionIn( outDecisions, inputDecision, comboHypoAlgNodeName(), context );
        ATH_MSG_DEBUG("New decision (Container Index:" << input_counter << ", Element Index:"<< newDec->index() <<") has "
          << (TrigCompositeUtils::findLink<TrigRoiDescriptorCollection>(newDec, initialRoIString())).isValid()
          << " valid initialRoI, "<< TrigCompositeUtils::getLinkToPrevious(newDec).size() <<" previous decisions and "<<finalIds.size()<<" decision IDs") ;   
        insertDecisionIDs( finalIds, newDec );

      }
    }

    if (msgLvl(MSG::DEBUG)) {
      ATH_MSG_DEBUG("Output Handle " << m_outputs.at(input_counter).key() << " with " << outputHandle->size() <<" Decision objects");
      for (const Decision* d : *outputHandle){
        DecisionIDContainer objDecisions;      
        decisionIDs( d, objDecisions );
        ATH_MSG_DEBUG("  Decision object #" << d->index() << " with " << objDecisions.size()<<" positive decision IDs");
        for (const TrigCompositeUtils::DecisionID id : objDecisions ) {
          ATH_MSG_DEBUG("   --- Passes: " << HLT::Identifier( id ));
        }  
      }
    }
  }

  return StatusCode::SUCCESS;
}


StatusCode ComboHypo::execute(const EventContext& context ) const {
  ATH_MSG_DEBUG( "Executing " << name() << "..." );
  
  // it maps decidionID to the combinations (list of dec object) that passed that ID
  LegDecisionsMap dmap;
  ATH_CHECK( fillDecisionsMap( dmap, context ) );

  //this is added for saving good combinations for the hypocombo tools
  LegDecisionsMap passingLegs;


  // loop over all chains in the mult-map
  for ( const auto& m : m_multiplicitiesReqMap ) { 
    const HLT::Identifier chainId = HLT::Identifier(m.first);
    const std::vector<int>& multiplicityPerLeg = m.second;
    const DecisionID requiredDecisionID = chainId.numeric();

    DecisionIDContainer allDecisionIds;
    allDecisionIds.insert(requiredDecisionID);

    bool overallDecision = true;

    std::vector< std::set<uint32_t> > legFeatureHashes; //!< Keeps track per leg of the hash of the objects passing the leg
    legFeatureHashes.resize( multiplicityPerLeg.size() );
    size_t fsCount = 0; //!< We allow the FullScan ROI to pass any multiplicity. So we may have to magic up some unique hashes. Counting integers work fine.

    LegDecisionsMap thisChainCombMap;

    // Check multiplicity of each leg of this chain
    for ( size_t legIndex = 0; legIndex <  multiplicityPerLeg.size(); ++legIndex ) {
      const size_t requiredMultiplicity =  multiplicityPerLeg.at( legIndex );

      HLT::Identifier legId = TrigCompositeUtils::createLegName(chainId, legIndex);
      // If there is only one leg, then we just use the chain's name.
      if (multiplicityPerLeg.size() == 1) {
        ATH_MSG_DEBUG(chainId << " has multiplicityPerLeg.size() == 1, so we don't use legXXX_HLT_YYY, we just use HLT_YYY");
        legId = chainId; 
      }

      const DecisionID requiredDecisionIDLeg = legId.numeric();
      ATH_MSG_DEBUG("Container " << legIndex << ", looking at leg : " << legId );
     
      LegDecisionsMap::const_iterator it = dmap.find(requiredDecisionIDLeg);
      if ( it == dmap.end() ) {
        overallDecision = false;
        break;
      }

      // Check the total number of decision objects we have available on this leg from which to satisfy its multiplicity requirement
      const size_t nLegDecisionObjects = it->second.size();

      ATH_MSG_DEBUG( "Will attempt to meet the required multiplicity of " << requiredMultiplicity  << " for leg " << legId 
        << " with " << nLegDecisionObjects << " Decision Objects in leg " << legIndex << " of " << legId);

      // We extract unique passing features on the leg, if there are no features yet - then the L1 ROI is used as a fall-back feature
      // A special behaviour is that if this fall-back triggers, and the full-scan ROI, then the leg is assumed valid. 
      // This is regardless of whether or not other legs also use the same FS ROI. Regardless of if the leg's required multiplicity.
      // This keeps the leg alive until the actual FS reconstruction may occur. At which point, the following ComboHypo will begin
      // to cut on the actual reconstructed physics objects.
      //
      // The behaviour may also be kept even after we have started to process a leg through HypoAlgs. This is done by the HypoAlg
      // setting the "feature" to be the same as the initialRoI. The initialRoI must still be a FullScan ROI for this to work.
      for (const ElementLink<DecisionContainer> dEL : it->second){
        uint32_t featureKey = 0, roiKey = 0;
        uint16_t featureIndex = 0, roiIndex = 0;
        bool roiFullscan = false;
        // NOTE: roiKey, roiIndex are only currently used in the discrimination for L1 Decision objects (which don't have a 'feature' link)
        // NOTE: We should make it configurable to choose either the feature or the ROI here, as done in the InputMaker base class when merging.
        ATH_CHECK( extractFeatureAndRoI(dEL, featureKey, featureIndex, roiKey, roiIndex, roiFullscan) );
        if (roiFullscan and (    (featureKey == roiKey and featureIndex == roiIndex) // The user explicitly set the feature === the initialRoI (and it is FS)
                              or (featureKey == 0 and roiKey != 0) // The leg has not yet started to process, the initialRoI is FS
                            )
           )
        {
          // This fsCount integer is being to generate unique "hash" values to allow the FS ROI to meet the multiplicity requirements of this leg
          for (size_t i = 0; i < requiredMultiplicity; ++i) {
            legFeatureHashes.at( legIndex ).insert( ++fsCount );
            ATH_MSG_DEBUG("  -- Add feature hash '" << fsCount << "' to leg " << legIndex << ". (Note: passing hash generated from FullScan ROI)");
          }
        } else {
          const uint32_t uniquenessHash = (featureKey != 0 ? (featureKey + featureIndex) : (roiKey + roiIndex)); 
          legFeatureHashes.at( legIndex ).insert( uniquenessHash );
          ATH_MSG_DEBUG("  -- Add feature hash '" << uniquenessHash << "' to leg " << legIndex << ".");
        }
      }

      // save combinations of all legs for the tools
      thisChainCombMap.insert (*it);
      allDecisionIds.insert(requiredDecisionIDLeg);
      
    } // end loop over legIndex


    // Remove any duplicated features which are shared between legs.
    // Keep the feature only in the leg which can afford to loose the least number of object, given its multiplicity requirement.
    std::set<uint32_t> allFeatureHashes;
    for (const std::set<uint32_t>& legHashes : legFeatureHashes) {
      allFeatureHashes.insert(legHashes.begin(), legHashes.end());
    }
    for (const uint32_t featureHash : allFeatureHashes) {
      size_t legsWithHash = 0; //!< If this grows greater than one then we have to start culling features from legs
      size_t keepLegIndex = 0; //!< If the hash is used in multiple legs, which leg can least afford to loose it?
      int32_t keepLegMargin = std::numeric_limits<int32_t>::max(); //!< How many features the leg at keepLegIndex can afford to loose before it starts to fail its multiplicity requirement. 
      for (size_t legIndex = 0; legIndex <  multiplicityPerLeg.size(); ++legIndex) {
        if (legFeatureHashes.at(legIndex).count(featureHash) == 0) {
          continue;
        }
        ++legsWithHash;
        const int32_t requiredMultiplicity = multiplicityPerLeg.at(legIndex);
        const int32_t currentMultiplicity = legFeatureHashes.at(legIndex).size();
        const int32_t safetyMargin = currentMultiplicity - requiredMultiplicity; // Signed, if -ve then the chain has already been failed by the leg at legIndex
        if (safetyMargin < keepLegMargin) {
          keepLegMargin = safetyMargin;
          keepLegIndex = legIndex;
        }
      }
      if (legsWithHash == 1) {
        continue;
      }
      // If a feature is found on multiple legs, then remove it from all but the leg which can afford to loose it the least
      for (size_t legIndex = 0; legIndex <  multiplicityPerLeg.size(); ++legIndex) {
        if (legIndex == keepLegIndex) {
          ATH_MSG_DEBUG("Keeping feature hash '" << featureHash << "', on leg " << legIndex << ". This leg can least afford to loose it. "
            << "Leg has " << legFeatureHashes.at(legIndex).size() 
            << " features, and a multiplicity requirement of " << multiplicityPerLeg.at(legIndex));
          continue;
        }
        if (legFeatureHashes.at(legIndex).erase(featureHash)) {
          ATH_MSG_DEBUG("Removed duplicate feature hash '" << featureHash << "', from leg " << legIndex << ". Leg now has " << legFeatureHashes.at(legIndex).size() 
            << " remaining features, and a multiplicity requirement of " << multiplicityPerLeg.at(legIndex));
        }
      }
    }

    //check that the multiplicity of unique features is high enough
    for (size_t legIndex = 0; legIndex <  multiplicityPerLeg.size(); ++legIndex) {
      const size_t requiredMultiplicity = multiplicityPerLeg.at(legIndex);
      const size_t currentMultiplicity = legFeatureHashes.at(legIndex).size();
      if (currentMultiplicity < requiredMultiplicity) {
        ATH_MSG_DEBUG("Leg " << legIndex << " fails multiplicity check. Required unique features:" << requiredMultiplicity << ", found unique features: " << currentMultiplicity);
        overallDecision = false;
        break;
      }
    }

    //Overall chain decision
    ATH_MSG_DEBUG( "Chain " << chainId <<  ( overallDecision ? " is accepted" : " is rejected")  <<" after multiplicity requirements" );
    if ( overallDecision == true ) {
      for (auto decID: allDecisionIds) {
        // saving the good combinations
        passingLegs.insert (thisChainCombMap.begin(), thisChainCombMap.end());
        ATH_MSG_DEBUG("  Passing " << HLT::Identifier(decID)<<" after multiplicity test");
      }
    }      
  }

  if  (passingLegs.size()!=0){
    // launching the tools:
    ///////////////////////
    if (m_hypoTools.size()>0){
      for ( auto& tool: m_hypoTools ) {
        ATH_MSG_DEBUG( "Calling  tool "<<tool->name());
        ATH_CHECK( tool->decide( passingLegs, context ) );
      }
    }
  }

  // this is only for debug:
  if (msgLvl(MSG::DEBUG)){
    DecisionIDContainer passing;
    for (auto const& element : passingLegs) {
      passing.insert(element.first);
    }
    for (auto p: passing)
    ATH_MSG_DEBUG("Passing "<<HLT::Identifier(p));
  }

  // need to pass all combinations, since not all element pass the decID
  ATH_CHECK( copyDecisions( passingLegs, context ) );
  
  return StatusCode::SUCCESS;
}


StatusCode ComboHypo::extractFeatureAndRoI(const ElementLink<DecisionContainer>& dEL,
  uint32_t& featureKey, uint16_t& featureIndex, uint32_t& roiKey, uint16_t& roiIndex, bool& roiFullscan) const 
{
  uint32_t featureClid = 0; // Note: Unused. We don't care what the type of the feature is here
  const bool foundFeature = typelessFindLink((*dEL), featureString(), featureKey, featureClid, featureIndex);
  // Try and get seeding ROI data too. Don't need to be type-less here
  LinkInfo<TrigRoiDescriptorCollection> roiSeedLI = findLink<TrigRoiDescriptorCollection>((*dEL), initialRoIString());
  if (roiSeedLI.isValid()) {
    roiKey = roiSeedLI.link.key();
    roiIndex = roiSeedLI.link.index();
    roiFullscan = (*(roiSeedLI.link))->isFullscan();
  }
  if (!foundFeature && !roiSeedLI.isValid()) {
    ATH_MSG_WARNING("Did not find the feature or initialRoI for " << dEL.dataID() << " index " << dEL.index());
  }
  return StatusCode::SUCCESS;
}


StatusCode ComboHypo::fillDecisionsMap( LegDecisionsMap &  dmap, const EventContext& context) const {
  for ( size_t inputContainerIndex = 0; inputContainerIndex < m_inputs.size(); ++inputContainerIndex ) {   
    auto inputHandle = SG::makeHandle( m_inputs.at(inputContainerIndex), context );
    if ( !inputHandle.isValid() ) {
      ATH_MSG_ERROR( "No input ReadHandle from " << inputHandle.key() );
      return StatusCode::FAILURE;
    }
    ATH_MSG_DEBUG( "Found ReadHandle from " << inputHandle.key() <<" with "<< inputHandle->size() << " elements:"  );
    for ( const Decision* decision : *inputHandle ) {
      ATH_MSG_DEBUG( "Input Decision #"<< decision->index() <<" with "<< decisionIDs( decision ).size() << " active IDs; these are found in the multiplicity map:" );
      for ( const DecisionID id: decisionIDs( decision ) ) {
        for ( const auto& m : m_multiplicitiesReqMap ) {
          // Search for this ID in the list of active chains processed by this ComboHypo
          // Note: We do a find() check rather than require equality as m_multiplicitiesReqMap is configured by chain name
          // whereas id is here the per-leg name. The chain name is a sub-string of the leg name.
          if (HLT::Identifier(id).name().find( m.first ) == std::string::npos){
            continue;
          }
          ATH_MSG_DEBUG( " +++ " << HLT::Identifier( id ) );

          dmap[id].push_back( TrigCompositeUtils::decisionToElementLink(decision, context) );
        }
      }
    }
  }

  if (msgLvl(MSG::DEBUG)){
    ATH_MSG_DEBUG( "Decision map filled :" );
    size_t legCount = 0;
    for (const auto& entry: dmap){
      ATH_MSG_DEBUG("leg ["<<legCount<<"]: ");
      const ElementLinkVector<DecisionContainer>& decisions = entry.second;
      ATH_MSG_DEBUG(" +++ " << HLT::Identifier( entry.first ) <<" Number Decisions: "<< decisions.size());
      for (const ElementLink<DecisionContainer> d : decisions){
        ATH_MSG_DEBUG("     Decision: (ContainerKey:"<<d.dataID()<<", DecisionElementIndex:"<<d.index()<<")");
      }
      legCount++;
    }
  }

  return StatusCode::SUCCESS;
}
