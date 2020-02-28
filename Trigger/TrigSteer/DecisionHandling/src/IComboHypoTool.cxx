#include "IComboHypoTool.h"


void IComboHypoTool::recursive_combine(const std::vector<std::vector< std::pair<uint32_t,uint16_t>>> &all,
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

void IComboHypoTool::createCombinations( const  std::vector<std::vector< std::pair<uint32_t,uint16_t> >> & v_combinations,
					 std::vector<std::vector< std::pair<uint32_t,uint16_t>> >& tocombine, int nLegs, int nToGruop) const
{

  if (nLegs ==1) {
    auto combinations = v_combinations[0];
    std::vector<int> selector(combinations.size());
    //    int nRoIsToCombine = 2;// use two RoIs, get this from theconfiguration?
    fill(selector.begin(), selector.begin() + nToGruop, 1);
    std::vector < std::pair<uint32_t,uint16_t>> thecomb;
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
    std::vector<std::pair<uint32_t,uint16_t>> local;
    recursive_combine(v_combinations, tocombine, local, 0);
  }


  return;
}
