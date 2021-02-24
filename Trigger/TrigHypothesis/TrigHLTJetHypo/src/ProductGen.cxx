/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


#include "./ProductGen.h"
#include <algorithm>


ProductGen::ProductGen() {}

ProductGen::ProductGen(const std::vector<std::size_t>& ends):
  m_ends(ends), m_counters(ends.size(), 0), m_ncounters(ends.size()){

  if (!m_ends.empty()){
    m_done = false;
    for(auto& i : m_ends){
      if(i < 1) {
	m_done = true;
	break;
      } else {
	i -= 1;
      }
    }
  }
  
}


std::vector<std::size_t>  ProductGen::next() {
  
  if(m_done){return std::vector<std::size_t>();}

  auto result =  std::vector<std::size_t>(m_counters);
  m_done = atEnd();
  for(std::size_t i = 0; i < m_ncounters; ++i){
    
    if(m_counters[i] == m_ends[i]){
      m_counters[i] = 0;
    } else {
      m_counters[i] += 1;
      break;
    }
  }
  return result;
}
