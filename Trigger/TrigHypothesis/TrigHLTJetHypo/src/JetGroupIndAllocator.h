/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_JETGROUPINDALLOCATOR_H
#define TRIGHLTJETHYPO_JETGROUPINDALLOCATOR_H

#include "dumpIndices.h"

#include <vector>
#include <map>
#include <algorithm>

#include <sstream>

// Avoid assigning different jet group indices to equivalent job groups.
// job groups are equivalent if their sorted lists of elemental
// groups are the same;
//
// JetGroupIndAllocator - a class to allocate a jet group index
// The elemental job groups are received. 
// "Seen" means that an index has already been issued for a job group
// with the same elemental jet groups



class JetGroupIndAllocator {
 public:
  std::size_t operator()(std::vector<std::size_t> v){

    std::sort(v.begin(), v.end());

    auto cmp = [&v](const std::vector<std::size_t>& known){
      auto sz = v.size();
      if(sz != known.size()){return false;}
      
      // vectors are sorted
      for(std::size_t i = 0; i < sz; ++i){
	if (known.at(i) != v.at(i)) {
	  return false;
	}
      }

      return true;
    };

    
    auto iter = std::find_if(m_seen.begin(), m_seen.end(), cmp);
    if(iter == m_seen.end()){
      std::size_t ind = m_seen.size();
      m_seen.push_back(v);
      return ind;
    }
    auto ind = iter - m_seen.begin();
    return ind;
}
 private:
  std::vector<std::vector<std::size_t>> m_seen;
};
#endif
