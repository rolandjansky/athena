/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANP_CONFIGABC_H
#define ANP_CONFIGABC_H

/**********************************************************************************
 * @Package: TrigCostBase
 * @Class  : ConfigABC, BunchGroup
 * @Author : Rustem Ospanov
 *
 * @Brief  : Base class for trigger configuration algorithm
 * 
 **********************************************************************************/

// C/C++
#include <map>
#include <set>

// Trigger
#include "TrigMonitoringEvent/TrigMonConfig.h"

// Local
#include "TrigCostBase/AlgEvent.h"
#include "TrigCostBase/UtilTrig.h"

namespace Anp
{
  class ConfigABC: public virtual AlgEvent {
  public:

    ConfigABC();
    virtual ~ConfigABC() {}

    virtual const TrigMonConfig& GetConfig(const std::string &key = "CURRENT") const = 0;   

    virtual TrigMonConfig& GetConfigModifiable(const std::string modifyingAlgName,
					       const std::string &key = "CURRENT") = 0;   

    virtual void RegisterCallback(Callback *alg) = 0;   

    virtual std::map<std::string, Trig::BunchGroup> GetBG() const { return fBunchMap; }
    
    virtual std::set<std::string> GetItemThr(unsigned ctpid) const;

  protected:

    void ReadConfig(const Registry &reg);
    
    void ReadItemTh(const TrigMonConfig &config);

    void UpdateConfig(TrigMonConfig &config) const;
    
  protected:
    
    typedef std::map<unsigned, std::set<std::string> > ItemThrMap;

    std::map<std::string, Trig::BunchGroup>    fBunchMap;
    std::map<std::string, Trig::PSData>  fDataPS;         // PS data read from XML
    std::vector<Trig::GrData>            fDataGr;         // Groups  read from XML
    ItemThrMap                           fItemThr;        // Map L1 CTP id -> L1 thresholds
    
    std::string                          fUpdateName;     // Name for additional config alg
    std::string                          fWriteXML;       // Path for writing XML files

    bool                                 fDebug;          // Debug flag
    bool                                 fRemoveNoPS;     // Remove triggers without defined PS/PT

    double                               fDefaultPS;      // Set default PS before doing update
    double                               fDefaultPT;      // Set default PT before doing update
    
    bool                                 fHasDefaultPS;   // Should we set default PS?
    bool                                 fHasDefaultPT;   // Should we set default PT?
    bool                                 fUseMenuGroups;  // Use groups as defined in menu?

    Handle<ConfigABC>                    fUpdateAlg;      // Algorithm provides PS/PT to update this algorithm
  }; 
}

#endif
