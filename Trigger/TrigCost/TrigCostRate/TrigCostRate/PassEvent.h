/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANP_PASSEVENT_H
#define ANP_PASSEVENT_H

/**********************************************************************************
 * @Package: TrigCostRate
 * @Class  : PassEvent
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

namespace Anp
{
  class PassEvent: public virtual AlgEvent {
  public:

    PassEvent();
    virtual ~PassEvent();
    
    void Config(const Registry &reg);

    void Signal(const Registry &reg, const std::string &signal);

    bool Init();

    void Exec();

    void Done();

    void Set(const TrigMonConfig &config);
    bool Run(const TrigMonEvent  &event);
    bool EventLimit(const TrigMonEvent  &event);

  private:    

    Branch<TrigMonEvent>        fEntry;      // Trigger configuration
    Handle<ConfigABC>           fConf;       // Configuration algorithm handle

    // Properties:
    bool                        fDebug;      // Print debugging info
    bool                        fPrint;      // Print summary
    std::string                 fLogic;      // Selection logic: AND or OR
    std::vector<std::string>    fAccept;     // Triggers required for accepting event
    std::vector<std::string>    fVeto;       // Triggers anti-required for accepting event
    std::vector<unsigned>       fEvents;     // Number of events accepted per lumi block
    
    unsigned                    fMaxEvts;    // Max number of events allowed per lumi block
    unsigned                    fLumiBeg;    // Range for selecting LB: event lb >= fLumiBeg
    unsigned                    fLumiEnd;    // Range for selecting LB: event lb <= fLumiEnd

    // Variables:
    unsigned                    fLogicKey;   // Selection logic key: AND=1, OR=2
    unsigned                    fCountPass;  // Count events which passed accepted trigger(s)
    unsigned                    fCountFail;  // Count events which failed accepted trigger(s)
    unsigned                    fCountBuff;  // Count number of T0 events

    TrigMonL1Item::Decision     fDecisionBitLV1; // Decision bit used for LV1 triggers
    TrigMonChain::Decision      fDecisionBitHLT; // Decision bit used for HLT triggers

    std::set<unsigned>          fAcceptIds;  // Encoded ids for accept triggers    
    std::set<unsigned>          fVetoIds;    // Encoded ids for accept triggers    
  };
}

#endif
