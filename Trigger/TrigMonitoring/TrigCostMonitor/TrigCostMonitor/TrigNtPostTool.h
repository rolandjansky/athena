/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIG_TRIGNTPOSTTOOL_H
#define TRIG_TRIGNTPOSTTOOL_H

//
// Tool collects data during post-processing
//

// C/C++
#include <set>

// Framework
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/StoreGateSvc.h"

// Local
#include "TrigMonitoringEvent/TrigMonConfig.h"
#include "TrigMonitoringEvent/TrigMonEvent.h"
#include "TrigCostMonitor/ITrigNtTool.h"

namespace Trig
{
  static const InterfaceID IID_TrigNtPostTool("Trig::TrigNtPostTool", 1, 0); 
  
  class TrigNtPostTool : virtual public Trig::ITrigNtTool, public AthAlgTool {
  public:
    
    static const InterfaceID& interfaceID() { return IID_TrigNtPostTool; }
    
    TrigNtPostTool(const std::string &, const std::string &, const IInterface *);
    virtual ~TrigNtPostTool();
    
    StatusCode initialize();
    StatusCode finalize();

    bool Fill(TrigMonConfig *config);
    bool Fill(TrigMonEvent &event);

  private:

    TrigMonConfig               *m_config;   // Current trigger configuration

    std::set<unsigned>           m_robIds;   // Set of all processed ROB ids
    std::set<unsigned>           m_detIds;   // Set of all processed sub-detectors
  };
}

#endif
