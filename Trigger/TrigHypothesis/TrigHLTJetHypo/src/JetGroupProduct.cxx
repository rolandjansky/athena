/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "./JetGroupProduct.h"
#include <set>
#include <string>


JetGroupProduct::JetGroupProduct(const std::vector<std::size_t>& siblings,
				 const CondInd2JetGroupsInds& satisfiedBy,
				 const std::vector<std::size_t>& condMult
				 ) {
  // inner vector of jetGroupIndVec are the indices of indJetGroups
  // that satisfy one child. A parent may have many children
  // indJetGroups is a size_t: jet group table.


  // copy the parts od satisfiedBy corresponding to the sibling indices
  // into m_condIndices. The number of copies made per sibling is the
  // given by the sibling multiplicity.

  for(const auto& isib : siblings){
    auto mult = condMult[isib];  // jet groups indices of satisying jgs.

    // no of copies = multiplicity of the Condition
    for (std::size_t im = 0; im != mult; ++im){
      m_condIndices.push_back(satisfiedBy.at(isib));
    }
  }

  // find the size for the satisfying jet group vectors.
  // these values will be used ot generate indexes into m_condIndices.
  std::vector<std::size_t> ends;
  ends.reserve(m_condIndices.size());
  for(const auto& s : m_condIndices){
    ends.push_back(s.size());
  }

  // ProductGen is a device for calculating a tuple of indices
  // into a vector of vectors of indices. The length of the tuple
  // is the length of m_condIndices. The values of the tuple
  // are indices into the inner vectors.
  m_productGen = ProductGen(ends);
}
  
std::optional<std::vector<std::size_t>>
JetGroupProduct::next(const Collector& collector){

  unsigned int ipass{0};
  
  while(true){
    
    if(collector){
      collector->collect("JetGroupProduct::next()",
                         "loop start pass" + std::to_string(ipass++));
    }
      
    auto opt_indices = m_productGen.next();
    if(!opt_indices.has_value()){
      return std::optional<std::vector<std::size_t>>();
    }

    // indices index jet groups in the indJetGroups table
    auto indices = *opt_indices;

    // select indicies from the child jet group indicies. Form a vector
    // of indices.
    std::vector<std::size_t> jg_indices;
    for(std::size_t i = 0; i < indices.size(); ++i){
      jg_indices.push_back((m_condIndices.at(i)).at(indices[i]));
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
