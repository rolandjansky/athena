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
#include "L1TopoCoreSim/GlobalOutput.h"
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
   class CountingConnector;

   class TopoCoreSimResult : public TrigConf::TrigConfMessaging {
   public:
      TopoCoreSimResult();
      ~TopoCoreSimResult();

      bool triggerDecision(const std::string & triggerName) const;

      const TCS::TOBArray* triggerOutput(const std::string & triggerName) const;

      const GlobalOutput& globalOutput() const { return m_globalOutput; }

      const std::vector<const TCS::TOBArray*> & output(const std::string & connName) const;

      StatusCode setupFromMenu(const std::map<std::string, TCS::DecisionConnector*>& outputConnectorMap, const std::map<std::string, TCS::CountingConnector*>& countConnectorMap);

      StatusCode collectResult(TCS::DecisionConnector* outputConn = nullptr, TCS::CountingConnector* countConn = nullptr );

      StatusCode reset();

      void setMsgLevel( TrigConf::MSGTC::Level lvl );

   private:
      friend std::ostream& ::operator<<(std::ostream&, const TCS::TopoCoreSimResult &);

      GlobalOutput m_globalOutput;

      // map from connector name to decision connectors
      std::map<std::string, TCS::DecisionConnector*> m_outputConnectorMap;

      // map from trigger name to decision connectors
      std::map<std::string, TCS::DecisionConnector*> m_triggerLocationDec;

      std::set<TCS::DecisionConnector*> m_outputConnectors;

      // map from connector name to counting connectors
      std::map<std::string, TCS::CountingConnector*> m_countConnectorMap;

      // map from trigger name to counting connectors
      std::map<std::string, TCS::CountingConnector*> m_triggerLocationCount;

      std::set<TCS::CountingConnector*> m_countConnectors;


   };

}


#endif
