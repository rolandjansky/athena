/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


#ifndef L1TopoCoreSim_GlobalDecision
#define L1TopoCoreSim_GlobalDecision

#include "TrigConfBase/TrigConfMessaging.h"

#include "L1TopoCommon/StatusCode.h"
#include "L1TopoConfig/L1TopoConfigOutputList.h"

#include "TrigConfData/L1Connector.h"

#include <iostream>
#include <vector>
#include <set>
#include <string>
#include <vector>
#include <stdint.h>

namespace TCS {
   class GlobalDecision;
}


namespace TCS {

  std::ostream& operator<<(std::ostream&, const TCS::GlobalDecision &);

   class Decision;
   class DecisionConnector;

   class GlobalDecision : public TrigConf::TrigConfMessaging {
   public:
      GlobalDecision(const std::string & name = "L1TopoGlobalDecision");

      uint64_t decision_field(const std::string & connName) const { return m_decision.find(connName)->second; }

      uint32_t decision_field(std::string connName, unsigned int clock) const;

      bool passed(std::string connName, unsigned int bit) const { return ( ( (uint64_t)0x1 << bit) & m_decision.find(connName)->second) != 0; }
      uint64_t overflow_field(std::string connName) const { return m_overflow.find(connName)->second; }
      uint32_t overflow_field(std::string connName, unsigned int clock) const;
      bool overflowed(std::string connName, unsigned int bit) const { return ( ( (uint64_t)0x1 << bit) & m_overflow.find(connName)->second) != 0; }

      const Decision & decision(const std::string & algName) const;

      bool isValid() const { return m_valid; }

      void setTriggerLines(const std::vector<TrigConf::TriggerLine> & triggers);

      StatusCode collectDecision(const std::set<DecisionConnector*> & outconn);

      StatusCode resetDecision();
      void print() const;

   private:
      friend std::ostream& operator<<(std::ostream&, const TCS::GlobalDecision &);

      // 64 bit decision bit field - map connector name-decision field
      std::map<std::string,uint64_t> m_decision;
      // 64 bit overflow bit field - map connector name-overflow field
      std::map<std::string,uint64_t> m_overflow;
      // flags if the decision field is up to date
      // set by @collectDecision(), unset by @resetDecision()
      bool m_valid {false};

      // trigger lines
      std::vector<TrigConf::TriggerLine> m_triggers;
   };

}

#endif
