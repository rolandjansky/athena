/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_INDEXVECCOMP_H
#define TRIGHLTJETHYPO_INDEXVECCOMP_H

struct IndexVecComp {
  bool operator () (const std::vector<std::size_t>& lhs,
		    const std::vector<std::size_t>& rhs){
    if(lhs.size() < rhs.size()){return true;}
    if(rhs.size() < rhs.size()){return false;}
    
    for(std::size_t i = 0; i < lhs.size(); ++i){
      if(lhs[i] > rhs[i]){return false;}
    }
    
    return true;
  }
  
};

#endif
