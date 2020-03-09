/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "ComboHypo.h"
#include "DecisionHandling/TrigCompositeUtils.h"
#include "DecisionHandling/HLTIdentifier.h"
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
  
  // find max inputs size
  auto maxMultEl = std::max_element( m_multiplicitiesReqMap.begin(), m_multiplicitiesReqMap.end(),  
    []( MultiplicityReqMap::value_type a, MultiplicityReqMap::value_type b ){ return a.second.size() < b.second.size(); }
    ); 
  
  const size_t maxMult = maxMultEl->second.size();

  if (msgLvl(MSG::INFO)){ 
    ATH_MSG_INFO( "with this multiplicity map: ");
    for ( const auto& m : m_multiplicitiesReqMap ) {
      std::stringstream msgSS, mmsSS;
      msgSS << "[";
      mmsSS << "[";
      for (const int mult : m.second){
        msgSS << mult << ", ";
        mmsSS << std::to_string(mult) << ", ";
      }
      msgSS << "]";
      mmsSS << "]";
      ATH_MSG_INFO("-- " << m.first << " multiplicities: " << msgSS.str() << " -> " << mmsSS.str());
    }
  }

  ATH_CHECK( m_multiplicitiesReqMap.size() != 0 );

  bool errorOccured = false;
  for ( const auto& m : m_multiplicitiesReqMap ) {
    if ( m.second.size() != maxMult )  {
      errorOccured =  true;
      ATH_MSG_ERROR( "Chain " << m.first 
        << " configured with input multiplicity " << m.second.size() << " like this: " <<  m.second 
        << " which is lower than for this chain " << maxMultEl->first <<  " " << maxMult);
    }
  }

  return ( errorOccured ? StatusCode::FAILURE : StatusCode::SUCCESS );
}


StatusCode ComboHypo::finalize() {
  return StatusCode::SUCCESS;
}


StatusCode ComboHypo::copyDecisions( const DecisionIDContainer& passing, const EventContext& context ) const {

  ATH_MSG_DEBUG( "Copying "<<passing.size()<<" positive decisions to outputs");
  for ( size_t input_counter = 0; input_counter < m_inputs.size(); ++input_counter ) {

    // new output decisions
    SG::WriteHandle<DecisionContainer> outputHandle = createAndStore(m_outputs.at(input_counter), context ); 
    auto outDecisions = outputHandle.ptr();    

    auto inputHandle = SG::makeHandle( m_inputs.at(input_counter), context );
    if ( inputHandle.isValid() ) {
      
      for (const Decision* inputDecision : *inputHandle) {

        DecisionIDContainer inputDecisionIDs;
        decisionIDs( inputDecision, inputDecisionIDs );

        // from all positive decision in the input only the ones that survived counting are passed over
        DecisionIDContainer common;      
        std::set_intersection( inputDecisionIDs.begin(), inputDecisionIDs.end(), passing.begin(), passing.end(),
          std::inserter( common, common.end() ) );

        // Propagate chain names as well as leg names
        DecisionIDContainer finalIds;   
        for (const DecisionID c : common){
          const HLT::Identifier cID = HLT::Identifier(c);
          finalIds.insert( cID.numeric() ); // all Ids used by the Filter, including legs
          if (TrigCompositeUtils::isLegId ( cID )){
            const HLT::Identifier mainChain = TrigCompositeUtils::getIDFromLeg( cID );
            finalIds.insert( mainChain.numeric() );
          }
        }

        Decision* newDec = newDecisionIn( outDecisions );
        linkToPrevious( newDec, inputDecision, context );
        ATH_MSG_DEBUG("New decision (Container Index:" << input_counter << ", Element Index:"<< newDec->index() <<") has "
          << (TrigCompositeUtils::findLink<TrigRoiDescriptorCollection>(newDec, initialRoIString())).isValid()
          << " valid initialRoI and "<< TrigCompositeUtils::getLinkToPrevious(newDec).size() <<" previous decisions; valid Ids="<<finalIds.size()) ;   

        insertDecisionIDs( finalIds, newDec );
      }
    }

    if (msgLvl(MSG::DEBUG)) {
      ATH_MSG_DEBUG("Output Handle " << m_outputs.at(input_counter).key() << " with " << outputHandle->size() <<" Decision objects");
      for (const Decision* d : *outputHandle){
        DecisionIDContainer objDecisions;      
        decisionIDs( d, objDecisions );
        ATH_MSG_DEBUG("Number of positive decisions for Decision object #" << d->index() << ": " << objDecisions.size());
        for (const TrigCompositeUtils::DecisionID id : objDecisions ) {
          ATH_MSG_DEBUG(" --- Passes: " << HLT::Identifier( id ));
        }  
      }
    }
  }

  return StatusCode::SUCCESS;
}


