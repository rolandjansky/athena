/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "./JetGroupUnion.h"
#include <set>
#include <string>
#include <sstream>


JetGroupUnion::JetGroupUnion(const std::vector<std::size_t>& siblings,
			     const std::vector<bool>& leaves,
			     const CondInd2JetGroupsInds& satisfiedBy,
			     const  JetGroupInd2ElemInds& jg2elemjgs,
			     std::size_t n_required){
  
  // make a vector of the jet indices that satisfy the sibling Conditions
  // with no duplicates

  auto s_iter = siblings.cbegin();
  auto s_end = siblings.end();
  auto l_iter = leaves.cbegin();

  // collect all the passing jets into a vector
  for (;s_iter != s_end; ++s_iter, ++l_iter){
    const auto& passing_jets = satisfiedBy.at(*s_iter);
    if (*l_iter){
      // want elementary jet groups. If sibling is a leaf node,
      // its jet groups will be elementary...
      m_jetIndices.insert(m_jetIndices.end(),
			  passing_jets.begin(),
			  passing_jets.end());
    } else {
      //... otherwise, need to obtain the elementary jet groups
      // from a compound jet group, and add these to the vector
      for (const auto& pj : passing_jets) {
	m_jetIndices.insert(m_jetIndices.end(),
			    jg2elemjgs.at(pj).cbegin(),
			    jg2elemjgs.at(pj).cend());
      }
    }

    // remove  duplicates, and resize the vector
    // to contain only what remains
    std::sort(m_jetIndices.begin(), m_jetIndices.end());
    auto iter = std::unique(m_jetIndices.begin(),
			    m_jetIndices.end());
    m_jetIndices.resize(iter - m_jetIndices.begin());
  }

  // are there enough jets to  the min jet requirement?
  if (m_jetIndices.size() < n_required) {m_done = true;}
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
