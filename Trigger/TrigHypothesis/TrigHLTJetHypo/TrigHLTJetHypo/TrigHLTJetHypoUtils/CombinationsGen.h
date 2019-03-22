/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTHETHYPO_COMBINATIONSGEN_H
#define TRIGHLTHETHYPO_COMBINATIONSGEN_H

#include <algorithm>
#include <string>
#include <vector>
#include <sstream>

/* 
Combinations generator. Given n, k > 0, n>= k, next() returns the
next combination in a pair of std::array<int, k>, bool, where bool
= false means the array is not a valid combination (ie the iteration
has terminated) 
*/

class CombinationsGen {
 public:
 CombinationsGen(unsigned int n, unsigned int k): 
  m_more{true}, m_invalid{false}{
    
    if (k<=n){
      m_bitmask = std::string(k, 1);
      m_bitmask.resize(n, 0);
    } else {
      m_invalid = true;
    }
  }
  
  std::pair<std::vector<unsigned int>, bool> next() {

    std::vector<unsigned int> comb;
    if (m_invalid){return {comb, false};}

    if(not m_more){return {comb, false};}
    
    for(std::size_t  i = 0; i < m_bitmask.size(); ++i){
      if(m_bitmask[i]){comb.push_back(i);}
    }
    m_more = std::prev_permutation(m_bitmask.begin(), m_bitmask.end());
    return {comb, true};
  }

private:
  bool m_more;
  bool m_invalid;
  std::string m_bitmask;
};
#endif
