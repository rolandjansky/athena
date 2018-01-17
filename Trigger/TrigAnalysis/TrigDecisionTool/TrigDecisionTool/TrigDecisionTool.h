// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

#include "AsgTools/AsgToolsConf.h"
#include "AsgTools/AsgMetadataTool.h"
#include "AsgTools/ToolHandle.h"

#include "TrigConfInterfaces/ITrigConfigTool.h" 
#ifdef ASGTOOL_ATHENA
#include "AthenaBaseComps/AthMessaging.h"


#ifndef XAOD_ANALYSIS
#include "TrigNavigation/Navigation.h"
#include "TrigConfInterfaces/ITrigConfigSvc.h" 
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/ServiceHandle.h"
#endif

#endif


// interface to implement for offline access (outside AtlasTrigger)
#include "TrigDecisionInterface/ITrigDecisionTool.h"

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
#ifdef ASGTOOL_ATHENA
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

    #ifdef ASGTOOL_ATHENA
    void outputlevelupdateHandler(Property& p);  //propagates outputlevel changes to the Logger
    
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

  private:
      
    bool configKeysMatch(uint32_t smk, uint32_t lvl1psk, uint32_t hltpsk);
    std::vector<uint32_t> m_configKeysCache; //cache for config keys. only update CacheGlobalMemory when these change
    bool m_configKeysCached; // flag to indicate if we have ever cached config keys (set to true on first caching)
    ToolHandle<TrigConf::ITrigConfigTool> m_configTool;    //!< trigger configuration service handle

 

    //full Athena
    #if defined(ASGTOOL_ATHENA) && !defined(XAOD_ANALYSIS)
    ServiceHandle<TrigConf::ITrigConfigSvc> m_configSvc;    //!< trigger configuration service handle
    ToolHandle<HLT::Navigation> m_fullNavigation;
    #endif
    HLT::TrigNavStructure* m_navigation;
    
    bool m_acceptMultipleInstance;
    bool m_useAODDecision;
    std::string m_decisionKey;

    TrigDecisionTool& operator= (const TrigDecisionTool&);

    std::map<std::string, std::string> m_publicChainGroups;

  };

} // End of namespace


#endif
