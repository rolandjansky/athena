//  TopoCoreSimResult.h
//  L1TopoEvent
//  Created by Joerg Stelzer on 11/29/12.
//  Copyright (c) 2012 Joerg Stelzer. All rights reserved.

#ifndef __L1TopoEvent__TopoCoreSimResult__
#define __L1TopoEvent__TopoCoreSimResult__

#include <iostream>
#include "L1TopoCommon/StatusCode.h"
#include "L1TopoCoreSim/GlobalDecision.h"
#include <vector>
#include <map>
#include <set>

namespace TCS { class TopoCoreSimResult; }

std::ostream& operator<<(std::ostream&, const TCS::TopoCoreSimResult &);

namespace TXC {
   class L1TopoMenu;
}

namespace TCS {

   class TOBArray;
   class DecisionConnector;

   class TopoCoreSimResult {
   public:
      TopoCoreSimResult();
      ~TopoCoreSimResult();

      const GlobalDecision& globalDecision() const { return m_globalDecision; }

      const std::vector<const TCS::TOBArray*> & output(const std::string & connName) const;

      StatusCode setupFromMenu(const TXC::L1TopoMenu & menu, const std::map<std::string, TCS::DecisionConnector*>& outputConnectorMap);

      StatusCode collectResult();

      StatusCode reset();

   private:
      friend std::ostream& ::operator<<(std::ostream&, const TCS::TopoCoreSimResult &);

      GlobalDecision m_globalDecision;

      // map to all decision algorithms for configuration and decision
      std::map<std::string, TCS::DecisionConnector*> m_outputConnectorMap;

      std::set<TCS::DecisionConnector*> m_outputConnectors;

   };

}


#endif
