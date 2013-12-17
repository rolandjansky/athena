/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANP_READCOSTLB_H
#define ANP_READCOSTLB_H

/**********************************************************************************
 * @Package: TrigCostBase
 * @Class  : ReadCostLB
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
#include "TrigCostBase/NtupleSvc.h"

class TrigMonConfig;
class TrigMonEvent;

namespace Anp
{
  class ReadCostLB: public virtual AlgEvent {
  public:

    ReadCostLB();
    virtual ~ReadCostLB();
    
    void Config(const Registry &reg);

    void Signal(const Registry &reg, const std::string &signal);

    bool Init();

    void Exec();

    void Done();

  private:

    void Run(const TrigMonConfig &config);
    bool Run(const TrigMonEvent  &event);
    
    bool AcceptLB(unsigned lb) const;

  private:

    Branch<TrigMonConfig>       fConfg;          // Trigger configuration
    Branch<TrigMonEvent>        fEntry;          // Trigger data

    // Properties:
    bool                        fDebug;
    unsigned                    fBegLB;          // Beg LB
    unsigned                    fEndLB;          // End LB

    // Variables
    std::string                 fCurrFile;
    std::vector<std::string>    fFiles;          // Algorithms for configuration callback

    bool                        fAcceptFile;
    unsigned                    fCountPass;
    unsigned                    fCountFail;
   };
}

#endif
