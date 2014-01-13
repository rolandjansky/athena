/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANP_SIMLABC_H
#define ANP_SIMLABC_H

/**********************************************************************************
 * @Package: TrigCostRate
 * @Class  : SimlABC
 * @Author : Rustem Ospanov
 *
 * @Brief  : Interface class for adding "simulated" triggers to rates calculation
 * 
 **********************************************************************************/

// Local
#include "TrigCostBase/Handle.h"
#include "TrigMonitoringEvent/TrigMonConfig.h"
#include "TrigMonitoringEvent/TrigMonEvent.h"

namespace Anp
{
  class SimlABC: virtual public Base {
  public:
    
    SimlABC() {}
    virtual ~SimlABC() {}
    
    virtual const std::vector<TrigMonChain>&  GetBits() const = 0;
    virtual const std::vector<TrigConfChain>& GetConf() const = 0;

  private:
    
  };
}

#endif
