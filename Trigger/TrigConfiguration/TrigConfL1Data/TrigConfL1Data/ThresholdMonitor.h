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
   
      int thresholdId() const { return m_ThresholdId; }
      int multiplicity() const { return m_Multiplicity; }
      int internalCounter() const { return m_InternalCounter; }
      int bunchGroupId() const { return m_BunchGroupId; }
      std::string counterType() const { return m_CounterType; }
      std::string thresholdName() const { return m_ThresholdName; }
      uint16_t ctpinSlot() const { return m_CtpinSlot; }
      uint16_t ctpinConnector() const { return m_CtpinConnector; }
      int thresholdStartBit() const { return m_ThresholdStartBit; }
      int thresholdEndBit() const { return m_ThresholdEndBit; }
      bool thresholdActive() const { return m_ThresholdActive; }

      // setters
      void setThresholdId( const int& id) { m_ThresholdId = id; }
      void setMultiplicity( const int& multiplicity) { m_Multiplicity = multiplicity; }
      void setInternalCounter( const int& internalcounter ) { m_InternalCounter = internalcounter; }
      void setBunchGroupId( const int& bunchgroupid ) { m_BunchGroupId = bunchgroupid; }
      void setCounterType(const std::string& countertype) { m_CounterType = countertype; }
      void setThresholdName(const std::string& name) { m_ThresholdName = name; }
      void setCtpinSlot(const uint16_t & slot) { m_CtpinSlot = slot; }
      void setCtpinConnector(const uint16_t & con) { m_CtpinConnector = con; }
      void setThresholdStartBit( const int& bit) { m_ThresholdStartBit = bit; }
      void setThresholdEndBit( const int& bit) { m_ThresholdEndBit = bit; }
      void setThresholdActive( const bool& active) { m_ThresholdActive = active; }

      virtual void print(const std::string& indent="", unsigned int detail=1) const;
      virtual void writeXML(std::ostream & xmlfile, int indentLevel=0, int indentWidth=2) const;

   protected:
      std::string m_CounterType {""};
      std::string m_ThresholdName {""};
      uint16_t m_CtpinSlot {0};
      uint16_t m_CtpinConnector {0};
      int m_ThresholdId {0};
      int m_BunchGroupId {0};
      int m_Multiplicity {0};
      int m_InternalCounter {0};
      int m_ThresholdStartBit {0};
      int m_ThresholdEndBit {0};
      bool m_ThresholdActive {false};
   };
} 

#endif 
  
