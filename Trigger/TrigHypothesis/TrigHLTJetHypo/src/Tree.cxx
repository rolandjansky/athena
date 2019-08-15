/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "./Tree.h"
#include <iostream>

Tree::Tree(const std::vector<std::size_t>& parents) :
  m_parents(parents),
  m_children(m_parents.size(),std::vector<std::size_t>()){
  auto nParents = parents.size();

  for(auto i = 0; i < nParents; ++i){
    m_children[m_parents[i]].push_back(i);
  }

  auto nChildren = m_children.size();

  for(auto i = 0; i < nChildren; ++i){
    if(m_children[i].empty()){
      m_leaves.push_back(i);
    }
  }

  for(auto i = 0; i < nParents; ++i){
    if (i == 0){continue;}
    if(parents[i] == 0){m_firstGeneration.push_back(i);}
  }
}


std::size_t Tree::getParent(std::size_t k) const {
  return m_parents[k];
}


std::vector<std::size_t> Tree::getSiblings(std::size_t k) const {
  return m_children[getParent(k)];
}

std::size_t Tree::size() const {
  return m_parents.size();
}


const std::vector<std::size_t>& Tree::leaves() const {
  return m_leaves;
}

const std::vector<std::size_t>& Tree::firstGeneration() const {
  return m_firstGeneration;
}


