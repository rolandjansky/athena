/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
   @class LArSamplesFilterList
   @brief storage of a collection of digits for one cell
*/

#ifndef LArSamples_FilterList_H
#define LArSamples_FilterList_H


#include "LArSamplesMon/FilterParams.h"
#include "TString.h"
#include <vector>


namespace LArSamples {
  
  class FilterList  {
  
   public:
   
    FilterList() { }

    void add(const FilterParams& params, const TString& fileName) { m_params.push_back(params); m_fileNames.push_back(fileName); }
    
    unsigned int size() const { return m_params.size(); }
    const FilterParams& filterParams(unsigned int i) const { return m_params[i]; }
    const TString&      fileName    (unsigned int i) const { return m_fileNames[i]; }
    
    bool pass(unsigned int hashId, const History& history, unsigned int k) const;
    bool passHash(unsigned int hashId) const;
    bool passCell(const CellInfo& info) const;
    bool passEvent(const Data& data) const;
    
   private:

    std::vector<FilterParams> m_params;
    std::vector<TString> m_fileNames;
     
  };
}
#endif
