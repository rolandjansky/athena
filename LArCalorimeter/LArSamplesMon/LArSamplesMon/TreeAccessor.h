/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
   @class LArSamples::Container
   @brief storage of the time histories of all the cells
*/

#ifndef LArSamples_TreeAccessor_H
#define LArSamples_TreeAccessor_H

#include "LArSamplesMon/Accessor.h"
#include "LArCafJobs/PersistentAccessor.h"
#include "LArCafJobs/CellInfo.h"
#include "LArSamplesMon/History.h"
#include "TString.h"
#include "TTree.h"
#include "LArSamplesMon/Chi2Calc.h"
#include "LArSamplesMon/FilterList.h"

class TFile;

namespace LArSamples {
  
  class Data;
  class FilterParams;
  class DataTweaker;
  class EventData;
  class RunData;

  class TreeAccessor : public Accessor, public PersistentAccessor {
  
    public:
   
      /** @brief Constructor  */
      TreeAccessor(TTree& cellTree, TTree& eventTree, TTree* runTree, TFile* file) 
       : PersistentAccessor(cellTree, eventTree, runTree, file) { }

      TreeAccessor(const TString& fileName) : PersistentAccessor(fileName) { }
      
      static TreeAccessor* open(const TString& fileName);
      
      virtual ~TreeAccessor() { resetCache(); }
                
      static TreeAccessor* merge(const std::vector<const Accessor*>& accessors,const TString& fileName = "");
      static TreeAccessor* merge(const std::vector<const Accessor*>& accessors,const TString& fileName,const TString& LBFile);
      static TreeAccessor* filter(const Accessor& accessor, 
                                  const FilterParams& filterParams,
                                  const TString& fileName, const DataTweaker& tweaker);
      
      static std::vector<TreeAccessor*> filter(const Accessor& accessor, 
                                               const FilterList& filterList, const DataTweaker& tweaker);
                                               
      friend class Interface;

      static TreeAccessor* makeTemplate(const Accessor& accessor, const TString& fileName);
      
      bool writeToFile(const TString& fileName) const;

      unsigned int historySize(unsigned int i) const { return PersistentAccessor::historySize(i); }

      unsigned int nEvents() const { return PersistentAccessor::nEvents(); }
      const EventData* eventData(unsigned int i) const { return PersistentAccessor::eventData(i); }

      unsigned int nRuns() const { return PersistentAccessor::nRuns(); }
      const RunData* runData(unsigned int i) const { return PersistentAccessor::runData(i); }

      void add(HistoryContainer* cont) { PersistentAccessor::add(cont); resetCache(); }

      
      const History* getCellHistory(unsigned int i) const;      
      const CellInfo* getCellInfo(unsigned int i) const;      
      
  };
}
  
#endif
