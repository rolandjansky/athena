/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConf_PIT
#define TrigConf_PIT

#include <string>
#include <stdint.h>
#include "TrigConfL1Data/L1DataBaseclass.h"

namespace TrigConf {
   class PIT : public L1DataBaseclass {
   public:
      PIT();
      ~PIT();

      // Accessors
      
      const std::string & thresholdName() const {return m_ThresholdName;}
      void setThresholdName(const std::string& name) {m_ThresholdName = name;}

      uint16_t ctpinSlot() const {return m_CtpinSlot;}
      void setCtpinSlot(const uint16_t& id) {m_CtpinSlot = id;}

      uint16_t ctpinConnector() const {return m_CtpinConnector;}
      void setCtpinConnector(const uint16_t& id) {m_CtpinConnector = id;}

      uint16_t pitNumber() const {return m_PitNumber;}
      void setPitNumber (const uint16_t& pitnumber) {m_PitNumber = pitnumber;}

      uint16_t thresholdBit() const {return m_ThresholdBit;}
      void setThresholdBit (const uint16_t& num) {m_ThresholdBit = num;}

      uint16_t cableBit() const {return m_CableBit;}
      void setCableBit (const uint16_t& num) {m_CableBit = num;}

      int tmToTtId() const {return m_TmToTtId;}
      void setTmToTtId(const int& tmtottid){m_TmToTtId = tmtottid;}

      int triggerThresholdId() const {return m_TriggerThresholdId;}
      void setTriggerThresholdId(const int& id){m_TriggerThresholdId = id;}

      int16_t thresholdMapping() const {return m_ThresholdMapping;}
      void setThresholdMapping(const int16_t& m){m_ThresholdMapping = m;}

      bool thresholdActive() const {return m_ThresholdActive;}
      void setThresholdActive(const bool& a){m_ThresholdActive = a;}

      virtual void print(const std::string& indent="", unsigned int detail=1) const;

   protected:
      std::string m_ThresholdName;
      uint16_t m_CtpinSlot;
      uint16_t m_CtpinConnector;
      uint16_t m_PitNumber;
      uint16_t m_ThresholdBit;
      uint16_t m_CableBit;
      int m_TmToTtId;
      int m_TriggerThresholdId;
      bool m_ThresholdActive;
      int16_t m_ThresholdMapping;
   };
} 

#endif 
