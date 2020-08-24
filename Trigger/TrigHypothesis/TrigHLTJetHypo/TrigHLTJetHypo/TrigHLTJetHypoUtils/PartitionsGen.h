/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TRIGHLTHETHYPO_PARTITIONSGEN_H
#define TRIGHLTHETHYPO_PARTITIONSGEN_H

/*
  Partition Generator.
  Input: no of items to distribute (jet indices)
         no of jets per allocated object (no of jets to satisfy each condition

 Example 1: to_allocate = 6, 
          assoc_inds = [2, 3]
        
	  corresponds to 6 jets being allocated to 2 conditions, the first of
	  which requires 2 jets, while the second requires 3 jets.

	  each call to next() returns a new allocation.

	  In this example there are:

	  6*5*4*3*2/(2!* 3!) = 60 partitions.

 Example 2: to_allocate = 4
            assoc_inds = [2, 1]

	    alloc mask allocations (j1, j2) -> c0, (j3) -> c1 
	    -1 0 0 1  (1 2 ) (3 ) 
	    -1 0 1 0  (1 3 ) (2 ) 
	    -1 1 0 0  (2 3 ) (1 ) 
	    0 -1 0 1  (0 2 ) (3 ) 
	    0 -1 1 0  (0 3 ) (2 ) 
	    0 0 -1 1  (0 1 ) (3 ) 
	    0 0 1 -1  (0 1 ) (2 ) 
	    0 1 -1 0  (0 3 ) (1 ) 
	    0 1 0 -1  (0 2 ) (1 ) 
	    1 -1 0 0  (2 3 ) (0 ) 
	    1 0 -1 0  (1 3 ) (0 ) 
	    1 0 0 -1  (1 2 ) (0 ) 

*/


#include <vector>
#include <algorithm>
#include <numeric>
#include <optional>


class PartitionsGen {
 public:

  PartitionsGen(std::size_t to_allocate,
	       const std::vector<std::size_t>& mults){

    if(!to_allocate){m_valid=false;}

    if(m_valid){
      m_current = std::vector<int>(to_allocate, -1);
      m_nc = mults.size();
      std::size_t total = std::accumulate(mults.begin(),
					  mults.end(),
					  0);
      if (total >  m_current.size()) {m_valid = false;}
    
      auto pos{0};
      auto ic{0};
      for(std::size_t i : mults){
	for(std::size_t j = 0; j < i; ++j){
	  m_current[pos] = ic;
	  ++pos;
	}
	++ic;
      }
    
      std::sort(m_current.begin(), m_current.end());
    }
  }	    


  std::optional<std::vector<std::vector<std::size_t>>> next() {
    if(!m_valid){
      return  std::optional<std::vector<std::vector<std::size_t>>> ();
    }

    std::vector<std::vector<std::size_t>> result(m_nc);

    auto nj = m_current.size();
    for(std::size_t ij = 0; ij < nj; ++ij){
      const auto& ic = m_current[ij];
      if(ic != -1){
	result[ic].push_back(ij);
      }
    }

    m_valid = std::next_permutation(m_current.begin(),
 				    m_current.end());
    return std::make_optional<std::vector<std::vector<std::size_t>>>(result); 
  }

 private:
  
  bool m_valid{true};
  std::vector<int> m_current;
  std::size_t m_nc = 0;
  
};

# endif

