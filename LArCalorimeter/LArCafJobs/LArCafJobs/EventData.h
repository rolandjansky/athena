/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/**
   @class LArSamplesEventData
   @brief Liquid Argon class for compact storage of samples data
*/

#ifndef LArSamples_EventData_H
#define LArSamples_EventData_H

#include "TString.h"
#include <vector>

namespace LArSamples {

  class RunData;
  
  struct RoIPosition { 
    RoIPosition(float e = -999, float p = -999, const char* n = "", const char* l = "") 
    : eta(e), phi(p), name(n), label(l) { } 
    double eta, phi; 
    TString name, label; 
  };
  
  class EventData {
    
    public:
      
      /** @brief Constructor  */
      EventData();
      
      EventData(int event, int runIndex, int lumiBlock, int bunchId); 

      EventData(const EventData& other, int newRunIndex = -1);
             
      /** @brief Destructor */
      virtual ~EventData();
  
      /** @return event */
      int event() const { return m_eventData[0]; }
      
      /** @return lumi block */
      unsigned int lumiBlock() const { return m_eventData[2]; }
      
       /** @return bunch crossing id */     
      unsigned int bunchId() const { return m_eventData[3]; }

      /** @return event */
      int runIndex() const { return m_eventData[1]; }

      /** @return run */
      int run() const;
     
      /** @return event */
      unsigned int nTriggerWords() const { return m_eventData.size() - 4; }
      unsigned int triggerWord(unsigned int i) const;
      unsigned int l1Word(unsigned int i) const { return triggerWord(i); }

      unsigned int nRoIs() const { return m_rois.size(); }
      RoIPosition roI(unsigned int i) const { if (i < nRoIs()) return m_rois[i]; return RoIPosition(); }

      std::vector<unsigned int> triggerData() const;

      bool isPassed(const TString& bitName) const;
      
      double footprint() const { return sizeof(*this) + m_eventData.size()*sizeof(unsigned int); }

      void setTriggerData(const std::vector<unsigned int>& words);
      void addRoI(float eta, float phi, const char* name = "", const char* label = "") { m_rois.push_back(RoIPosition(eta, phi, name, label)); }
      
      void setRunData(const RunData* runData) { m_runData = runData; }
      const RunData* runData() const { return m_runData; }
      
      bool bit(unsigned int pos) const;
      TString triggers() const;
      TString roIs() const;
      TString description(unsigned int verbosity) const;
      
      void drawDRRoIs(double roISize = 0.1) const;
      void drawBoxRoIs(double etaSize = 0.2, double phiSize = 0.2) const;
      void removeRoIs() { m_rois.clear(); }

      static void drawBoxRoI(double eta, double phi, double etaSize = 0.2, double phiSize = 0.2);

    private:
  
      void setRunIndex(int idx) { m_eventData[1] = idx; }
      
      std::vector<unsigned int> m_eventData;
      std::vector<RoIPosition> m_rois;
      
      mutable const RunData* m_runData;

      EventData& operator= (const EventData&);
  };
}
#endif

