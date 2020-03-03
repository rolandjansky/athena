#include "IComboHypoTool.h"


void IComboHypoTool::recursive_combine(const std::vector<ElementLinkVector<TrigCompositeUtils::DecisionContainer>> &all,
			 std::vector<ElementLinkVector<TrigCompositeUtils::DecisionContainer>> & tocombine,
			 ElementLinkVector<TrigCompositeUtils::DecisionContainer> & local, u_int lindex)  const
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

void IComboHypoTool::createCombinations(const std::vector<ElementLinkVector<TrigCompositeUtils::DecisionContainer>> & v_combinations,
			  std::vector<ElementLinkVector<TrigCompositeUtils::DecisionContainer>> & tocombine, int nLegs, int nToGroup) const
{

  if (nLegs ==1) {
    auto combinations = v_combinations[0];
    std::vector<int> selector(combinations.size());
    //    int nRoIsToCombine = 2;// use two RoIs, get this from theconfiguration?
    fill(selector.begin(), selector.begin() + nToGroup, 1);
    ElementLinkVector<TrigCompositeUtils::DecisionContainer> thecomb;
    do {
      for (u_int i = 0; i < combinations.size(); i++) {
	if (selector[i]) {
	  thecomb.push_back(combinations[i]);
	}
      }
      tocombine.push_back(thecomb);    
      thecomb.clear();
    }  while (std::prev_permutation(selector.begin(), selector.end()));
  }

  else {
    ElementLinkVector<TrigCompositeUtils::DecisionContainer> local;
    recursive_combine(v_combinations, tocombine, local, 0);
  }


  return;
}
