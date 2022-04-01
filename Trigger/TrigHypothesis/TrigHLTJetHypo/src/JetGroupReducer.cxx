/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "./JetGroupReducer.h"
#include <set>
#include <string>
#include <sstream>


JetGroupReducer::JetGroupReducer(const std::vector<std::size_t>& siblings,
				 const CondInd2JetGroupsInds& satisfiedBy, 
				 const JetGroupInd2ElemInds& jg2elem){
  
  // make a vector of the jet indices that satisfy the sibling Conditions
  // with no duplicates

  // edge cases
  if (siblings.empty()){
    m_done = true;
    return;
  }

  for (const auto& s : siblings){
    if (satisfiedBy.at(s).empty()) {
      m_done = true;
      return;
    }
  }

  std::vector<std::size_t> satisfying; 

  for (const auto& s :siblings){
    satisfying.insert(satisfying.end(),
		      satisfiedBy.at(s).cbegin(),
		      satisfiedBy.at(s).cend()
		      );
  }

  // If more there are more than one siblings, there could be
  // duplicates in the combined list of (non-elementary) jet groups.
  // Eliminate these.
  
  auto end = satisfying.end();
  if (siblings.size() > 1) {
    std::sort(satisfying.begin(),
	      satisfying.end());

    end = std::unique(satisfying.begin(),
		      satisfying.end());
  }

  for(auto iter = satisfying.begin(); iter != end; ++iter) {
    m_jetIndices.insert(m_jetIndices.end(),
			jg2elem.at(*iter).begin(),
			jg2elem.at(*iter).end());
  }

  std::sort(m_jetIndices.begin(),
	    m_jetIndices.end());
  
  auto final_end = std::unique(m_jetIndices.begin(),
			       m_jetIndices.end());

  
  m_jetIndices.resize(final_end-m_jetIndices.begin());
  m_done = false;
  }


std::vector<std::size_t> JetGroupReducer::next(const Collector& collector){
  if(collector){
    std::stringstream sstr;
    sstr << "no of indices " << m_jetIndices.size()
	 << " done " << std::boolalpha << m_done;
    collector->collect("JetGroupReducer::next()", sstr.str());
  }
  
  if (m_done) {
    return std::vector<std::size_t>();
  }

  m_done = true;
  return m_jetIndices;
}

bool JetGroupReducer::valid() const {return true;}
