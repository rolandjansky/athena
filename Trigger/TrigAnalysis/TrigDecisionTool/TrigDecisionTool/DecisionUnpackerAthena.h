/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIG_DECISIONUNPACKERATHENA_H
#define TRIG_DECISIONUNPACKERATHENA_H

#include "TrigConfHLTData/HLTChain.h"
#include "TrigConfL1Data/CTPConfig.h"
#include "TrigSteeringEvent/Chain.h"


#include "TrigDecisionTool/IDecisionUnpacker.h"
#include "TrigDecisionTool/Logger.h"
#include "AsgTools/AsgMessaging.h"

#include "AsgDataHandles/ReadHandleKey.h"



class StoreGateSvc;

namespace TrigDec {
  class TrigDecision;
}

namespace HLT {
  class TrigNavStructure;
}

namespace LVL1CTP{
  class Lvl1Item;
  class Lvl1Result;
}

namespace Trig{  


  class DecisionObjectHandleAthena;  

  class DecisionUnpackerAthena : public IDecisionUnpacker, public Logger {
  public:
    DecisionUnpackerAthena( SG::ReadHandleKey<TrigDec::TrigDecision>* olddeckey );
    virtual ~DecisionUnpackerAthena();

    DecisionUnpackerAthena (const DecisionUnpackerAthena&) = delete;
    DecisionUnpackerAthena& operator= (const DecisionUnpackerAthena&) = delete;

    virtual StatusCode unpackDecision(std::unordered_map<std::string, const LVL1CTP::Lvl1Item*>&,
				      std::map<CTPID, LVL1CTP::Lvl1Item*>&,
				      std::unordered_map<std::string, const HLT::Chain*>&,
				      std::map<CHAIN_COUNTER, HLT::Chain*>&,
				      std::unordered_map<std::string, const HLT::Chain*>&,
				      std::map<CHAIN_COUNTER, HLT::Chain*>&,
				      char&,
				      bool
				    );
    virtual StatusCode unpackNavigation(HLT::TrigNavStructure*);
    virtual bool assert_handle();
    virtual void validate_handle();
    virtual void invalidate_handle();
  private:
    DecisionObjectHandleAthena* m_handle;
    StatusCode unpackItems(const LVL1CTP::Lvl1Result& result,
			   std::map<CTPID, LVL1CTP::Lvl1Item*>&,
			   std::unordered_map<std::string, const LVL1CTP::Lvl1Item*>&);
    StatusCode unpackChains(const std::vector<uint32_t>& serialized_chains,
			    std::map<unsigned, HLT::Chain*>& cache,
			    std::unordered_map<std::string, const HLT::Chain*>& output);

  };


} //end of Trig namespace

#endif
