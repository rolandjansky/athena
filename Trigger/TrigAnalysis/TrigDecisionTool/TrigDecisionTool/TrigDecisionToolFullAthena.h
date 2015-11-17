/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAOD_ANALYSIS

// Dear emacs, this is -*- c++ -*-
// $Id: TrigDecisionTool.h 672848 2015-06-05 15:55:58Z lheinric $
#ifndef TrigDecision_TrigDecisionToolFullAthena_h
#define TrigDecision_TrigDecisionToolFullAthena_h
/**********************************************************************************
 * @Project: TrigDecisionTool
 * @Package: TrigDecisionTool
 * @Class  : TrigDecisionTool
 *
 * @brief main tool
 *
 * @author Michael Begel  <michael.begel@cern.ch>  - Brookhaven National Laboratory
 * @author Tomasz Bold    <Tomasz.Bold@cern.ch>    - UC Irvine - AGH Krakow
 * @author Joerg Stelzer  <Joerg.Stelzer@cern.ch>  - DESY
 * @author Lukas Heinrich <lukas.heinrich@cern.ch> - NYU
 *
 ***********************************************************************************/
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IIncidentListener.h"
#include "StoreGate/DataHandle.h"
#include "StoreGate/StoreGateSvc.h"

// interface to implement for offline access (outside AtlasTrigger)
#include "TrigDecisionInterface/ITrigDecisionTool.h"

// base classes
#include "TrigDecisionTool/TrigDecisionToolCore.h"
#include "TrigDecisionTool/Logger.h"

#include "TrigNavigation/Navigation.h"
#include "TrigConfInterfaces/ITrigConfigSvc.h" 


namespace TrigConf {
   class ITrigConfigSvc;
}

namespace Trig {
  static const InterfaceID IID_TrigDecisionTool("Trig::TrigDecisionTool", 1, 0);

  class TrigDecisionTool :
    public AthAlgTool,
    virtual Trig::ITrigDecisionTool,
    virtual public IIncidentListener,
    public TrigDecisionToolCore,
    public virtual Logger
      
  { 
      
  public:
    using Logger::msgLvl;//resolve ambiguity from also inheriting from Logger
    using Logger::msg;   //resolve ambiguity from also inheriting from Logger

    // constructors, destructor
    TrigDecisionTool(const std::string& name, const std::string& type,
                     const IInterface* parent = 0);
    virtual ~TrigDecisionTool();

    // initialize routine as required for an Algorithm
    StatusCode initialize();

    StatusCode beginEvent();

    StatusCode finalize();

    #ifdef ASGTOOL_ATHENA
    void outputlevelupdateHandler(Property& p);  //propagates outputlevel changes to the Logger
    // this is called by Incident dispatcher
    virtual void handle(const Incident& inc); 
    #endif
      
    static const InterfaceID& interfaceID() { return IID_TrigDecisionTool; }
      
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
      
    ServiceHandle<TrigConf::ITrigConfigSvc> m_configSvc;    //!< trigger configuration service handle

    ToolHandle<HLT::Navigation> m_navigation;

    
    bool m_useEventInfoDecision;
    bool m_useAODDecision;
    std::string m_decisionKey;

    TrigDecisionTool& operator= (const TrigDecisionTool&);

    std::map<std::string, std::string> m_publicChainGroups;

  };

} // End of namespace


#endif

#endif //XAOD_ANALYSIS
