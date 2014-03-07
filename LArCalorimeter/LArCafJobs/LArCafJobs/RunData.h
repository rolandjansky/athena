/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/**
   @class LArSamplesRunData
   @brief Liquid Argon class for compact storage of samples data
*/

#ifndef LArSamples_RunData_H
#define LArSamples_RunData_H

#include "TString.h"
#include <map>

namespace LArSamples {
  
  class RunData {
    
    public:
      
      /** @brief Constructor  */
      RunData(int run = 0);
      
      RunData(const RunData& other);
             
      /** @brief Destructor */
      virtual ~RunData();
  
      void addBit(const TString& triggerBitName, unsigned int triggerBitPosition) { m_triggerConfig[triggerBitName] = triggerBitPosition; }
      int triggerBitPosition(const TString& triggerBitName) const;
      
      int run() const { return m_run; }
      
      unsigned int nTriggerBits() const { return m_triggerConfig.size(); }
      const std::map<TString, unsigned int>& triggerConfig() const { return m_triggerConfig; }

      TString triggerBits() const;
      
      double footprint() const { return sizeof(*this); }

      void setRun(unsigned int run) { m_run = run; }
      TString description(unsigned int verbosity) const;

    private:

      int m_run;
      std::map<TString, unsigned int> m_triggerConfig;

      RunData& operator= (const RunData&);
  };
}
#endif

