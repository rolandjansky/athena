/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANP_READROB_H
#define ANP_READROB_H

/**********************************************************************************
 * @Package: TrigCostExec
 * @Class  : ReadRob
 * @Author : Rustem Ospanov
 *
 * @Brief  : Algorithm for reading ROB configuration data into RobService
 *           - read all TrigMonConfig entries
 *
 **********************************************************************************/

// Trigger
#include "TrigMonitoringEvent/TrigMonConfig.h"

// Local
#include "TrigCostBase/AlgEvent.h"

namespace Anp
{
  class ReadRob: public virtual AlgEvent {
  public:

    ReadRob();
    virtual ~ReadRob();
    
    void Config(const Registry &reg);

    bool Init();

    void Exec();

    void Done();

  private:

    Branch<TrigMonConfig>       fConfig;          // Trigger configuration
    bool                        fDebug;           // Orint debug info?
  };
}

#endif
