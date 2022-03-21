/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "./JetGroupSingleClique.h"
#include <set>
#include <string>
#include <sstream>


JetGroupSingleClique::JetGroupSingleClique(const std::vector<std::size_t>& satisfyingJets,
					   std::size_t n_required){
  
  // Make a jet group for the special case of a simple mono-clique tree.
  // Only check needed is that there are enough jets. As the three is mono-clique
  // all Conditions pass the same jets. Only the first condition need be checked.
  // No unions need be made.
  
  if (satisfyingJets.empty()) {
    m_done = true;
  } else  if (satisfyingJets.size() < n_required) {
    m_done = true;
  } else {
    m_jetIndices = satisfyingJets;
    m_done = false;
  }

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

bool  JetGroupSingleClique::valid() const {return true;}
