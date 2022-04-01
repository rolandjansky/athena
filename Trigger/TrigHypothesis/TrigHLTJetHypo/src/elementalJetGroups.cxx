/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "./elementalJetGroups.h"
#include <algorithm>

std::vector<std::size_t>
elementalJetGroups(const std::vector<std::size_t>& non_elemental,
		   const JetGroupInd2ElemInds& jg2elemjg) {

  std::vector<std::size_t> elem_indices;
  
  for(auto i : non_elemental){
    elem_indices.insert(elem_indices.end(),
			jg2elemjg.at(i).begin(),
			jg2elemjg.at(i).end());
  }
  
  // if any of the elemental jet group indices are repeated,
  // stop processing the new jet group. (do not allow sharing for
  // among Conditions. Sharing is handled by having the use of
  // multiple FastReducer instances).
  
  std::sort(elem_indices.begin(), elem_indices.end());
  if (std::unique(elem_indices.begin(),
		  elem_indices.end()) != elem_indices.end()){
    elem_indices.clear();
  }
  return elem_indices;
}
