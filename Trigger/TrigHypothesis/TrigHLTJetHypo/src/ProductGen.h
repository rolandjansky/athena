/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_PRODUCTGEN_H
#define TRIGHLTJETHYPO_PRODUCTGEN_H

#include <optional>
#include <vector>
#include <algorithm>

// For a vector of vectors, repeated calls to next() returns
// indices that point to elements in the inner vectors.
// The calls will step through this data structure.
class ProductGen{
 public:
  ProductGen() {}

  // ends contains the sizes of the inner vectors
  explicit ProductGen(const std::vector<std::size_t>& ends):
  m_ends(ends),
    m_counters(ends.size(), 0),
    m_ncounters(m_counters.size()){
    for(auto& i : m_ends){
      if(i < 1) {
	m_done = true;
	break;
      } else {
	i -= 1;
      }
    }
  }
 
  std::optional<std::vector<std::size_t>> next() {

    if(m_done){return std::optional<std::vector<std::size_t>>();}
    
    auto result =  std::make_optional<std::vector<std::size_t>>(m_counters);
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
 private:
  std::vector<std::size_t> m_ends;
  std::vector<std::size_t> m_counters;
  std::size_t m_ncounters;
  bool m_done{false};
  bool atEnd(){return (m_counters == m_ends);}
};

#endif
