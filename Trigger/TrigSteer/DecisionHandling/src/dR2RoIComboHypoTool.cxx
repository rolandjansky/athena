#include "dR2RoIComboHypoTool.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include <algorithm>
#include <cmath>

using namespace TrigCompositeUtils;

dR2RoIComboHypoTool::dR2RoIComboHypoTool(const std::string& type,
					 const std::string& name,
					 const IInterface* parent)
  : IComboHypoTool( name ),
    AthAlgTool( type, name, parent )
{ }

dR2RoIComboHypoTool::~dR2RoIComboHypoTool(){}


StatusCode dR2RoIComboHypoTool::initialize() {
  ATH_MSG_DEBUG("Initializing dR2RoIComboHypoTool");
  ATH_MSG_DEBUG("DR threshold set to " << m_DRcut );
  return StatusCode::SUCCESS;
}

StatusCode dR2RoIComboHypoTool::finalize() {
  return StatusCode::SUCCESS;
}


StatusCode dR2RoIComboHypoTool::decide( std::vector<TrigCompositeUtils::DecisionContainer>& decisions, const CombinationMap IDCombMap, std::map<TrigCompositeUtils::DecisionID, std::vector< std::pair<uint32_t,uint16_t> >> & passingCombinations ) const
{

  u_int nLegs=IDCombMap.size();
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
  
  if (nLegs ==1) return combineOneLeg(decisions, v_combinations[0], passingCombinations );
  else return combineMoreLegs(decisions, v_combinations, passingCombinations );

  return StatusCode::SUCCESS;
}



StatusCode dR2RoIComboHypoTool::combineMoreLegs(std::vector<TrigCompositeUtils::DecisionContainer>& decisions,
						std::vector<std::vector< std::pair<uint32_t,uint16_t>> > v_combinations,
						std::map<TrigCompositeUtils::DecisionID,
						std::vector< std::pair<uint32_t,uint16_t> >> & passingCombinations) const
{

  std::vector<std::pair<uint32_t,uint16_t>> local;
  std::vector<std::vector< std::pair<uint32_t,uint16_t>> > tocombine;
  recursive_combine(v_combinations, tocombine, local, 0);

  for (auto combpar: tocombine){
    ATH_MSG_DEBUG("Combination");
    for (auto comb:combpar){
      ATH_MSG_DEBUG(comb<<"");
    }
  }

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
//  passingCombinations[ m_decisionId]=newcomb;
//  ATH_MSG_DEBUG("End of tool: Passing combinations with "<< passingCombinations[ m_decisionId].size()<<" elements");
  ATH_MSG_DEBUG("End of tool: Passing combinations: ");
  for  (auto id: passingCombinations){
    ATH_MSG_DEBUG("Id "<<HLT::Identifier(id.first)<<" with "<<id.second.size()<<" elements");
  }
  return StatusCode::SUCCESS;
}
  




StatusCode dR2RoIComboHypoTool::combineOneLeg(std::vector<TrigCompositeUtils::DecisionContainer>& decisions,
					      std::vector< std::pair<uint32_t,uint16_t> > combinations,
					      std::map<TrigCompositeUtils::DecisionID,
					      std::vector< std::pair<uint32_t,uint16_t> >> & passingCombinations) const
{
  std::vector< std::pair<uint32_t,uint16_t> > newcomb; //better a set to esclude duplicates?

    // do all possible combinations: next_permutation + using the selector+selected
  std::vector<int> selector(combinations.size());
  int multiplicity = 2;// use two RoIs, get this from theconfiguration
  fill(selector.begin(), selector.begin() + multiplicity, 1);
  std::vector<int> selected;   
  do {//prev_permutation
    for (u_int i = 0; i < combinations.size(); i++) {
      if (selector[i]) {
	selected.push_back(i);
      }
    }
    
    // TODO: one can loop over the multiplicity required by the algorithm
    auto comb1=combinations[selected[0]];
    auto comb2=combinations[selected[1]];
    auto dec1 = decisions[comb1.first][comb1.second];
    auto dec2 = decisions[comb2.first][comb2.second];
    auto roiLink1 = TrigCompositeUtils::findLink<TrigRoiDescriptorCollection>( dec1, initialRoIString() ).link;
    auto roiLink2 = TrigCompositeUtils::findLink<TrigRoiDescriptorCollection>( dec2, initialRoIString() ).link;
    float Dr = deltaR((*roiLink1)->eta(), (*roiLink2)->eta(), (*roiLink1)->phi(), (*roiLink2)->phi());
    ATH_MSG_DEBUG("Found two RoIs with eta/phi " << (*roiLink1)->eta() <<"/"<<(*roiLink1)->phi() <<" and "<< (*roiLink2)->eta()<<"/"<<(*roiLink2)->phi() <<" with Dr="<<Dr);
    if (Dr <= m_DRcut){
      ATH_MSG_DEBUG( "  RoIs within DR<"<<m_DRcut<<". Insert combinations: "<< comb1<<" "<<comb2);
      //if(std::find(newcomb.begin(), newcomb.end(), comb1) == newcomb.end())
      newcomb.push_back(comb1);
      //if(std::find(newcomb.begin(), newcomb.end(), comb2) == newcomb.end())
      newcomb.push_back(comb2);
    }
    selected.clear();
  }  while (std::prev_permutation(selector.begin(), selector.end()));
  
  // remove duplicates:
  std::sort( newcomb.begin(), newcomb.end() );
  newcomb.resize(std::distance(newcomb.begin(), std::unique( newcomb.begin(), newcomb.end() ))); 
  passingCombinations[ m_decisionId]=newcomb;
  ATH_MSG_DEBUG("End of tool: Passing combinations with "<< passingCombinations[ m_decisionId].size()<<" elements");
    
  return StatusCode::SUCCESS;
}
  



double dR2RoIComboHypoTool::deltaR(double eta1, double eta2, double phi1, double phi2) const {
  double dPhi=std::remainder( phi1 - phi2, 2*M_PI );
  double dEta=std::fabs(eta1-eta2);
  return std::sqrt(dEta*dEta + dPhi*dPhi);
}


void dR2RoIComboHypoTool::recursive_combine(std::vector<std::vector< std::pair<uint32_t,uint16_t>>> &all,
					    std::vector<std::vector< std::pair<uint32_t,uint16_t> >> & tocombine,
					    std::vector<std::pair<uint32_t,uint16_t>>& local, u_int lindex)  const
{
  
  for (size_t leg =lindex; leg<all.size(); leg++){
    for (size_t i=0; i<all[leg].size(); i++){
      local.push_back(all[leg][i]);
      recursive_combine(all, tocombine,local, leg+1);
      local.pop_back(); 
    }
  }
  if (local.size() == all.size())
    tocombine.push_back(local);
 
 return;
 
}
