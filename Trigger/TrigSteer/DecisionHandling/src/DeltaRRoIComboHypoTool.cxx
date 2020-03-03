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


StatusCode DeltaRRoIComboHypoTool::decide( const std::vector<TrigCompositeUtils::DecisionContainer>& decisions,
					const CombinationMap & IDCombMap,
					std::map<TrigCompositeUtils::DecisionID, std::vector< std::pair<uint32_t,uint16_t> >> & passingCombinations ) const
{

  size_t nLegs=IDCombMap.size();
  ATH_MSG_DEBUG( "Looking for "<< m_decisionId <<" in the map. Map contains "<<nLegs<<" legs");
  if (passingCombinations[ m_decisionId].size() >0){
    ATH_MSG_ERROR("Passing combinations have "<< passingCombinations[ m_decisionId].size()<<" elements: what to do?");
    return StatusCode::FAILURE;
  }
  
  std::vector<std::vector< std::pair<uint32_t,uint16_t> >> v_combinations;// vector of combinaitons per leg
  // collect combinations from all the legs
  for (auto id: IDCombMap){
    // get th ecorresponding chian ID combinations
    HLT::Identifier chainId=0;
    if (TrigCompositeUtils::isLegId(id.first))
      chainId= TrigCompositeUtils::getIDFromLeg(id.first);
    else
      chainId=id.first;
    if ( chainId != m_decisionId ) continue;    
    ComboHypoCombination comb = id.second;
    v_combinations.push_back(comb.getCombinations());
  }

  if (nLegs != v_combinations.size()){
    ATH_MSG_ERROR("Expecting "<<nLegs<<" legs, but found "<< v_combinations.size() <<" legs to combine");
    return StatusCode::FAILURE;
  }
  
  ATH_MSG_DEBUG("Getting "<<v_combinations.size()<<" legs to combine, for ID: "<< m_decisionId);
  for (auto combinations: v_combinations){
    ATH_MSG_DEBUG("Leg --");
    for (auto comb:combinations) ATH_MSG_DEBUG(comb<<"");
  }

  // create the combinatios:
  // If we have one leg, the combination is between the elements in that leg;
  // if we have more leges, use recursive combinator
  std::vector<std::vector< std::pair<uint32_t,uint16_t>> > tocombine;
  createCombinations( v_combinations, tocombine,  nLegs, 2);
  for (auto combpar: tocombine){
    ATH_MSG_DEBUG("Combination");
    for (auto comb:combpar){
      ATH_MSG_DEBUG(comb<<"");
    }
  }
    

  // do the actual algorithm:
  // to add: protection when the two decisions are the same object
  
  std::vector< std::pair<uint32_t,uint16_t> > newcomb; //better a set?
  for (auto paircomb: tocombine){
    std::vector<ElementLink<TrigRoiDescriptorCollection>> selected_rois;
    for (auto comb: paircomb){
      auto dec=decisions[comb.first][comb.second];
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
      ATH_MSG_DEBUG( "  RoIs within DR<"<<m_DRcut<<". Insert combinations: ");
       for (auto comb: paircomb){
     	ATH_MSG_DEBUG( comb);	
     	newcomb.push_back(comb);
       }
    }
    
  }
  
  // remove duplicates:
  std::sort( newcomb.begin(), newcomb.end() );
  newcomb.resize(std::distance(newcomb.begin(), std::unique( newcomb.begin(), newcomb.end() )));

  // recreating the legname, since it was lost.... may do it better with other classes
  for (auto el: newcomb){   
    auto id=TrigCompositeUtils::createLegName(m_decisionId, el.first);
    passingCombinations[id].push_back(el);
  }

  ATH_MSG_DEBUG("End of tool: Passing combinations: ");
  for  (auto id: passingCombinations){
    ATH_MSG_DEBUG("Id "<<HLT::Identifier(id.first)<<" with "<<id.second.size()<<" elements");
  }

  return StatusCode::SUCCESS;
}



double DeltaRRoIComboHypoTool::deltaR(double eta1, double eta2, double phi1, double phi2) const {
  double dPhi=std::remainder( phi1 - phi2, 2*M_PI );
  double dEta=std::fabs(eta1-eta2);
  return std::sqrt(dEta*dEta + dPhi*dPhi);
}

