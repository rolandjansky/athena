/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "./JetGroupUnion.h"
#include <set>
#include <string>
#include <sstream>


JetGroupUnion::JetGroupUnion(const std::vector<std::size_t>& siblings,
			     const CondInd2JetGroupsInds& satisfiedBy,
			     const  JetGroupInd2ElemInds& jg2elemjgs,
			     const Collector&) {
  
  // make a vector of the jet indices that satisfy the sibling Conditions
  // with no duplicates

  std::set<std::size_t> j_elem_indices;
  for(const auto& is : siblings) {
    const auto& indices = satisfiedBy.at(is);
    for (const auto& i : indices){
      j_elem_indices.insert(jg2elemjgs.at(i).begin(),
			    jg2elemjgs.at(i).end());
    }
  }
  
  m_jetIndices.assign(j_elem_indices.begin(), j_elem_indices.end());
  if (m_jetIndices.empty()) {m_done = true;}
}
  
std::vector<std::size_t> JetGroupUnion::next(const Collector& collector){
  if(collector){
    std::stringstream sstr;
    sstr << "no of indices " << m_jetIndices.size()
	 << " done " << std::boolalpha << m_done;
    collector->collect("JetGroupUnion::next()", sstr.str());
  }
  
  if (m_done) {
    return std::vector<std::size_t>();
  }

  m_done = true;
  return m_jetIndices;
}