StatusCode ComboHypo::execute(const EventContext& context ) const {
  ATH_MSG_DEBUG( "Executing " << name() << "..." );
  
  DecisionIDContainer passing;
  // this map is filled with the count of positive decisions from each input
  CombinationMap dmap;

  ATH_CHECK( fillDecisionsMap( dmap, context ) );

  for ( const auto& m : m_multiplicitiesReqMap ) {
    uint32_t nRequiredUnique = 0;

    const HLT::Identifier chain = HLT::Identifier(m.first);
    const std::vector<int>& multiplicityPerLeg = m.second;
    const DecisionID requiredDecisionID = chain.numeric();

    DecisionIDContainer allDecisionIds;
    allDecisionIds.insert(requiredDecisionID);

    bool overallDecision = true;
    DecisionIDContainer uniqueDecisions;

    // Check multiplicity of each leg 
    for ( size_t legIndex = 0; legIndex <  multiplicityPerLeg.size(); ++legIndex ) {
      const size_t requiredMultiplicity =  multiplicityPerLeg.at( legIndex );
      nRequiredUnique += requiredMultiplicity;

      HLT::Identifier legId = TrigCompositeUtils::createLegName(chain, legIndex);

      // If there is only one leg, then we just use the chain's name.
      if (multiplicityPerLeg.size() == 1) {
        ATH_MSG_DEBUG(chain << " has multiplicityPerLeg.size() == 1, so we don't use legXXX_HLT_YYY, we just use HLT_YYY");
        legId = chain; 
      }

      const DecisionID requiredDecisionIDLeg = legId.numeric();
      ATH_MSG_DEBUG("Container " << legIndex << ", looking at leg : " << legId );
     
      CombinationMap::const_iterator it = dmap.find(requiredDecisionIDLeg);
      if ( it == dmap.end() ) {
        overallDecision = false;
        break;
      }

      //check this leg of the chain passes with required multiplicity

      const size_t observedMultiplicity = it->second.getCombinations().size();
      ATH_MSG_DEBUG( "Required multiplicity " << requiredMultiplicity  << " for leg " << legId 
        << ": observed multiplicity " << observedMultiplicity << " in leg " << legIndex  );

      if ( observedMultiplicity < requiredMultiplicity ) {
        overallDecision = false;
        break;
      }

      //keep track of the number of unique features

      for (const auto& entry : it->second.getFeatures()){
        if (entry.first == 0) {
          ATH_MSG_WARNING("Disregarding feature hash of zero");
          continue;
        }
        uniqueDecisions.insert( entry.first );
        // TODO - do something with entry.second (the ROI)
      }

      allDecisionIds.insert(requiredDecisionIDLeg);
    }

    //check that the multiplicity of unique features is high enough
    ATH_MSG_DEBUG("Number of unique decisions: " << uniqueDecisions.size() << ", number of required unique decisions: " << nRequiredUnique);
    if ( uniqueDecisions.size() < nRequiredUnique ) {
      overallDecision = false;
    }

    //Overall chain decision
    ATH_MSG_DEBUG( "Chain " << chain <<  ( overallDecision ? " is accepted" : " is rejected") );
    if ( overallDecision == true ) {
      for (auto decID: allDecisionIds) {
        passing.insert( passing.end(), decID );
        ATH_MSG_DEBUG("  Passing " << HLT::Identifier(decID));
      }
    }      
  }

  ATH_CHECK( copyDecisions( passing, context ) );
  
  return StatusCode::SUCCESS;
}

