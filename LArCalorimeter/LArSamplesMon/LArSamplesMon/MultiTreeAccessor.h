/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
   @class LArSamples::Container
   @brief storage of the time histories of all the cells
*/

#ifndef LArSamples_MultiTreeAccessor_H
#define LArSamples_MultiTreeAccessor_H

#include "LArSamplesMon/Accessor.h"
#include "LArCafJobs/CellInfo.h"
#include "LArSamplesMon/History.h"
#include "LArSamplesMon/FilterList.h"
#include "TString.h"
#include <vector>

namespace LArSamples {
  
  class Data;
  class FilterParams;
  class DataTweaker;
  class TreeAccessor;
  
  class MultiTreeAccessor : public Accessor {
  
    public:
   
      /** @brief Constructor  */
      MultiTreeAccessor(const std::vector<const TreeAccessor*>& accessors)
        : m_accessors(accessors) { }
      
      static MultiTreeAccessor* open(const std::vector<TString>& files);
      static MultiTreeAccessor* openList(const TString& fileList);
      static MultiTreeAccessor* openWild(const TString& wcName);

      virtual ~MultiTreeAccessor();

      unsigned int nEvents() const;
      unsigned int nRuns() const;
      
      const EventData* eventData(unsigned int i) const;
      const RunData* runData(unsigned int i) const;
      
      unsigned int historySize(unsigned int i) const;
          
      bool writeToFile(const TString& fileName) const;

      const TreeAccessor& accessor(unsigned int i) const { return *m_accessors[i]; }
      unsigned int nAccessors() const { return m_accessors.size(); }
      
      std::vector<MultiTreeAccessor*> filterComponents(const FilterList& filterList, const DataTweaker& tweaker) const;

    protected:
      
      const History* getCellHistory(unsigned int i) const;      
      const CellInfo* getCellInfo(unsigned int i) const;      

    private:
      
      std::vector<const TreeAccessor*> m_accessors;
  };
}
  
#endif
