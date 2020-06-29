/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
	  // add teh decID only if this candidate passed the combination selection
	  const ElementLinkVector<DecisionContainer>& Comb=passingLegs.at(c);
	  if(std::find(Comb.begin(), Comb.end(), thisEL) == Comb.end()) continue;

	  ATH_MSG_DEBUG("  Adding "<< cID <<" because EL is found in the passingLegs map");
          finalIds.insert( cID.numeric() ); // all Ids used by the Filter, including legs
          if (TrigCompositeUtils::isLegId ( cID )){
            const HLT::Identifier mainChain = TrigCompositeUtils::getIDFromLeg( cID );
            finalIds.insert( mainChain.numeric() );
	    ATH_MSG_DEBUG("  Adding "<< mainChain <<" consequently");
          }
        }

        Decision* newDec = newDecisionIn( outDecisions );
        linkToPrevious( newDec, inputDecision, context );
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
    uint32_t nRequiredUnique = 0;
    const HLT::Identifier chainId = HLT::Identifier(m.first);
    const std::vector<int>& multiplicityPerLeg = m.second;
    const DecisionID requiredDecisionID = chainId.numeric();

    DecisionIDContainer allDecisionIds;
    allDecisionIds.insert(requiredDecisionID);

    bool overallDecision = true;

    std::set<uint32_t> uniqueDecisionFeatures;
    LegDecisionsMap thisChainCombMap;


    // Check multiplicity of each leg of this chain
    for ( size_t legIndex = 0; legIndex <  multiplicityPerLeg.size(); ++legIndex ) {
      const size_t requiredMultiplicity =  multiplicityPerLeg.at( legIndex );
      nRequiredUnique += requiredMultiplicity;

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

      //check this leg of the chain passes with required multiplicity
      const size_t observedMultiplicity = it->second.size();

      ATH_MSG_DEBUG( "Required multiplicity " << requiredMultiplicity  << " for leg " << legId 
        << ": observed multiplicity " << observedMultiplicity << " in leg " << legIndex  );

      if ( observedMultiplicity < requiredMultiplicity ) {
        overallDecision = false;
        break;
      }
  
      //keep track of the number of unique features/rois
      for (const ElementLink<DecisionContainer>& dEL : it->second){
        uint32_t featureKey = 0, roiKey = 0;
        uint16_t featureIndex = 0, roiIndex = 0;
        // NOTE: roiKey, roiIndex are only currently used in the discrimination for L1 Decision objects (which don't have a 'feature' link)
        // NOTE: We should make it configurable to choose either the feature or the ROI here, as done in the InputMaker base class when merging.
        ATH_CHECK( extractFeatureAndRoI(dEL, featureKey, featureIndex, roiKey, roiIndex) );
        const uint32_t uniquenessHash = (featureKey != 0 ? (featureKey + featureIndex) : (roiKey + roiIndex)); 
        if (uniquenessHash == 0) {
          ATH_MSG_ERROR("Object has no feature, and no initialRoI. Cannot get obtain unique element to avoid double-counting.");
          return StatusCode::FAILURE;
        }
        uniqueDecisionFeatures.insert( uniquenessHash );
      }

      // save combinations of all legs for the tools
      thisChainCombMap.insert (*it);
      allDecisionIds.insert(requiredDecisionIDLeg);
    }

    //check that the multiplicity of unique features is high enough
    ATH_MSG_DEBUG("Number of unique features: " << uniqueDecisionFeatures.size() << ", number of required unique decisions: " << nRequiredUnique);
    if ( uniqueDecisionFeatures.size() < nRequiredUnique ) {
      overallDecision = false;
    }

    //Overall chain decision
    ATH_MSG_DEBUG( "Chain " << chainId <<  ( overallDecision ? " is accepted" : " is rejected")  <<" after multiplicity requirements" );
    if ( overallDecision == true ) {
      for (auto decID: allDecisionIds) {
	// saving the good combiantions
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
  uint32_t& featureKey, uint16_t& featureIndex, uint32_t& roiKey, uint16_t& roiIndex) const 
{
  uint32_t featureClid = 0; // Note: Unused. We don't care what the type of the feature is here
  const bool result = (*dEL)->typelessGetObjectLink(featureString(), featureKey, featureClid, featureIndex);
  // Try and get seeding ROI data too. Don't need to be type-less here
  LinkInfo<TrigRoiDescriptorCollection> roiSeedLI = findLink<TrigRoiDescriptorCollection>((*dEL), initialRoIString());
  if (roiSeedLI.isValid()) {
    roiKey = roiSeedLI.link.key();
    roiIndex = roiSeedLI.link.index();
  }
  if (!result && !roiSeedLI.isValid()) {
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
      for (const ElementLink<DecisionContainer>& d : decisions){
        ATH_MSG_DEBUG("     Decision: (ContainerKey:"<<d.dataID()<<", DecisionElementIndex:"<<d.index()<<")");
      }
      legCount++;
    }
  }

  return StatusCode::SUCCESS;
}
