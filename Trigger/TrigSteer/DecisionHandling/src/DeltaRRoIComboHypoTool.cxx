#include "DeltaRRoIComboHypoTool.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include <algorithm>
#include <cmath>

using namespace TrigCompositeUtils;

DeltaRRoIComboHypoTool::DeltaRRoIComboHypoTool(const std::string& type,
					 const std::string& name,
					 const IInterface* parent)
  : IComboHypoTool( name ),
    AthAlgTool( type, name, parent )
{ }

DeltaRRoIComboHypoTool::~DeltaRRoIComboHypoTool(){}


StatusCode DeltaRRoIComboHypoTool::initialize() {
  ATH_MSG_DEBUG("DR threshold set to " << m_DRcut );
  return StatusCode::SUCCESS;
}

StatusCode DeltaRRoIComboHypoTool::finalize() {
  return StatusCode::SUCCESS;
}


StatusCode DeltaRRoIComboHypoTool::decide(const LegDecisionsMap & IDCombMap, LegDecisionsMap & passingCombinations ) const
{
  size_t nLegs=IDCombMap.size();
  ATH_MSG_DEBUG( "Looking for "<< m_decisionId <<" in the map. Map contains "<<nLegs<<" legs");
  if (passingCombinations[ m_decisionId].size() >0){
    ATH_MSG_ERROR("Passing combinations have "<< passingCombinations[ m_decisionId].size()<<" elements: what to do?");
    return StatusCode::FAILURE;
  }

  std::vector<ElementLinkVector<TrigCompositeUtils::DecisionContainer>> leg_decisions;// vector of decisions per leg, to combine
  // collect combinations from all the legs, searching both chain name and leg name
  for (auto id: IDCombMap){
    // get the Element links from the chainID (from all the legs)
    HLT::Identifier chainId=0;
    if (TrigCompositeUtils::isLegId(id.first))
      chainId= TrigCompositeUtils::getIDFromLeg(id.first);
    else
      chainId=id.first;
    if ( chainId != m_decisionId ) continue;    
    auto comb = id.second; 
    leg_decisions.push_back(comb);
  }

  if (nLegs != leg_decisions.size()){
    ATH_MSG_ERROR("Expecting "<<nLegs<<" legs, but found "<< leg_decisions.size() <<" legs to combine");
    return StatusCode::FAILURE;
  }
  
  ATH_MSG_DEBUG("Getting "<<leg_decisions.size()<<" legs to combine, for ID: "<< m_decisionId);
  for (auto leg: leg_decisions){
    ATH_MSG_DEBUG("Leg --");
    for (auto dEL:leg) ATH_MSG_DEBUG(dEL.dataID() << " , " << dEL.index());
  }

  // create the combinatios:
  // If we have one leg, the combination is between the elements in that leg;
  // if we have more leges, use recursive combinator
  std::vector<ElementLinkVector<TrigCompositeUtils::DecisionContainer>> combinations;
  createCombinations( leg_decisions, combinations,  nLegs, 2);
  for (auto comb: combinations){
    ATH_MSG_DEBUG("Combination");
    for (auto dEL :comb){
      ATH_MSG_DEBUG(dEL.dataID() << " , " << dEL.index());
    }
  }
    

  // do the actual algorithm:
  // to add: protection when the two decisions are the same object
  for (auto paircomb: combinations){
    std::vector<ElementLink<TrigRoiDescriptorCollection>> selected_rois;
    for (auto el: paircomb){
      auto dec= (*el);
      auto roiLink = TrigCompositeUtils::findLink<TrigRoiDescriptorCollection>( dec, initialRoIString() ).link;
      selected_rois.push_back(roiLink);
    }
    ATH_MSG_DEBUG("Selected RoIs: "<<selected_rois.size());
    
    //retrieve the rois and calcualte the Dr
    auto roiLink1=selected_rois[0];
    auto roiLink2=selected_rois[1];
    float Dr = deltaR((*roiLink1)->eta(), (*roiLink2)->eta(), (*roiLink1)->phi(), (*roiLink2)->phi());
    ATH_MSG_DEBUG("Found two RoIs with eta/phi " << (*roiLink1)->eta() <<"/"<<(*roiLink1)->phi() <<" and "<< (*roiLink2)->eta()<<"/"<<(*roiLink2)->phi() <<" with Dr="<<Dr);
    if (Dr <= m_DRcut){
      ATH_MSG_DEBUG( "  RoIs within DR<"<<m_DRcut<<". Insert elements for this combination: ");

      for (auto dEL: paircomb){
	auto decision= (*dEL);
	// get back the decID of this element
	auto allDec = decisionIDs( decision );
     	ATH_MSG_DEBUG( dEL.dataID() << " , " << dEL.index() <<" size ="<<allDec.size());
	for (auto id: allDec){
	  if ((HLT::Identifier(id).name()).find(m_decisionId.name())!=std::string::npos){
	    // we may have this element already stored, so there might be duplications
	    // add to both legID and chainID?
	    passingCombinations[id].push_back(dEL);
	    passingCombinations[ m_decisionId ].push_back(dEL);
	  }
	}
      }
    }
    
  }
  
  // remove duplicates?

  ATH_MSG_DEBUG("End of tool: Passing elments are: ");
  for  (auto id: passingCombinations){
    ATH_MSG_DEBUG("Id "<<HLT::Identifier(id.first)<<" with "<<id.second.size()<<" elements");
     for (auto dEL: id.second){
       ATH_MSG_DEBUG( dEL.dataID() << " , " << dEL.index());
     }
  }

  return StatusCode::SUCCESS;
}



double DeltaRRoIComboHypoTool::deltaR(double eta1, double eta2, double phi1, double phi2) const {
  double dPhi=std::remainder( phi1 - phi2, 2*M_PI );
  double dEta=std::fabs(eta1-eta2);
  return std::sqrt(dEta*dEta + dPhi*dPhi);
}

