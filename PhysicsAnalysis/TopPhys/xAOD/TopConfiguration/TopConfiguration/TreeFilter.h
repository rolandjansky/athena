/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */
 
#ifndef ANALYSISTOP_TOPCONFIGURATION_TREEFILTER_H
#define ANALYSISTOP_TOPCONFIGURATION_TREEFILTER_H
  
#include <string>
#include <vector>
 
namespace top {
  
  class TreeFilter {
    private:
      std::vector<std::string> m_vecFilters;
    
    public:
      TreeFilter();
      TreeFilter(const std::string& longstring);
      void init(const std::string& longstring);
      bool filterTree(const std::string& treename) const;
    
  };
  
}


#endif
