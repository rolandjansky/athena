/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANP_CONFIGDB_H
#define ANP_CONFIGDB_H

/**********************************************************************************
 * @Package: TrigCostRate
 * @Class  : ConfigDB
 * @Author : Rustem Ospanov
 *
 * @Brief  : Concrete implementation of configuration algorithm.
 *
 * Algorithm reads configuration from trigger DB and COOL
 *  - for each run, lumi read from COOL: SMK, PS and BG keys
 *  - for new keys read configuration from trigger DB
 *
 **********************************************************************************/

// C/C++
#include <string>

// Trigger
#include "TrigCostMonitor/ReadConf.h"

// Local
#include "TrigCostBase/AlgEvent.h"
#include "TrigCostBase/ConfigABC.h"
#include "TrigCostBase/UtilTrig.h"

namespace Anp
{
  class ConfigDB: public virtual ConfigABC {
  public:

    ConfigDB();
    virtual ~ConfigDB();
    
    void Config(const Registry &reg);

    bool Init();

    void Exec();

    void Done();

    const TrigMonConfig& GetConfig(const std::string &key = "CURRENT") const;

    TrigMonConfig& GetConfigModifiable(const std::string modifyingAlgName,
				       const std::string &key = "CURRENT");

    std::map<std::string, Trig::BunchGroup> GetBG() const;

    void RegisterCallback(Callback *alg);

  private:

    typedef std::map<unsigned, Trig::ConfigKeys> KeyMap;
    typedef std::map<std::string, Trig::BunchGroup>    BGMap;
    typedef std::map<std::string, Trig::PSData>  PSMap;
    struct ConfData 
    {
      TrigMonConfig  config;
      BGMap          bgmap;

      void clear() 
      {
	config.clear();
	bgmap.clear();
      }
    };

    typedef std::map<std::vector<unsigned>, ConfData> ConfMap;

  private:

    void Run(unsigned run, unsigned lumi);

    void Read(unsigned run);

    void Load(TrigMonConfig &config, 
	      BGMap &bmap,
	      const Trig::ConfigKeys &key) const;

  private:

    Registry                    fReg;            // Copy of registry configuration
    Branch<TrigMonEvent>        fEntry;          // Trigger event data

    // Properties:
    bool                        fDebug;          // Print debugging info
    bool                        fUseKeys;        // Use keys to read configuration

    unsigned                    fBGK;            // BG key
    unsigned                    fSMK;            // SMK
    unsigned                    fLV1_PS;         // LV1 PS key
    unsigned                    fHLT_PS;         // HLT PS key
    
    unsigned                    fConfigRun;      // Run number from configuration
    unsigned                    fConfigLB;       // Lumi block number from configuration

    std::string                 fCoolId;         // COOL connection id
    std::string                 fTrigId;         // Trigger DB connection id
    std::string                 fUsername;       // Trigger DB username
    std::string                 fPassword;       // Trigger DB password

    // Variables
    bool                        fIsConfig;       // Has read first configuration?
    bool                        fIsFrozen;       // Allow to add callback algorithms?
    bool                        fValidConf;      // Trigger Configuration is valid?
    unsigned                    fConfigCount;    // Count configuration objects

    unsigned                    fRun;            // Current run number        
    unsigned                    fLumi;           // Current lumi block
    std::vector<unsigned>       fRunList;        // List of all run number previously called        

    KeyMap                      fKeys;           // Configuration keys for current run
    Trig::ConfigKeys            fKey;            // Current configuration keys

    ConfMap                     fConf;           // Configuration map
    ConfData                    fData;           // Current trigger configuration

    std::vector<Callback *>     fCallback;       // Algorithms for configuration callback
  };
}

#endif



