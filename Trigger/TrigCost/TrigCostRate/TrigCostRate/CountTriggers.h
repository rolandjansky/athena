/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANP_COUNTTRIGGERS_H
#define ANP_COUNTTRIGGERS_H

/**********************************************************************************
 * @Package: TrigCostRate
 * @Class  : CountTriggers
 * @Author : Rustem Ospanov
 *
 * @Brief  : Algorithm for selecting events which pass required trigger
 * 
 **********************************************************************************/

// C/C++
#include <string>

// Local
#include "TrigCostBase/AlgEvent.h"
#include "TrigCostBase/NtupleSvc.h"

// Local
#include "TrigCostRate/RateChain.h"
#include "TrigCostRate/RateGroup.h"
#include "TrigCostBase/ConfigABC.h"
#include "TrigCostBase/HtmlABC.h"

namespace Anp
{
  class CountTriggers: public virtual AlgEvent, public virtual HtmlABC {
  public:

    CountTriggers();
    virtual ~CountTriggers();
    
    void Config(const Registry &reg);

    void Signal(const Registry &reg, const std::string &signal);

    bool Init();

    void Exec();

    void Done();

    bool Run(const TrigMonEvent  &event);

    void Write(HtmlNode &inode, HtmlMenu &hmenu);

    const Registry& GetIndexRegistry() const { return fRegIndex; }
    
    void Write(std::ostream &os) const;

  public:    

    struct TData {
      TData();

      float GetEff() const;

      std::string name;
      
      unsigned    level;
      unsigned    counter;
      float       ps;
      float       pt;

      unsigned    count_tav;
      unsigned    count_tbp;
      unsigned    count_tap;

      unsigned    count_raw;
      unsigned    count_phy;
    };

    //
    // Definitions
    //
    typedef std::map<uint32_t, TData>  TrigMap;

  private:

    TData& FindTData(uint32_t id);

  private:

    Branch<TrigMonEvent>        fEntry;      // Trigger data
    Handle<ConfigABC>           fConf;       // Configuration algorithm handle
    TrigMonConfig               fConfig;     // Trigger configuration

    // Properties:
    bool                        fDebug;      // Print debugging info
    bool                        fPrint;      // Print summary

    Anp::Registry               fRegIndex;   // Registry configuration for index page
    
    unsigned                    fLumiBeg;    // Range for selecting LB: event lb >= fLumiBeg
    unsigned                    fLumiEnd;    // Range for selecting LB: event lb <= fLumiEnd

    // Variables:
    unsigned                    fCountRaw;
    unsigned                    fLogicKey;   // Selection logic key: AND=1, OR=2
    unsigned                    fCountPass;  // Count events which passed accepted trigger(s)
    unsigned                    fCountFail;  // Count events which failed accepted trigger(s)
    unsigned                    fCountBuff;  // Count number of T0 events

    bool                        fCheckForL1Failure;

    TrigMap                     fTrigMap;
  };
}

#endif
