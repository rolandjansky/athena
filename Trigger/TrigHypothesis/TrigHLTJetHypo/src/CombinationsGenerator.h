/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIG_HLTJETHYPO_COMBINATIONSGENERATOR_H
#define TRIG_HLTJETHYPO_COMBINATIONSGENERATOR_H

#include <algorithm>
#include <string>
#include <vector>
#include <sstream>

/* 
Combinations generator. Given n, k > 0, n>= k, bump() calculate sets 
a bit mask of length n with k set values and n-k unset values.

get() uses the bitmask to return the postions of the k set values.

When all combinations havebeen exhausted by succesive calls to bump
the process cycles. At the point of recycling a bool flag is set.
This flag is unset on the next call to bump().
*/

class CombinationsGenerator {
 public:

  friend std::ostream& operator << (std::ostream&, const CombinationsGenerator&);
  
  CombinationsGenerator(std::size_t n, std::size_t k):
    m_invalid{false}, m_N{n}, m_K(k){

    // if n==k, std::prev_permutations never returns false,
    // so treat as a special case
    if (m_N == 0 or m_K > m_N) {
      m_invalid = true;
    } else if (m_N==m_K) {
      m_NequalsKvec.reserve(m_K);
      for(std::size_t i = 0u; i != m_K; ++i){
	m_NequalsKvec.push_back(i);
      }
    } else if (m_K < m_N){
      m_bitmask = std::string(m_K, 1);
      m_bitmask.resize(m_N, 0);
    } else {
      m_invalid = true;
    }
  }

    
  std::vector<std::size_t> get() const {

    if (m_K < m_N and m_K > 0) {
      std::vector<std::size_t> comb;
      for(std::size_t  i = 0; i < m_bitmask.size(); ++i){
	if(m_bitmask[i]){comb.push_back(i);}
      }
      return comb;
    }
    
    if(m_K == m_N) {return m_NequalsKvec;}

    return std::vector<std::size_t>();
  }

  bool bump() {
    // returns true if have cycled

    if (m_K < m_N and m_K > 0) {
      return  ! std::prev_permutation(m_bitmask.begin(), m_bitmask.end());
    }
    return true;
  }

private:

  bool m_invalid;
  std::string m_bitmask;

  std::size_t m_N;
  std::size_t m_K;
  std::vector<std::size_t> m_NequalsKvec; 
};

std::ostream& operator << (std::ostream& os, const CombinationsGenerator& cg);

#endif
