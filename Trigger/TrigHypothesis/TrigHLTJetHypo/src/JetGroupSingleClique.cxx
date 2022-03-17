/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "./JetGroupSingleClique.h"
#include "./JetStreamer.h"

#include <set>
#include <string>

JetGroupSingleClique::JetGroupSingleClique(const std::vector<std::size_t>& satisfyingJetGroups,
					   const JetGroupInd2ElemInds& jg2elemjgs
					   ) {

  // make a vector of the jet indices that satisfy a sibling Condition
  // All siblings are of the same clique, only one passed here.
  
  std::set<std::size_t> j_elem_indices;
  for (const auto& i : satisfyingJetGroups){
    j_elem_indices.insert(jg2elemjgs.at(i).begin(),
			  jg2elemjgs.at(i).end());
  }

  m_jetIndices.assign(j_elem_indices.cbegin(), j_elem_indices.cend());
  
  if (m_jetIndices.empty()) {m_done = true;}
}
  

std::vector<std::size_t> JetGroupSingleClique::next(const Collector& collector){
  if(collector){
    std::stringstream sstr;
    sstr << "no of indices " << m_jetIndices.size()
	 << " done " << std::boolalpha << m_done;
    collector->collect("JetGroupSingleClique::next()", sstr.str());
  }
  
  if (m_done) {
    return std::vector<std::size_t>();
  }

  m_done = true;
  return m_jetIndices;
}
