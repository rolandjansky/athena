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
#include "GaudiKernel/AlgTool.h"
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
  
  class TrigNtRobsTool : virtual public Trig::ITrigNtTool, public AlgTool {
  public:
    
    static const InterfaceID& interfaceID() { return IID_TrigNtRobsTool; }
    
    TrigNtRobsTool(const std::string &, const std::string &, const IInterface *);
    virtual ~TrigNtRobsTool();
    
    StatusCode initialize();
    StatusCode finalize();

    bool Fill(TrigMonConfig *confg);
    bool Fill(TrigMonEvent &event);
    
  private:

    MsgStream& log() const { return *m_log; }
    
    void CleanROB(TrigMonROB &data) const;
    void CheckROB(const TrigMonROB &data, robmonitor::ROBDataMonitorStruct &rob) const;

  private:

    // Tool properties
    std::string                  m_keyROB;     // StoreGate key for ROB collection
    bool                         m_cleanROBs;  // Make ROB summary per request
    bool                         m_keepSubDet; // Keep subdet id
    bool                         m_printDebug; // Print debugging information

    // Tool variables
    MsgStream                   *m_log;
    ServiceHandle<StoreGateSvc>  m_storeGate;
    TrigMonConfig               *m_config;

    std::set<uint32_t>           m_algIds;
  };
}

#endif
