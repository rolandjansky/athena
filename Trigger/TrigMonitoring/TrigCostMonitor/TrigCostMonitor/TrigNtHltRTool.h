/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIG_TRIGNTHLTRTOOL_H
#define TRIG_TRIGNTHLTRTOOL_H

//
// Collect HLT decisions from HLTResult
//

// Framework
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/StoreGateSvc.h"

// Trigger
#include "TrigSteering/HLTResultAccessTool.h"

// Local
#include "TrigCostMonitor/ITrigNtTool.h"

namespace Trig
{
  static const InterfaceID IID_TrigNtHltRTool("Trig::TrigNtHltRTool", 1, 0); 
  
  class TrigNtHltRTool : virtual public Trig::ITrigNtTool, public AthAlgTool {
  public:
    
    static const InterfaceID& interfaceID() { return IID_TrigNtHltRTool; }
    
    TrigNtHltRTool(const std::string &, const std::string &, const IInterface *);
    virtual ~TrigNtHltRTool() {}
    
    StatusCode initialize();
    StatusCode finalize();

    void SetSteer(const HLT::TrigSteer *ptr);
    
    bool Fill(TrigMonConfig *confg);
    bool Fill(TrigMonEvent &event);
    
  private:
    
    bool FillFromSteering(TrigMonEvent &event);
    bool FillFromHLTResult(TrigMonEvent &event);

  private:
    
    // Properties:
    std::string                           m_keyResult;        // StoreGate key to retrieve HLTResult
    bool                                  m_saveFailedChains; // Flag to save chain info even if no trigger bits are set to pass
    bool                                  m_useSteering;      // Flag to use steering info, if false then use HLT result
 
    // Tools and services:
    const HLT::TrigSteer                 *m_parentAlg;
    ToolHandle<HLT::IHLTResultAccessTool> m_hltTool;      // Helper tool for unpacking HLTResult
  };
}

#endif
