/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConf_TIP
#define TrigConf_TIP

#include <string>
#include <stdint.h>
#include "TrigConfL1Data/L1DataBaseclass.h"

namespace TrigConf {
   class TIP : public L1DataBaseclass {
   public:
      TIP();
      ~TIP();
		
      // Accessors
		
      const std::string & thresholdName() const {return m_ThresholdName;}
      void setThresholdName(const std::string& name) {m_ThresholdName = name;}
		
      uint16_t slot() const {return m_Slot;}
      void setSlot(const uint16_t& id) {m_Slot = id;}
		
      uint16_t connector() const {return m_Connector;}
      void setConnector(const uint16_t& id) {m_Connector = id;}
		
      uint16_t tipNumber() const {return m_TipNumber;}
      void setTipNumber (const uint16_t& tipnumber) {m_TipNumber = tipnumber;}
		
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
		
      bool isDirect() const {return m_IsDirect;}
      void setIsDirect(const bool& a) {m_IsDirect = a;}
		
      int clock() const {return m_Clock;}
      void setClock(const int& clock) {m_Clock = clock;}
		
      virtual void print(const std::string& indent="", unsigned int detail=1) const;
		
   protected:
      std::string m_ThresholdName; // the name of the corresponding threshold (can appear multiple times but with different thresholdBits)
      uint16_t m_Slot;
      uint16_t m_Connector;
      uint16_t m_TipNumber;  // 0-159 for the PIT, 320-511 for the direct inputs
      uint16_t m_ThresholdBit; // a threshold uses multiple bits to encode the multiplicity
      uint16_t m_CableBit;
      int m_TmToTtId;
      int m_TriggerThresholdId;
      bool m_ThresholdActive;
      int16_t m_ThresholdMapping;
      bool m_IsDirect;
      int m_Clock;
   };
} 

#endif 
