/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//  TopoCoreSimResult.h
//  L1TopoEvent
//  Created by Joerg Stelzer on 11/29/12.

#ifndef __L1TopoEvent__TopoCoreSimResult__
#define __L1TopoEvent__TopoCoreSimResult__

#include "TrigConfBase/TrigConfMessaging.h"

#include "TrigConfData/L1Menu.h"

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

   class TopoCoreSimResult : public TrigConf::TrigConfMessaging {
   public:
      TopoCoreSimResult();
      ~TopoCoreSimResult();

      bool triggerDecision(const std::string & triggerName) const;

      const TCS::TOBArray* triggerOutput(const std::string & triggerName) const;

      const GlobalDecision& globalDecision() const { return m_globalDecision; }

      const std::vector<const TCS::TOBArray*> & output(const std::string & connName) const;

      StatusCode setupFromMenu(const std::map<std::string, TCS::DecisionConnector*>& outputConnectorMap);

      StatusCode collectResult(TCS::DecisionConnector* outputConn = nullptr );

      StatusCode reset();

      void setMsgLevel( TrigConf::MSGTC::Level lvl );

   private:
      friend std::ostream& ::operator<<(std::ostream&, const TCS::TopoCoreSimResult &);

      GlobalDecision m_globalDecision;

      // map from connector name to decision connectors
      std::map<std::string, TCS::DecisionConnector*> m_outputConnectorMap;

      // map from trigger name to decision connectors
      std::map<std::string, TCS::DecisionConnector*> m_triggerLocation;

      std::set<TCS::DecisionConnector*> m_outputConnectors;

   };

}


#endif
