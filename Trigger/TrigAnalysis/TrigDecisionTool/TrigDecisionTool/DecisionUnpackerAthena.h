/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIG_DECISIONUNPACKERATHENA_H
#define TRIG_DECISIONUNPACKERATHENA_H

#include "TrigConfHLTData/HLTChain.h"
#include "TrigConfL1Data/CTPConfig.h"
#include "TrigSteeringEvent/Chain.h"
#include "TrigSteeringEvent/Lvl1Item.h"

#include "TrigDecisionTool/IDecisionUnpacker.h"
#include "TrigDecisionTool/Logger.h"

#include "AsgDataHandles/ReadHandleKey.h"

namespace TrigDec {
  class TrigDecision;
}

namespace HLT {
  class TrigNavStructure;
}

namespace LVL1CTP{
  class Lvl1Result;
}

namespace Trig{  

  class DecisionUnpackerAthena : public IDecisionUnpacker, public Logger {
  public:
    DecisionUnpackerAthena( SG::ReadHandleKey<TrigDec::TrigDecision>* olddeckey );
    virtual ~DecisionUnpackerAthena();

    DecisionUnpackerAthena (const DecisionUnpackerAthena&) = delete;
    DecisionUnpackerAthena& operator= (const DecisionUnpackerAthena&) = delete;

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
    /// Key of the trigger decision object in the event
    SG::ReadHandleKey<TrigDec::TrigDecision>* m_deckey{nullptr};

    StatusCode unpackItems(const LVL1CTP::Lvl1Result& result,
               std::map<CTPID, LVL1CTP::Lvl1Item>&,
               std::unordered_map<std::string, const LVL1CTP::Lvl1Item*>&) const;
    StatusCode unpackChains(const std::vector<uint32_t>& serialized_chains,
                std::map<unsigned, HLT::Chain>& cache,
                std::unordered_map<std::string, const HLT::Chain*>& output) const;
  };


} //end of Trig namespace

#endif
