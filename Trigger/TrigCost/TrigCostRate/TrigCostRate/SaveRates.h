/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANP_SAVERATES_H
#define ANP_SAVERATES_H

/**********************************************************************************
 * @Package: TrigCostRate
 * @Class  : SaveRates
 * @Author : Rustem Ospanov
 *
 * @Brief  : Filter trigger decisions and write them out as a compact tree
 * 
 **********************************************************************************/

// C/C++
#include <string>

// Local
#include "TrigCostBase/AlgEvent.h"
#include "TrigCostBase/NtupleSvc.h"

namespace Anp
{
  class SaveRates: public virtual AlgEvent {
  public:

    SaveRates();
    virtual ~SaveRates();
    
    void Config(const Registry &reg);

    void Signal(const Registry &reg, const std::string &signal);
   
    bool Init();

    void Exec();

    void Done();

  private:

    Branch<TrigMonConfig>       fConfg;      // Trigger configuration
    Branch<TrigMonEvent>        fEntry;      // Trigger data
    Branch<TrigMonEvent>        fOnlineEntry;// Online trigger data

    // Properties:
    bool                        fDebug;      // Print debugging info
    
    TFile                      *fFile;
    TTree                      *fTreeConfig;
    TTree                      *fTreeEvent;
    TTree                      *fTreeOnline;

    TrigMonConfig              *fConfig;
    TrigMonEvent               *fEvent;
    TrigMonEvent               *fOnline;
  };
}

#endif
