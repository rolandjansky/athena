/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "./JetGroupUnion.h"
#include <set>
#include <string>
#include <sstream>


JetGroupUnion::JetGroupUnion(const std::vector<std::size_t>& siblings,
			     const CondInd2JetGroupsInds& satisfiedBy) {
  
  // make a vector of the jet indices that satisfy the sibling Conditions
  // with no duplicates

  for (const auto& s :siblings){
    m_jetIndices.insert(m_jetIndices.end(),
			satisfiedBy.at(s).cbegin(),
			satisfiedBy.at(s).cend()
			);
  }
  
  std::sort(m_jetIndices.begin(),
	    m_jetIndices.end());

  auto iter = std::unique(m_jetIndices.begin(),
			  m_jetIndices.end());
					      
  m_jetIndices.resize(iter-m_jetIndices.begin());
  m_done = false;
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

bool JetGroupUnion::valid() const {return true;}
