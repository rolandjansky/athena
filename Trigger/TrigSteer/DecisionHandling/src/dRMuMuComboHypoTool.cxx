#include "dRMuMuComboHypoTool.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include <algorithm>
#include <cmath>

using namespace TrigCompositeUtils;

dRMuMuComboHypoTool::dRMuMuComboHypoTool(const std::string& type,
					 const std::string& name,
					 const IInterface* parent)
  : IComboHypoTool( name ),
    AthAlgTool( type, name, parent )
{ }

dRMuMuComboHypoTool::~dRMuMuComboHypoTool(){}


StatusCode dRMuMuComboHypoTool::initialize() {
  ATH_MSG_DEBUG("Initializing dRMuMuComboHypoTool");
  ATH_MSG_DEBUG("DR threshold set to " << m_DRcut );
  return StatusCode::SUCCESS;
}

StatusCode dRMuMuComboHypoTool::finalize() {
  return StatusCode::SUCCESS;
}


StatusCode dRMuMuComboHypoTool::decide( std::vector<TrigCompositeUtils::DecisionContainer>& decisions, const CombinationMap IDCombMap, std::map<TrigCompositeUtils::DecisionID, std::vector< std::pair<uint32_t,uint16_t> >> & passingCombinations ) const {


  std::vector<xAOD::TrigComposite> TCdecisions_to_combine; // can use Decision?
  TrigCompositeUtils::DecisionContainer decisions_to_combine; //DataVector does not have BidirectionalIterator, so cannot use next_permutation
  ElementLinkVector<DecisionContainer> ELdecisions_to_combine;
  ATH_MSG_DEBUG( "Looking for "<< m_decisionId <<" in the map");
  if (passingCombinations[ m_decisionId].size() >0)
    ATH_MSG_ERROR("Passing combinations has "<< passingCombinations[ m_decisionId].size()<<" elements: what to do?");
  
  std::vector< std::pair<uint32_t,uint16_t> > newcomb; //better a set?
		  
  for (auto id: IDCombMap){
    if ( HLT::Identifier( id.first ) != m_decisionId ) continue;    
    ComboHypoCombination comb = id.second;
    auto combinations = comb.getCombinations();
    ATH_MSG_DEBUG("Getting "<<combinations.size()<<" combinations for ID: "<< m_decisionId);
    for (auto comb:combinations) ATH_MSG_DEBUG(comb<<"");

    // do permutations: next_permutation + using the selector+selected
    std::vector<int> selector(combinations.size());
    fill(selector.begin(), selector.begin() + 2, 1);// use two muons
    std::vector<int> selected;   
    do {
      for (u_int i = 0; i < combinations.size(); i++) {
	if (selector[i]) {
	  selected.push_back(i);
	}
      }
      auto comb1=combinations[selected[0]];
      auto comb2=combinations[selected[1]];
      auto dec1 = decisions[comb1.first][comb1.second];
      auto dec2 = decisions[comb2.first][comb2.second];
      auto muonLink1 = TrigCompositeUtils::findLink<TrigRoiDescriptorCollection>( dec1, initialRoIString() ).link;
      // ElementLink<xAOD::TrigComposite> muonLink0 = dec0->objectLink<xAOD::TrigComposite>(featureString());
      auto muonLink2 = TrigCompositeUtils::findLink<TrigRoiDescriptorCollection>( dec2, initialRoIString() ).link;
      // ElementLink<xAOD::TrigComposite> muonLink1 = dec1->objectLink<xAOD::TrigComposite>(featureString());
      float Dr = deltaR((*muonLink1)->eta(), (*muonLink2)->eta(), (*muonLink1)->phi(), (*muonLink2)->phi());
      ATH_MSG_DEBUG("Found two muons with eta/phi " << (*muonLink1)->eta() <<"/"<<(*muonLink1)->phi() <<" and "<< (*muonLink2)->eta()<<"/"<<(*muonLink2)->phi() <<" with Dr="<<Dr);
      if (Dr < m_DRcut){
	ATH_MSG_DEBUG( "Found two muons within DR<"<<m_DRcut<<". Insert combinations: "<< comb1<<" "<<comb2);
	//if(std::find(newcomb.begin(), newcomb.end(), comb1) == newcomb.end())
	  newcomb.push_back(comb1);
	  //if(std::find(newcomb.begin(), newcomb.end(), comb2) == newcomb.end())
	  newcomb.push_back(comb2);
      }
      selected.clear();
    }  while (std::prev_permutation(selector.begin(), selector.end()));
  }
  // remove duplicates:
  std::sort( newcomb.begin(), newcomb.end() );
  newcomb.resize(std::distance(newcomb.begin(), std::unique( newcomb.begin(), newcomb.end() ))); 
  passingCombinations[ m_decisionId]=newcomb;
  ATH_MSG_DEBUG("End of tool: Passing combinations with "<< passingCombinations[ m_decisionId].size()<<" elements");
    
  return StatusCode::SUCCESS;
}
  


double dRMuMuComboHypoTool::deltaR(double eta1, double eta2, double phi1, double phi2) const {
  double dPhi=std::remainder( phi1 - phi2, 2*M_PI );
  double dEta=std::fabs(eta1-eta2);
  return std::sqrt(dEta*dEta + dPhi*dPhi);
}
