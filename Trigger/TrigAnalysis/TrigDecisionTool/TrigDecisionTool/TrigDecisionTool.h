// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigDecisionTool.h 775686 2016-09-28 16:26:51Z lheinric $
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
#ifndef XAOD_STANDALONE
// Includes below are hidden from AnalysisBase
#include "AthenaBaseComps/AthMessaging.h"
#include "EventInfo/EventInfo.h"
#include "AthenaKernel/SlotSpecificObj.h"
#include "TrigConfInterfaces/ITrigConfigSvc.h" 
#include "GaudiKernel/ServiceHandle.h"

#ifndef XAOD_ANALYSIS
// Includes below are hidden from AnalysisBase and AthAnalysis
#include "GaudiKernel/IIncidentListener.h"
#include "TrigNavigation/Navigation.h"

#endif // XAOD_ANALYSIS

#endif // XAOD_STANDALONE

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
#ifndef XAOD_STANDALONE
    , public AthMessaging
#endif   
  { 
    // constructors, destructor
    ASG_TOOL_INTERFACE(Trig::TrigDecisionTool)
    ASG_TOOL_CLASS2(TrigDecisionTool,Trig::ITrigDecisionTool,Trig::TrigDecisionTool)

  public:
    using Logger::msgLvl;//resolve ambiguity from also inheriting from Logger
    using Logger::msg;   //resolve ambiguity from also inheriting from Logger

    // constructors, destructor
    TrigDecisionTool(const std::string& name);
    virtual ~TrigDecisionTool();

    // initialize routine as required for an Algorithm
    StatusCode initialize();

    StatusCode beginEvent();

    StatusCode beginInputFile();

    StatusCode finalize();

    #ifndef XAOD_STANDALONE
    void outputlevelupdateHandler(Gaudi::Details::PropertyBase& p);  //propagates outputlevel changes to the Logger
    
    #endif

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

    bool msgLvl(const MSG::Level lvl) const { return Logger::msgLvl(lvl); }

    const std::string& getNavigationFormat() const; //!< Note: Temporary

  private:
      
    bool configKeysMatch(uint32_t smk, uint32_t lvl1psk, uint32_t hltpsk);

    std::vector<uint32_t>* getKeys();

    void setForceConfigUpdate(bool b, bool forceForAllSlots = false);
    bool getForceConfigUpdate();

    ToolHandle<TrigConf::ITrigConfigTool> m_configTool{this, "ConfigTool", "TrigConf::xAODConfigTool"};    //!< trigger configuration service handle

    // XAOD_STANDALONE is AnalysisBase
    // XAOD_ANALYSIS is AnalysisBase and AthAnalysis
    #ifdef XAOD_STANDALONE // AnalysisBase

    std::vector<uint32_t>  m_configKeysCache; //!< cache for config keys. only update CacheGlobalMemory when these change 
    bool m_forceConfigUpdate; //!< Cache for registering new input files

    #else //AthAnalysis or full Athena

    ServiceHandle<TrigConf::ITrigConfigSvc> m_configSvc{this, "TrigConfigSvc", "TrigConf::xAODConfigSvc"};    //!< trigger configuration service handle

    SG::SlotSpecificObj< std::vector<uint32_t> > m_configKeysCache; //!< cache for config keys. only update CacheGlobalMemory when these change
    SG::SlotSpecificObj< std::atomic<bool> > m_forceConfigUpdate; //!< Cache for registering new input files.

    #endif

    ///

    #ifndef XAOD_ANALYSIS // full Athena

    Gaudi::Property<bool> m_useOldEventInfoDecisionFormat {this, "UseOldEventInfoDecisionFormat", false,
      "For use when reading old BS with trigger decision information available in the EventInfo"};

    SG::ReadHandleKey<EventInfo> m_oldEventInfoKey {this, "OldEventInfoKey", "EventInfo",
      "Storegate key of old pre-xAOD EventInfo object"};

    SG::ReadHandleKey<TrigDec::TrigDecision> m_oldDecisionKey {this, "OldTrigDecisionKey", "TrigDecision",
      "Storegate key of old pre-xAOD Decision object"};

    Gaudi::Property<bool> m_useRun1DecisionFormat {this, "UseAODDecision", false,
      "For use when reading old ESD/AOD with only a TrigDec::TrigDecision and no xAOD::TrigDecision"};

    ToolHandle<HLT::Navigation> m_fullNavigation;

    #endif

    Gaudi::Property<bool> m_acceptMultipleInstance{this, "AcceptMultipleInstance", false};

    SG::ReadHandleKey<xAOD::TrigNavigation> m_navigationKey {this, "NavigationKey", "TrigNavigation",
      "Storegate key of Run1, Run2 Trig Navigation"};

    /// @name Run 3 properties
    /// @{
    Gaudi::Property<std::string> m_navigationFormat{this, "NavigationFormat", "TriggerElement",
      "Allowed tokens are 'TriggerElement' or 'TrigComposite'"}; //!< Note: Temporary property

    SG::ReadHandleKey<TrigCompositeUtils::DecisionContainer> m_HLTSummaryKeyIn {this, "HLTSummary",
      "HLTNav_Summary", "HLT summary container Key"};

    SG::ReadHandleKey<xAOD::TrigDecision> m_decisionKey {this, "TrigDecisionKey", "xTrigDecision",
      "Storegate key of Trigger Decision"};
    /// @}

    TrigDecisionTool& operator= (const TrigDecisionTool&);

    Gaudi::Property< std::map<std::string,std::string> > m_publicChainGroups{this, "PublicChainGroups", 
      std::map<std::string,std::string>(), "Pre-created chain groups"};

  };

} // End of namespace


#endif
