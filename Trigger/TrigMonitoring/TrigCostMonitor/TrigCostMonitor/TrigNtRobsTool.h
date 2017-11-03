/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIG_TRIGNTROBSTOOL_H
#define TRIG_TRIGNTROBSTOOL_H

//
// Tool collects ROB data requests from ROBMonitor.
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

namespace robmonitor
{
  class ROBDataMonitorStruct;
}

namespace Trig
{
  static const InterfaceID IID_TrigNtRobsTool("Trig::TrigNtRobsTool", 1, 0); 
  
  class TrigNtRobsTool : virtual public Trig::ITrigNtTool, public AthAlgTool {
  public:
    
    static const InterfaceID& interfaceID() { return IID_TrigNtRobsTool; }
    
    TrigNtRobsTool(const std::string &, const std::string &, const IInterface *);
    virtual ~TrigNtRobsTool();
    
    StatusCode initialize();
    StatusCode finalize();

    bool Fill(TrigMonConfig *confg);
    bool Fill(TrigMonEvent &event);
    
    void CleanROB(TrigMonROB &data) const;
    void CheckROB(const TrigMonROB &data, const robmonitor::ROBDataMonitorStruct &rob) const;

  private:

    // Tool properties
    std::string                  m_keyROB;     // StoreGate key for ROB collection
    bool                         m_cleanROBs;  // Make ROB summary per request
    bool                         m_keepSubDet; // Keep subdet id
    bool                         m_printDebug; // Print debugging information

    // Tool variables
    TrigMonConfig               *m_config;
    std::unordered_map<std::string, uint32_t> m_algNameToIDMap; //Algorithm Name to ID caching
    std::set<uint32_t>           m_algIds;  
  };
}

#endif
