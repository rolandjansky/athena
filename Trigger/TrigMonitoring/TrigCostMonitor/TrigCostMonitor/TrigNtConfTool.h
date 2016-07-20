/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIG_TRIGNTCONFGTOOL_H
#define TRIG_TRIGNTCONFGTOOL_H

//
// Tool collects trigger configuration from TrigConfigSvc
//

// C/C++
#include <string>

// Framework
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/StoreGateSvc.h"

// Trigger
// #include "TrigConfigSvc/ITrigConfigSvc.h" TimM Depricated
#include "TrigConfInterfaces/ITrigConfigSvc.h"

// Local
#include "TrigCostMonitor/ITrigNtTool.h"
#include "TrigCostMonitor/ReadConf.h"

namespace TrigConf
{
  class IStorageMgr;
  class HLTFrame;
}

namespace Trig
{
  static const InterfaceID IID_TrigNtConfTool("Trig::TrigNtConfTool", 1, 0); 

  class TrigNtConfTool : virtual public Trig::ITrigNtTool, public AthAlgTool {
  public:
    
    static const InterfaceID& interfaceID() { return IID_TrigNtConfTool; }

    TrigNtConfTool(const std::string &, const std::string &, const IInterface *);
    virtual ~TrigNtConfTool();
    
    StatusCode initialize();
    StatusCode finalize();
    
    bool Fill(TrigMonConfig *);
    bool Fill(TrigMonEvent &);

    void SetOption(unsigned option) { m_dbOrConfSvcPass = option; }


    
  private:

    bool GetKeysChangedFromSv(TrigMonConfig &confg);
    bool GetKeysChangedFromDB(unsigned run, unsigned lumi);
   
    bool ReadFromSv(TrigMonConfig &confg);

    bool ReadFromDB(TrigMonConfig &confg, unsigned run, unsigned lumi);
    bool ReadKeysDB(unsigned run);

    uint32_t GetL1PSK(bool updateValue = false);

  private:

    typedef std::map<Trig::ConfigKeys, TrigMonConfig> ConfigMap;

  private:
    
    ServiceHandle<TrigConf::ITrigConfigSvc> m_configSvc;
    
    // Properties 
    bool             m_printConfig;          // Print trigger configuration
    bool             m_useDB;                // Enable reading of configuration from TriggerDB and COOL
    bool             m_useConfSvc;           // Enable reading of configuratuon from Conf Service

    std::string      m_connectionCool;       // COOL connection string
    std::string      m_connectionTrig;       // Trigger DB connection
    std::string      m_username;             // Trigger DB username
    std::string      m_password;             // Trigger DB password
    
    std::string      m_triggerMenuSetup;     // Menu setup key   - set in python
    std::string      m_L1PrescaleSet;        // L1  prescale key - set in python
    std::string      m_HLTPrescaleSet;       // HLT prescale key - set in python

    // Variables
    TrigConf::IStorageMgr         *m_storage;
    TrigConf::HLTFrame            *m_hltFrame;

    TrigMonConfig                 *m_config_db;  // Internal record of last DB config
    TrigMonConfig                 *m_config_sv;  // Internal record of last conf service condif
    unsigned                       m_countConfig_db;
    unsigned                       m_countConfig_sv;
    unsigned                       m_run;
    
    std::map<unsigned, ConfigKeys> m_keys;
    ConfigKeys                     m_currentKey;  // Map: lumi block -> Trigger config keys
    ConfigMap                      m_configMap;   // Map: Trigger config keys -> TrigMonConfig

    unsigned                       m_dbOrConfSvcPass; // This should be set to 1 for ConfigService pass or 2 for DB pass 
    unsigned                       m_currentL1PSK; // Cache current L1 PSK as read from CTP_RD0 online
  };
}

#endif