StatusCode ComboHypo::extractFeatureAndRoI(const Decision* d, const std::string& input,
  uint32_t& featureKey, uint16_t& featureIndex, uint32_t& roiKey, uint16_t& roiIndex) const 
{
  uint32_t featureClid = 0; // Note: Unused. We don't care what the type of the feature is here
  const bool result = d->typelessGetObjectLink(featureString(), featureKey, featureClid, featureIndex);
  if (!result) {
    ATH_MSG_ERROR("Did not find the feature for Input:" << input << " Element:" << d->index());
  }
  // Try and get seeding ROI data too. Don't need to be type-less here
  if (m_requireUniqueROI) {
    LinkInfo<TrigRoiDescriptorCollection> roiSeedLI = findLink<TrigRoiDescriptorCollection>(d, initialRoIString());
    if (roiSeedLI.isValid()) {
      roiKey = roiSeedLI.link.key();
      roiIndex = roiSeedLI.link.index();
    }
    else {
      ATH_MSG_ERROR("Did not find a seeding ROI for Input:" << input << " Element:" << d->index());
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode ComboHypo::fillDecisionsMap( CombinationMap &  dmap, const EventContext& context) const {

  for ( size_t inputContainerIndex = 0; inputContainerIndex < m_inputs.size(); ++inputContainerIndex ) {   
    auto inputHandle = SG::makeHandle( m_inputs.at(inputContainerIndex), context );
    if ( !inputHandle.isValid() ) {
      ATH_MSG_ERROR( "No input ReadHandle from " << inputHandle.key() );
      return StatusCode::FAILURE;
    }
    ATH_MSG_DEBUG( "Found ReadHandle from " << inputHandle.key() <<" with "<< inputHandle->size() << " elements:"  );
    for ( const Decision* decision : *inputHandle ) {
      ATH_MSG_DEBUG( "Input Decision #"<< decision->index() <<" with "<< decisionIDs( decision ).size() << " active IDs" );
      for ( const DecisionID id: decisionIDs( decision ) ) {
        for ( const auto& m : m_multiplicitiesReqMap ) {
          // Search for this ID in the list of active chains processed by this ComboHypo
          // Note: We do a find() check rather than require equality as m_multiplicitiesReqMap is configured by chain name
          // whereas id is here the per-leg name. The chain name is a sub-string of the leg name.
          if (HLT::Identifier(id).name().find( m.first ) == std::string::npos){
            continue;
          }
          ATH_MSG_DEBUG( " +++ " << HLT::Identifier( id ) );

          // Obtain unique but type-less key & index identifiers for this Decision node's "Feature" 
          // and (if m_requireUniqueROI) its initialRoI.
          // Allows us to check object uniqueness and prevent multiple objects from the same RoI satisfying one leg.
          uint32_t featureKey = 0, roiKey = 0;
          uint16_t featureIndex = 0, roiIndex = 0;
          ATH_CHECK( extractFeatureAndRoI(decision, inputHandle.key(), featureKey, featureIndex, roiKey, roiIndex) );

          dmap[id].add(inputContainerIndex, decision->index(), featureKey, featureIndex, roiKey, roiIndex);
        }
      }
    }
  }

  if (msgLvl(MSG::DEBUG)){
    ATH_MSG_DEBUG( "Decision map filled :" );
    size_t legCount = 0;
    for (const auto& entry: dmap){
      ATH_MSG_DEBUG("leg ["<<legCount<<"]: ");
      const std::vector<std::pair<uint32_t,uint16_t>>& combinations = entry.second.getCombinations();
      ATH_MSG_DEBUG(" +++ " << HLT::Identifier( entry.first ) <<" mult: "<< combinations.size());
      for (const auto& comb : combinations){
        ATH_MSG_DEBUG("     Comb: (ContainerIndex:"<<comb.first<<", DecisionElementIndex:"<<comb.second<<")");
      }
      const std::vector<std::pair<uint32_t,uint32_t>>& featureMap = entry.second.getFeatures();
      ATH_MSG_DEBUG("FeatureMap: found " << featureMap.size() << " entries");
      for (const auto& feat : featureMap) {
        ATH_MSG_DEBUG("    Unique Feature Identifier:" << feat.first << ", From ROI Identifier: " << feat.second);
      }
      legCount++;
    }
  }

  return StatusCode::SUCCESS;
}
