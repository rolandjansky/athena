/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


#include "./JetGroupProduct.h"

#include <iostream>

HypoJetVector merge_groups(HypoJetGroupCIter& iter,
			   const HypoJetGroupCIter& end){
  HypoJetVector jets;
  for(; iter != end; ++iter){
    jets.insert(jets.end(),
		iter->begin(),
		iter->end());
  }
  return jets;
    
  }


JetGroupProduct::JetGroupProduct(const std::vector<HypoJetGroupVector>& inVecs):
  m_inVecs(inVecs), m_nVec(inVecs.size()){
  std::vector<std::size_t> ends;
  ends.reserve(m_inVecs.size());
  for(const auto& v : m_inVecs){
    ends.push_back(v.size());
  }
  std::cout << "JetGroupProduct ends:\n";
  for( const auto& i : ends){std::cout << i << " ";}
  std::cout <<'\n';
      
      
  m_productGen = ProductGen(ends);
  std::cout << "JetGroupProduct end of constructor\n";
}
  
std::optional<HypoJetVector> JetGroupProduct::next(){
  auto indices = m_productGen.next();
  if(!indices.has_value()){
    return  std::optional<HypoJetVector>();
  }
    
  HypoJetGroupVector groups;
  for(std::size_t i = 0; i < indices->size(); ++i){
    groups.push_back(m_inVecs[i][(*indices)[i]]);
  }
  
  auto iter {groups.cbegin()};
  auto end {groups.cend()};
  return merge_groups(iter, end);
}
