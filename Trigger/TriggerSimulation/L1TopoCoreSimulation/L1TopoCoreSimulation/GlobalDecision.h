/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//
//  GlobalDecision.h
//  L1TopoCoreSimulation
//
//  Created by Joerg Stelzer on 11/25/12.
//

#ifndef __L1TopoCoreSimulation__GlobalDecision__
#define __L1TopoCoreSimulation__GlobalDecision__

#include "L1TopoCoreSimulation/StatusCode.h"

#include <iostream>
#include <string>
#include <map>
#include <stdint.h>

namespace TCS { class GlobalDecision; }

std::ostream& operator<<(std::ostream&, const TCS::GlobalDecision &);

namespace TCS {

   class Decision;
   class DecisionAlg;

   class GlobalDecision {
   public:

      uint64_t decision() const { return m_decision; }
      bool decision(uint64_t bit) const { return (0x1<<bit & m_decision) != 0; }

      const Decision & decision(const std::string & algName) const;

      bool isValid() const { return m_valid; }

      StatusCode addDecisionAlg(const DecisionAlg*);

      StatusCode collectDecision();

      StatusCode resetDecision();

   private:
      friend std::ostream& ::operator<<(std::ostream&, const TCS::GlobalDecision &);

      typedef std::map<std::string,const TCS::DecisionAlg*> algmap_t;

      // 64 bit decision bit field
      uint64_t m_decision;

      // flags if the decision field is up to date
      bool m_valid;

      // map to all decision algorithms for configuration and decision
      algmap_t m_algo;
   };

} // end of namespace TCS

#endif /* defined(__L1TopoCoreSimulation__GlobalDecision__) */
