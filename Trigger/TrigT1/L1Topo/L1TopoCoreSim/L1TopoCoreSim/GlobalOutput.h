/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#ifndef L1TopoCoreSim_GlobalOutput
#define L1TopoCoreSim_GlobalOutput

#include "TrigConfBase/TrigConfMessaging.h"

#include "L1TopoCommon/StatusCode.h"
#include "L1TopoConfig/L1TopoConfigOutputList.h"

#include "TrigConfData/L1Connector.h"

#include <iostream>
#include <vector>
#include <set>
#include <string>
#include <bitset>
#include <stdint.h>

namespace TCS {
   class GlobalOutput;
}


namespace TCS {

  std::ostream& operator<<(std::ostream&, const TCS::GlobalOutput &);

   class Decision;
   class DecisionConnector;
   class Count;
   class CountingConnector;

   class GlobalOutput : public TrigConf::TrigConfMessaging {
   public:
      GlobalOutput(const std::string & name = "L1TopoGlobalOutput");

      uint64_t decision_field(const std::string & l1connName) const;

      uint32_t decision_field(std::string l1connName, unsigned int clock) const;

      bool passed(std::string connName, unsigned int bit) const { return ( ( (uint64_t)0x1 << bit) & m_decision.find(connName)->second) != 0; }
      uint64_t overflow_field(std::string l1connName) const;
      uint32_t overflow_field(std::string l1connName, unsigned int clock) const;
      bool overflowed(std::string l1connName, unsigned int bit) const { return ( ( (uint64_t)0x1 << bit) & m_overflow.find(l1connName)->second) != 0; }

      std::bitset<128> count_field(std::string l1connName) const;

      const Decision & decision(const std::string & algName) const;

      bool isValid() const { return m_valid; }

      void setDecisionTriggerLines(const std::vector<TrigConf::TriggerLine> & triggers);

      void setMultiplicityTriggerLines(const std::vector<TrigConf::TriggerLine> & triggers);

      StatusCode collectOutput(const std::set<DecisionConnector*> & outConn, const std::set<CountingConnector*> & countConn);

      StatusCode resetOutput();
      void print() const;

   private:
      friend std::ostream& operator<<(std::ostream&, const TCS::GlobalOutput &);

      // 64 bit decision bit field - map L1Connector name-decision field
      std::map<std::string,uint64_t> m_decision;
      // 64 bit overflow bit field - map L1Connector name-overflow field
      std::map<std::string,uint64_t> m_overflow;

      // 128 bit multiplicity bit field - map L1Connector name-multiplicity field
      std::map<std::string,std::bitset<128>> m_count;
      // flags if the decision field is up to date
      // set by @collectDecision(), unset by @reset()
      bool m_valid {false};

      // trigger lines
      std::vector<TrigConf::TriggerLine> m_triggersDec;
      std::vector<TrigConf::TriggerLine> m_triggersCount;
   };

}

#endif
