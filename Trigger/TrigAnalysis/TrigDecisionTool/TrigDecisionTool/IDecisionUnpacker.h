// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TRIG_IDECISIONUNPACKER_H
#define TRIG_IDECISIONUNPACKER_H

#include "AsgMessaging/StatusCode.h"
#include <map>
#include <unordered_map>
#include <string>

namespace HLT {
  class TrigNavStructure;
  class Chain;
}

namespace LVL1CTP{
  class Lvl1Item;
}

namespace Trig{
  class IDecisionUnpacker{
  public:
    typedef unsigned CTPID;
    typedef unsigned CHAIN_COUNTER;
    IDecisionUnpacker();
    virtual ~IDecisionUnpacker();
    virtual StatusCode unpackDecision(std::unordered_map<std::string, const LVL1CTP::Lvl1Item*>&,
				      std::map<CTPID, LVL1CTP::Lvl1Item*>& itemsCache,
				      std::unordered_map<std::string, const HLT::Chain*>&,
				      std::map<CHAIN_COUNTER, HLT::Chain*>&,
				      std::unordered_map<std::string, const HLT::Chain*>&,
				      std::map<CHAIN_COUNTER, HLT::Chain*>&,
				      char&,
				      bool
				      ) = 0;
    virtual StatusCode unpackNavigation(HLT::TrigNavStructure*) = 0;
    bool unpacked_decision() const { return m_unpackedDecision;}
    void unpacked_decision(bool state) {  m_unpackedDecision = state;}

    bool unpacked_navigation() const {return m_unpackedNavigation;}
    void unpacked_navigation(bool state) { m_unpackedNavigation = state;}

    virtual bool assert_handle() = 0;
    virtual void validate_handle() = 0;
    virtual void invalidate_handle() = 0;

  private:
    bool m_unpackedDecision;
    bool m_unpackedNavigation;
  };
} //end of Trig namespace

#endif
