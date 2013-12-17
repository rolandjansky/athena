/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANP_CONFIGALG_H
#define ANP_CONFIGALG_H

/**********************************************************************************
 * @Package: TrigCostBase
 * @Class  : ConfigAlg
 * @Author : Rustem Ospanov
 *
 * @Brief  : Concrete implementation of configuration algorithm
 *           Algorithm reads configuration from ntuples.
 *        
 **********************************************************************************/

// C/C++
#include <string>

// Local
#include "TrigCostBase/AlgEvent.h"
#include "TrigCostBase/UtilTrig.h"
#include "TrigCostBase/ConfigABC.h"

namespace Anp
{
  class ConfigAlg: public virtual ConfigABC {
  public:

    ConfigAlg();
    virtual ~ConfigAlg();
    
    void Config(const Registry &reg);

    bool Init();

    void Exec();

    void Done();

    const TrigMonConfig& GetConfig(const std::string &key = "CURRENT") const;

    TrigMonConfig& GetConfigModifiable(const std::string modifyingAlgName,
				       const std::string &key = "CURRENT");   

    void RegisterCallback(Callback *alg);

  private:

    void Set(const TrigMonConfig &config);
    void Run(const TrigMonEvent  &event);
    
    void Update(TrigMonConfig &config);

  private:

    typedef std::map<std::string, TrigMonConfig> ConfigMap;
    typedef std::map<std::string, Trig::PSData> DataPSMap;
    typedef std::vector<Trig::GrData> GroupList;

  private:

    Branch<TrigMonConfig>       fConfg;          // Trigger configuration
    Branch<TrigMonEvent>        fEntry;          // Trigger data

    // Properties:
    

    // Variables
    bool                        fIsConfig;       // Has read first configuration?
    bool                        fIsFrozen;       // Are we allowed to add more callbacks?
    unsigned                    fConfigCount;    // Count configuration objects
        
    ConfigMap                   fConfig;         // Map of all configuration objects
    TrigMonConfig               fDefault;        // Current configuration

    std::vector<Callback *>     fCallback;       // Algorithms for configuration callback
  };
}

#endif
