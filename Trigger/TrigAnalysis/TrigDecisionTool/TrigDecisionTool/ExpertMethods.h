// -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGGER_DECISION_TOOL_ExpertMethods_H
#define TRIGGER_DECISION_TOOL_ExpertMethods_H

/**********************************************************************************
 * @Project: TrigDecisionTool
 * @Package: TrigDecisionTool
 * @class  : ExpertMethods
 *
 * @brief access to information about the trigger decision
 *
 * @author Michael Begel   <michael.begel@cern.ch>  - Brookhaven National Laboratory
 *
 ***********************************************************************************/
#include <string>
#include <boost/algorithm/string/predicate.hpp>

#include "TrigNavStructure/TriggerElement.h"
#include "TrigDecisionTool/Logger.h"
#include "TrigDecisionTool/Conditions.h"
#include "TrigDecisionTool/Combination.h"
#include "TrigDecisionTool/Feature.h"
#include "TrigDecisionTool/FeatureContainer.h"
#include "TrigDecisionTool/ChainGroup.h"

#include "TrigDecisionTool/Logger.h"


#if !defined(XAOD_STANDALONE) && !defined(XAOD_ANALYSIS)
#include "TrigNavigation/NavigationCore.h"
#include "AthenaKernel/SlotSpecificObj.h"
#endif

namespace HLT {
  class Chain;
}
namespace TrigConf {
  class HLTChain;
  class HLTTriggerElement;
}

class LVL1_ROI;

namespace Trig {

  class CacheGlobalMemory;
   
  class ExpertMethods : public virtual Logger {
  public:      

#if !defined(XAOD_STANDALONE) && !defined(XAOD_ANALYSIS)
    ExpertMethods(SG::SlotSpecificObj<Trig::CacheGlobalMemory>* m_cacheGlobalMemory);
#else
    ExpertMethods(Trig::CacheGlobalMemory* m_cacheGlobalMemory);
#endif

    virtual ~ExpertMethods();

    /**
     * @brief enable use of Experimental or expert methods
     **/
    void enable() {m_useExperimentalAndExpertMethods=true;};
    /**
     * @brief disable use of Experimental or expert methods
     **/
    void disable() {m_useExperimentalAndExpertMethods=false;};

    /**
     * @brief return TrigConf::TriggerItem
     * @param chain: name of the item
     **/
    const TrigConf::TriggerItem* getItemConfigurationDetails(const std::string& chain);

    /**
     * @brief return TrigConf::HLTChain
     * @param chain: name of the chain
     **/
    const TrigConf::HLTChain* getChainConfigurationDetails(const std::string& chain);

    /**
     * @brief return LVL1CTP::Lvl1Item
     * @param chain: name of the item
     **/
    const LVL1CTP::Lvl1Item* getItemDetails(const std::string& chain) const;

    /**
     * @brief return HLT::Chain
     * @param chain: name of the chain
     **/
    const HLT::Chain* getChainDetails(const std::string& chain) const;

    /**
     * @brief return HLT::NavigationCore
     **/
#if !defined(XAOD_STANDALONE) && !defined(XAOD_ANALYSIS) // Full Athena
    const HLT::NavigationCore* getNavigation() const;
#else // Analysis or Standalone
    const HLT::TrigNavStructure* getNavigation() const;
#endif

    /**
     * Returns true if HLT result is truncated. This refers only to L2 for the time beeing.
     * Pure convenience method.
     **/
    bool isHLTTruncated() const;

  private:

#if !defined(XAOD_STANDALONE) && !defined(XAOD_ANALYSIS) // Full Athena
    SG::SlotSpecificObj<Trig::CacheGlobalMemory>* m_cacheGlobalMemory;
#else // Analysis or Standalone
    Trig::CacheGlobalMemory* m_cacheGlobalMemory;
#endif    

    Trig::CacheGlobalMemory* cgm(bool onlyConfig=false) const;

    bool m_useExperimentalAndExpertMethods;

    bool checkExperimentalAndExpertMethods() const;

  };
  
} // End of namespace


#endif
