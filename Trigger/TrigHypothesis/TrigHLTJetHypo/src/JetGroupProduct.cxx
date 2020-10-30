/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "./JetGroupProduct.h"
#include <set>
#include <string>


JetGroupProduct::JetGroupProduct(const std::vector<std::size_t>& siblings,
				 const CondInd2JetGroupsInds& satisfiedBy
				 ):
  // inner vector of jetGroupIndVec are the indices of indJetGroups
  // that satisfy one child. A parent may have many children
  // indJetGroups is a size_t: jet group table.
  
  m_siblings(siblings), m_satisfiedBy(satisfiedBy){
  std::vector<std::size_t> ends;
  ends.reserve(m_siblings.size());
  for(const auto& s : m_siblings){
    ends.push_back((m_satisfiedBy.at(s)).size());
  }
  m_productGen = ProductGen(ends);
}
  
std::optional<std::vector<std::size_t>>
JetGroupProduct::next(const Collector& collector){

  unsigned int ipass{0};
  
  while(true){
    
    if(collector){
	  collector->collect("JobGroupProduct::next()",
                         "loop start pass" + std::to_string(ipass++));
	}
      
    auto opt_indices = m_productGen.next();
    if(!opt_indices.has_value()){
      return std::optional<std::vector<std::size_t>>();
    }

    // indices index job groups in the indJetGroups table
    auto indices = *opt_indices;

    // select indicies from the child jet group indicies. Form a vector
    // of indices.
    std::vector<std::size_t> jg_indices;
    for(std::size_t i = 0; i < indices.size(); ++i){
      auto s = m_siblings[i];

      //i: child; indices[i]: the particular job group for the child.
      jg_indices.push_back((m_satisfiedBy.at(s)).at(indices[i]));
    }

    
    // require there are no duplicate indices - this would be
    // rejected  by a non-sharing flow network. but remove the
    // case early. Sharing is handled otherwise...
    std::set<std::size_t> unique_indices(jg_indices.begin(),
					 jg_indices.end());
    
    if(jg_indices.size() == unique_indices.size()){
      return jg_indices;
    }
  }
  std::optional<std::vector<std::size_t>>();
}
