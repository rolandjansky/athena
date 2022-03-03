/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIG_DECISIONUNPACKEREVENTINFO_H
#define TRIG_DECISIONUNPACKEREVENTINFO_H

#include "TrigConfHLTData/HLTChain.h"
#include "TrigConfL1Data/CTPConfig.h"
#include "TrigSteeringEvent/Chain.h"
#include "TrigSteeringEvent/Lvl1Item.h"

#include "TrigDecisionTool/IDecisionUnpacker.h"
#include "TrigDecisionTool/Logger.h"

#include "AsgDataHandles/ReadHandleKey.h"
#include "EventInfo/EventInfo.h"

namespace HLT {
  class TrigNavStructure;
}

namespace LVL1CTP{
  class Lvl1Result;
}

namespace Trig{  
  

  class DecisionUnpackerEventInfo : public IDecisionUnpacker, public Logger {
  public:
    DecisionUnpackerEventInfo(SG::ReadHandleKey<EventInfo>* oldEventInfoKey);
    virtual ~DecisionUnpackerEventInfo();

    DecisionUnpackerEventInfo (const DecisionUnpackerEventInfo&) = delete;
    DecisionUnpackerEventInfo& operator= (const DecisionUnpackerEventInfo&) = delete;

    virtual StatusCode unpackDecision(const EventContext&,
                                      std::unordered_map<std::string, const LVL1CTP::Lvl1Item*>&,
                                      std::map<CTPID, LVL1CTP::Lvl1Item>&,
                                      std::unordered_map<std::string, const HLT::Chain*>&,
                                      std::map<CHAIN_COUNTER, HLT::Chain>&,
                                      std::unordered_map<std::string, const HLT::Chain*>&,
                                      std::map<CHAIN_COUNTER, HLT::Chain>&,
                                      char&,
                                      bool
                                      ) const override;
    virtual StatusCode unpackNavigation(const EventContext&, HLT::TrigNavStructure*) const override;

  private:
    SG::ReadHandleKey<EventInfo>* m_eventInfoKey{nullptr};
    StatusCode unpackItems(const std::vector<uint32_t>& level1TriggerInfo,
                           std::map<CTPID, LVL1CTP::Lvl1Item>& itemsCache,
                           std::unordered_map<std::string, const LVL1CTP::Lvl1Item*>& itemsByName) const;
    StatusCode unpackChains(const std::vector<uint32_t>& chainTriggerInfo,
                            std::map<unsigned, HLT::Chain>& cache,
                            std::unordered_map<std::string, const HLT::Chain*>& output) const;

  };


} //end of Trig namespace

#endif
