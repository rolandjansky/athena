// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TRIG_IDECISIONUNPACKER_H
#define TRIG_IDECISIONUNPACKER_H

#include "AsgMessaging/StatusCode.h"
#include <map>
#include <unordered_map>
#include <string>

#include "TrigSteeringEvent/Chain.h"
#include "TrigSteeringEvent/Lvl1Item.h"

class EventContext;

namespace HLT {
  class TrigNavStructure;
}

namespace LVL1CTP{
  class Lvl1Item;
}

namespace Trig{
  class IDecisionUnpacker{
  public:
    typedef unsigned CTPID;
    typedef unsigned CHAIN_COUNTER;
    IDecisionUnpacker() = default;
    virtual ~IDecisionUnpacker() = default;
    virtual StatusCode unpackDecision(const EventContext&,
                                      std::unordered_map<std::string, const LVL1CTP::Lvl1Item*>&,
                                      std::map<CTPID, LVL1CTP::Lvl1Item>& itemsCache,
                                      std::unordered_map<std::string, const HLT::Chain*>&,
                                      std::map<CHAIN_COUNTER, HLT::Chain>&,
                                      std::unordered_map<std::string, const HLT::Chain*>&,
                                      std::map<CHAIN_COUNTER, HLT::Chain>&,
                                      char&,
                                      bool
                                      ) const = 0;
    virtual StatusCode unpackNavigation(const EventContext&, HLT::TrigNavStructure*) const = 0;
  };
} //end of Trig namespace

#endif
