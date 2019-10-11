/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


#include "./JetGroupProduct.h"
#include <set>
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


JetGroupProduct::JetGroupProduct(const std::vector<std::vector<std::size_t>>&
				 jetGroupIndVec,
				 const std::map<std::size_t, HypoJetVector>&
				 indJetGroups
				 ):
  m_jetGroupIndVec(jetGroupIndVec), m_indJetGroups(indJetGroups){
  std::vector<std::size_t> ends;
  ends.reserve(m_jetGroupIndVec.size());
  for(const auto& v : m_jetGroupIndVec){
    ends.push_back(v.size());
  }
  m_productGen = ProductGen(ends);
  std::cout
    << "JetGroupProduct size jetGroupIndVec = "
    <<jetGroupIndVec.size() << " content sizes: ";
  for(const auto& e : jetGroupIndVec){std::cout <<  e.size() << " ";}
  std::cout << '\n';
  
}
  
std::optional<HypoJetVector> JetGroupProduct::next(){

  while(true){
    auto opt_indices = m_productGen.next();
    if(!opt_indices.has_value()){
      std::cout<<"JGP returning with no result\n";
      return  std::optional<HypoJetVector>();
    }

    auto indices = *opt_indices;

    std::vector<std::size_t> j_indices;
    for(std::size_t i = 0; i < indices.size(); ++i){
      std::cout << "Prodgen " << i << " " << indices[i] << " " << m_jetGroupIndVec.size() << '\n';
      j_indices.push_back(m_jetGroupIndVec.at(i).at(indices[i]));
    }
    
    HypoJetGroupVector groups;
    std::set<std::size_t> unique_indices(indices.begin(), indices.end());
    if(indices.size() == unique_indices.size()){
      for(const auto& i : j_indices){
	groups.push_back(m_indJetGroups.at(i));
      }
      
      auto iter {groups.cbegin()};
      auto end {groups.cend()};
      
      auto result =  merge_groups(iter, end);
      std::cout<< "JGP returning group of size " << result.size() << '\n';
      std:: copy(indices.begin(),
		 indices.end(),
		 std::ostream_iterator<std::size_t>(std::cout, " "));
      std::cout << '\n';
      return result;
    }
  }
}
