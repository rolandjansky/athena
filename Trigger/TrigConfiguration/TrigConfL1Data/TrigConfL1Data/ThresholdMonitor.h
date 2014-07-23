/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConf_ThresholdMonitor
#define TrigConf_ThresholdMonitor

#include "TrigConfL1Data/L1DataBaseclass.h"

#include <string>
#include <vector>
#include <stdint.h>

namespace TrigConf 
{

  class ThresholdMonitor : public L1DataBaseclass 
    {
      
    public:
      ThresholdMonitor();
      ThresholdMonitor( const ThresholdMonitor& thr);
      virtual ~ThresholdMonitor();

      // Accessors
   
      int thresholdId() const;
      void setThresholdId( const int& id) ; 
      int multiplicity() const;
      void setMultiplicity( const int& multiplicity) ; 
      int internalCounter() const;
      void setInternalCounter( const int& internalcounter ) ; 
      int bunchGroupId() const;
      void setBunchGroupId( const int& bunchgroupid ) ; 
      std::string counterType() const;
      void setCounterType(const std::string& countertype);

      std::string thresholdName() const;
      void setThresholdName(const std::string& name);

      uint16_t ctpinSlot() const;
      void setCtpinSlot(const uint16_t& );
      uint16_t ctpinConnector() const;
      void setCtpinConnector(const uint16_t& );

      int thresholdStartBit() const;
      void setThresholdStartBit( const int& bit) ; 
      int thresholdEndBit() const;
      void setThresholdEndBit( const int& bit) ; 

      bool thresholdActive() const;
      void setThresholdActive( const bool& active) ; 

      virtual void print(const std::string& indent="", unsigned int detail=1) const;

    protected:
      std::string m_CounterType;
      std::string m_ThresholdName;
      uint16_t m_CtpinSlot;
      uint16_t m_CtpinConnector;
      int m_ThresholdId;
      int m_BunchGroupId;
      int m_Multiplicity;
      int m_InternalCounter;
      int m_ThresholdStartBit;
      int m_ThresholdEndBit;
      bool m_ThresholdActive;
    };
} 

#endif 
  
