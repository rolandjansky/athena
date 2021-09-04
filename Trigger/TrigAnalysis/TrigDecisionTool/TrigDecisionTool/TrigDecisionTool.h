// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigDecision_TrigDecisionTool_h
#define TrigDecision_TrigDecisionTool_h
/**********************************************************************************
 * @Project: TrigDecisionTool
 * @Package: TrigDecisionTool
 * @class  : TrigDecisionTool
 *
 * @brief main tool
 *
 * @author Michael Begel  <michael.begel@cern.ch>  - Brookhaven National Laboratory
 * @author Tomasz Bold    <Tomasz.Bold@cern.ch>    - UC Irvine - AGH Krakow
 * @author Joerg Stelzer  <Joerg.Stelzer@cern.ch>  - DESY
 *
 ***********************************************************************************/

#include "AsgTools/AsgMetadataTool.h"
#include "AsgTools/ToolHandle.h"
#include "AsgTools/PropertyWrapper.h"

#include "TrigConfInterfaces/ITrigConfigTool.h"
#include "TrigNavStructure/StandaloneNavigation.h"

#ifndef XAOD_STANDALONE
#include "EventInfo/EventInfo.h"

#ifndef XAOD_ANALYSIS
#include "TrigNavigation/Navigation.h"
#include "TrigConfInterfaces/ITrigConfigSvc.h" 
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaKernel/SlotSpecificObj.h"

#endif

#endif

#include "TrigCompositeUtils/TrigCompositeUtils.h"

// interface to implement for offline access (outside AtlasTrigger)
#include "TrigDecisionInterface/ITrigDecisionTool.h"

#include "xAODTrigger/TrigDecision.h"
#include "xAODTrigger/TrigNavigation.h"

// base classes
#include "TrigDecisionTool/TrigDecisionToolCore.h"
#include "TrigDecisionTool/Logger.h"

namespace TrigConf {
   class ITrigConfigSvc;
}

namespace Trig {
  class TrigDecisionTool :
    public asg::AsgMetadataTool,
    virtual Trig::ITrigDecisionTool,
    public TrigDecisionToolCore
  {
    // constructors, destructor
    ASG_TOOL_INTERFACE(Trig::TrigDecisionTool)
    ASG_TOOL_CLASS2(TrigDecisionTool,Trig::ITrigDecisionTool,Trig::TrigDecisionTool)

  public:
    // resolve ambiguity and use Logger adaptor for all messaging
    using Logger::msgLvl;
    using Logger::msg;

    // constructors
    TrigDecisionTool(const std::string& name);

    // initialize routine as required for an Algorithm
    StatusCode initialize();

    StatusCode beginEvent();

    StatusCode beginInputFile();

    StatusCode finalize();

#ifndef XAOD_ANALYSIS
    // this is called by Incident dispatcher
    virtual void handle(const Incident& inc); 
#endif

    /**
     * @brief true if given chain passed
     * @see DecisionAccess::isPassed(const Trig::ChainGroup*, unsigned int)
     * @see Trig::DecisionAccess
     * @see Trig::ITrigDecisionTool
     **/ 
    virtual bool isPassed(const std::string& chain, 
                          unsigned int condition) const;
    /// Implementation of the ITrigDecisionTool function
    virtual bool isPassed(const std::string& chain) const;
    /// Implementation of the ITrigDecisionTool function
    virtual unsigned int isPassedBits( const std::string& chain ) const;
    
    using TrigDecisionToolCore::isPassed;
    using TrigDecisionToolCore::isPassedBits;

    const std::string& getNavigationFormat() const; //!< Note: Temporary

  private:
      
    bool configKeysMatch(uint32_t smk, uint32_t lvl1psk, uint32_t hltpsk);

    std::vector<uint32_t>* getKeys();

    void setForceConfigUpdate(bool b, bool forceForAllSlots = false);
    bool getForceConfigUpdate();


    #if !defined(XAOD_STANDALONE) && !defined(XAOD_ANALYSIS) // Athena: Do not set a Config Tool by default (this tool is not thread safe, the service should be used in Athena)
    ToolHandle<TrigConf::ITrigConfigTool> m_configTool{this, "ConfigTool", ""}; 
    #else // AnalysisBase: Do set a Config Tool by default for analysis convienience 
    ToolHandle<TrigConf::ITrigConfigTool> m_configTool{this, "ConfigTool", "TrigConf::xAODConfigTool"};
    #endif

    //full Athena
    #if !defined(XAOD_STANDALONE) && !defined(XAOD_ANALYSIS)
    ServiceHandle<TrigConf::ITrigConfigSvc> m_configSvc{this, "TrigConfigSvc", "TrigConf::xAODConfigSvc"};    //!< trigger configuration service handle
    ToolHandle<HLT::Navigation> m_fullNavigation{this, "Navigation", "HLT::Navigation/Navigation"};

    Gaudi::Property<bool> m_useOldEventInfoDecisionFormat {this, "UseOldEventInfoDecisionFormat", false,
      "For use when reading old BS with trigger decision information available in the EventInfo"};

    SG::ReadHandleKey<EventInfo> m_oldEventInfoKey {this, "OldEventInfoKey", "EventInfo",
      "Storegate key of old pre-xAOD EventInfo object"};

    SG::ReadHandleKey<TrigDec::TrigDecision> m_oldDecisionKey {this, "OldTrigDecisionKey", "TrigDecision",
      "Storegate key of old pre-xAOD Decision object"};

    Gaudi::Property<bool> m_useRun1DecisionFormat {this, "UseAODDecision", false,
      "For use when reading old ESD/AOD with only a TrigDec::TrigDecision and no xAOD::TrigDecision"};

    SG::SlotSpecificObj< std::vector<uint32_t> > m_configKeysCache; //!< cache for config keys. only update CacheGlobalMemory when these change
    SG::SlotSpecificObj< std::atomic<bool> > m_forceConfigUpdate; //!< Cache for registering new input files.

    #else // Analysis or standalone 

    std::vector<uint32_t>  m_configKeysCache; //!< cache for config keys. only update CacheGlobalMemory when these change 
    bool m_forceConfigUpdate; //!< Cache for registering new input files

    #endif

    HLT::StandaloneNavigation m_standaloneNavigation;

    Gaudi::Property<bool> m_acceptMultipleInstance{this, "AcceptMultipleInstance", false,
      "Allow multiple TrigDecisionTool instances"};

    SG::ReadHandleKey<xAOD::TrigNavigation> m_navigationKey {this, "NavigationKey", "TrigNavigation",
      "Storegate key of Run1, Run2 Trig Navigation"};

    /// @name Run 3 properties
    /// @{
    Gaudi::Property<std::string> m_navigationFormat{this, "NavigationFormat", "TriggerElement",
      "Allowed tokens are 'TriggerElement' or 'TrigComposite'"}; //!< Note: Temporary property

    SG::ReadHandleKey<TrigCompositeUtils::DecisionContainer> m_HLTSummaryKeyIn {this, "HLTSummary",
      "HLTNav_Summary_OnlineSlimmed", "HLT summary container Key"};

    SG::ReadHandleKey<xAOD::TrigDecision> m_decisionKey {this, "TrigDecisionKey", "xTrigDecision",
      "Storegate key of Trigger Decision"};
    /// @}

    TrigDecisionTool& operator= (const TrigDecisionTool&);

    Gaudi::Property< std::map<std::string,std::string> > m_publicChainGroups{this, "PublicChainGroups", 
      std::map<std::string,std::string>(), "Pre-created chain groups"};

  };

} // End of namespace


#endif
