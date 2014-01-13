/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANP_SIMLPS_H
#define ANP_SIMLPS_H

/**********************************************************************************
 * @Package: TrigCostRate
 * @Class  : SimlPS
 * @Author : Rustem Ospanov, Elliot Lipeles
 *
 * @Brief  : Algorithm for randomly applying PS/PT for current Branch<TrigMonEvent>
 * 
 **********************************************************************************/

// C/C++
#include <map>
#include <set>
#include <string>

// Trigger
#include "TrigMonitoringEvent/TrigMonConfig.h"
#include "TrigMonitoringEvent/TrigMonEvent.h"

// Local
#include "TrigCostBase/AlgEvent.h"
#include "TrigCostBase/NtupleSvc.h"

// Local
#include "TrigCostBase/ConfigABC.h"

class TRandom;

namespace Anp
{
  class SimlPS: public virtual AlgEvent {
  public:

    SimlPS();
    virtual ~SimlPS();
    
    void Config(const Registry &reg);

    void Signal(const Registry &reg, const std::string &signal);

    bool Init();

    void Exec();

    void Done();
   
  private:
    
    void Run(TrigMonEvent &event);

    void Set(const TrigMonConfig &config);

    void ComputeNextLevel(unsigned id);

    void InsertPT(std::vector<uint32_t> &hlt);

  public:

    struct CpsGroup;

    struct Data {
      
      Data() : encoded(0), ps_inv(0.0), pt_inv(0.0), cps(0), cps_passed(false) {}
      
      std::set<unsigned> next_ids;
      std::set<unsigned> pass_ids;

      unsigned    encoded;
      double      ps_inv;
      double      pt_inv;
     
      CpsGroup*   cps;
      bool        cps_passed;
    };
    
    struct CpsGroup : public Base {
      
      CpsGroup() {}
      virtual ~CpsGroup() {}

      
      std::vector< Data* > chains;

    };

    typedef std::map< std::string , Handle<CpsGroup> > CpsMap;

  private:

    TRandom                    *fRandom;         // Random number generator
    Handle<ConfigABC>           fConf;           // Configuration algorithm

    bool                        fDebug;          // Print debugging info
    bool                        fIsConfig;       // Is configured?
    unsigned                    fSeed;           // Random seed

    Branch<TrigMonEvent>        fEntry;          // Trigger data   

    TrigMonL1Item::Decision     fDecisionBitLV1; // Decision bit used for LV1 rates
    TrigMonChain::Decision      fDecisionBitHLT; // Decision bit used for HLT rates

    std::map<unsigned, Data>    fChains;         // Encoded chain id -> PS and PT values    

    CpsMap                      fCpsGroups;      // Coherent Prescale Groups map: group name->CpsGroup
        
    std::set<unsigned>          fPassPS;         // Chains which passed PS at L2 and/or EF
    std::set<unsigned>          fPassPT;         // Chains which passed PT at L2 and/or EF

  };
}

#endif
