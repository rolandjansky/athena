/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
   @class LArSamples::DataStore
   @brief storage of the time histories of all the cells
*/

#ifndef LArSamples_DataStore_H
#define LArSamples_DataStore_H

#include "LArCafJobs/Definitions.h"
#include "LArCafJobs/HistoryContainer.h"
#include "LArCafJobs/CellInfo.h"
#include "LArCafJobs/CaloId.h"

#include <vector>
#include "CLIDSvc/CLASS_DEF.h" 

class IdentifierHash;
class TH1I;
class LArShapeDumper;

namespace LArSamples {
  
  class D3PDConverter;
  class DataContainer;
  class EventData;
  class RunData;
  
  class DataStore  {
  
   public:
   
    /** @brief Constructor  */
    DataStore();

    /** @brief Destructor */
    virtual ~DataStore();
    
    const HistoryContainer* historyContainer(unsigned int i) const { return m_cellHistories[i]; }
    const EventData* eventData(unsigned int i) const { return m_events[i]; }
    const RunData* runData(unsigned int i) const { return m_runs[i]; }

    /** @brief append data (takes ownership of everything) */
    HistoryContainer* makeNewHistory(const IdentifierHash& hash, CellInfo* info);
    bool addData(const IdentifierHash& hash, DataContainer* data);
    
    unsigned int addEvent(EventData* eventData);
    unsigned int addRun(RunData* eventData);

    unsigned int nChannels() const { return Definitions::nChannels; }
    unsigned int size() const;
    unsigned int nFilledChannels() const;
    unsigned int nEvents() const { return m_events.size(); }
    unsigned int nRuns() const { return m_runs.size(); }

    double footprint() const;
    bool writeTrees(const char* fileName);
    
    friend class ::LArShapeDumper;
    friend class D3PDConverter;
    
   private:
      
    HistoryContainer*& hist_cont(unsigned int i) { return m_cellHistories[i]; }
    EventData*& eventData(unsigned int i) { return m_events[i]; }
    RunData*& runData(unsigned int i) { return m_runs[i]; }

    std::vector<HistoryContainer*> m_cellHistories;
    std::vector<EventData*> m_events;
    std::vector<RunData*> m_runs;
  };
}

#endif
