/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


#include "./JetGroupProduct.h"
#include <set>
#include <iostream>

/*
std::vector<std::size_t> merge_groups(std::vector<std::vector<std::size_t>>& iter,
				      std::vector<std::vector<std::size_t>>& end){

  std::vector<std::size_t> indices;
  for(; iter != end; ++iter){
    indices.insert(indices.end(),
		   iter->begin(),
		   iter->end());
  }
  return indices;
  
}
*/


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
  std::cout << "JetGroupProduct siblings ";
  for(const auto& e : m_siblings){std::cout <<  e << " ";}
  std::cout << '\n';
}
  
std::optional<std::vector<std::size_t>> JetGroupProduct::next(){

  while(true){
    auto opt_indices = m_productGen.next();
    if(!opt_indices.has_value()){
      std::cout<<"JGP returning with no result\n";
      return std::optional<std::vector<std::size_t>>();
    }

    // indices index job groups in the indJetGroups table
    auto indices = *opt_indices;

    // select inicies from the child jet group inicies. Form a vector
    // of inidices.
    std::vector<std::size_t> jg_indices;
    for(std::size_t i = 0; i < indices.size(); ++i){
      auto s = m_siblings[i];
      std::cout << "Sibling " << s << " prodgen pos " << i << " prodgen val " << indices[i] << " jg index "
		<< (m_satisfiedBy.at(s)).at(indices[i]) << '\n';
      //i: child; indices[i]: the particular job group for the child.
      jg_indices.push_back((m_satisfiedBy.at(s)).at(indices[i]));
    }

    
    // require there are no duplicate inidices - this would be
    // rejected  by a non-sharing flow network. but remove the
    // case early. Sharing is handled otherwise...
    std::set<std::size_t> unique_indices(jg_indices.begin(),
					 jg_indices.end());
    /*
    std::cout<< "JGP requiring unique indice set for ";
    std:: copy(jg_indices.begin(),
	       jg_indices.end(),
	       std::ostream_iterator<std::size_t>(std::cout, " "));
    std::cout<<'\n';
    */
    
    std::cout << "JGP indices before duplicates check\n";
    for(const auto& i :jg_indices){std::cout << i << " ";}
    std::cout<<'\n';

    if(jg_indices.size() == unique_indices.size()){
      std::cout << "JGP no duplicates, returning indices\n";
      return jg_indices;
    } else {
      std::cout << "JGP  duplicates, get next\n";
    }

  }
}
