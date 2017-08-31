//  GlobalDecision.h
//  L1TopoCoreSimulation
//  Created by Joerg Stelzer on 11/25/12.
//  Copyright (c) 2012 Joerg Stelzer. All rights reserved.

#ifndef L1TopoCoreSimulation_GlobalDecision
#define L1TopoCoreSimulation_GlobalDecision

#include "TrigConfBase/TrigConfMessaging.h"

#include "L1TopoCommon/StatusCode.h"
#include "L1TopoConfig/L1TopoConfigOutputList.h"

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
      GlobalDecision();

      uint64_t decision(unsigned int module) const { return m_decision[module]; }

      uint32_t decision(unsigned int module, unsigned int clock) const;

      bool passed(unsigned int module, unsigned int bit) const { return ( ( (uint64_t)0x1 << bit) & m_decision[module]) != 0; }
      uint64_t overflow(unsigned int module) const { return m_overflow[module]; }
      uint32_t overflow(unsigned int module, unsigned int clock) const;
      bool overflowed(unsigned int module, unsigned int bit) const { return ( ( (uint64_t)0x1 << bit) & m_overflow[module]) != 0; }

      const Decision & decision(const std::string & algName) const;

      bool isValid() const { return m_valid; }

      void setTriggerLines(const std::vector<TXC::TriggerLine> & triggers) { m_triggers = triggers; }

      StatusCode collectDecision(const std::set<DecisionConnector*> & outconn);

      StatusCode resetDecision();
      void print() const;

   private:
      friend std::ostream& operator<<(std::ostream&, const TCS::GlobalDecision &);

      // 64 bit decision bit field
      uint64_t m_decision[3] {0,0,0};
      // 64 bit overflow bit field
      uint64_t m_overflow[3] {0,0,0};
      // flags if the decision field is up to date
      // set by @collectDecision(), unset by @resetDecision()
      bool m_valid {false};

      // trigger lines
      std::vector<TXC::TriggerLine> m_triggers;
   };

}

#endif
