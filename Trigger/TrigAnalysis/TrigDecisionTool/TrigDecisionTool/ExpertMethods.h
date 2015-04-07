// -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGGER_DECISION_TOOL_ExpertMethods_H
#define TRIGGER_DECISION_TOOL_ExpertMethods_H

/**********************************************************************************
 * @Project: TrigDecisionTool
 * @Package: TrigDecisionTool
 * @Class  : ExpertMethods
 *
 * @brief access to information about the trigger decision
 *
 * @author Michael Begel   <michael.begel@cern.ch>  - Brookhaven National Laboratory
 *
 ***********************************************************************************/
#include <string>
#include <boost/algorithm/string/predicate.hpp>

#include "AnalysisTriggerEvent/Muon_ROI.h"
#include "AnalysisTriggerEvent/EmTau_ROI.h"
#include "AnalysisTriggerEvent/Jet_ROI.h"
#include "AnalysisTriggerEvent/JetET_ROI.h"
#include "AnalysisTriggerEvent/EnergySum_ROI.h"
#include "AnalysisTriggerEvent/LVL1_ROI.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"


#include "GaudiKernel/MsgStream.h"
#include "TrigNavigation/NavigationCore.h"
#include "TrigNavigation/TriggerElement.h"
#include "TrigDecisionEvent/TrigDecision.h"
#include "TrigDecisionTool/Logger.h"
#include "TrigDecisionTool/Conditions.h"
#include "TrigDecisionTool/Combination.h"
#include "TrigDecisionTool/Feature.h"
#include "TrigDecisionTool/FeatureContainer.h"
#include "TrigDecisionTool/ChainGroup.h"

#include "TrigDecisionTool/Logger.h"

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

    ExpertMethods(Trig::CacheGlobalMemory* m_cacheGlobalMemory);
    virtual ~ExpertMethods();

    /**
     * @brief enable use of Experimental or expert methods
     **/
    void enable() {useExperimentalAndExpertMethods=true;};
    /**
     * @brief disable use of Experimental or expert methods
     **/
    void disable() {useExperimentalAndExpertMethods=false;};

    /**
     * @brief return TrigConf::TriggerItem
     * @parm chain: name of the item
     **/
    const TrigConf::TriggerItem* getItemConfigurationDetails(const std::string& chain);

    /**
     * @brief return TrigConf::HLTChain
     * @parm chain: name of the chain
     **/
    const TrigConf::HLTChain* getChainConfigurationDetails(const std::string& chain);

    /**
     * @brief return LVL1CTP::Lvl1Item
     * @parm chain: name of the item
     **/
    const LVL1CTP::Lvl1Item* getItemDetails(const std::string& chain) const;

    /**
     * @brief return HLT::Chain
     * @parm chain: name of the chain
     **/
    const HLT::Chain* getChainDetails(const std::string& chain) const;

    /**
     * @brief return HLT::NavigationCore
     **/
    const HLT::NavigationCore* getNavigation() const;


    /**
     * Returns true if HLT result is truncated. This refers only to L2 for the time beeing.
     * Pure convenience method.
     **/
    bool isHLTTruncated() const;

  private:

    Trig::CacheGlobalMemory*    m_cacheGlobalMemory;    
    Trig::CacheGlobalMemory* cgm(bool onlyConfig=false) const;

    bool useExperimentalAndExpertMethods;

    bool checkExperimentalAndExpertMethods() const;

  };
  
} // End of namespace


#endif
