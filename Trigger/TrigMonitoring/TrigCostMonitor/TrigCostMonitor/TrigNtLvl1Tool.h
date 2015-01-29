/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIG_TRIGNTLVL1TOOL_H
#define TRIG_TRIGNTLVL1TOOL_H

//
// Collect L1 items and rois
//

// Framework
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/StoreGateSvc.h"

// Local
#include "TrigCostMonitor/ITrigNtTool.h"

namespace LVL1CTP
{
  class Lvl1Result;
}

namespace Trig
{
  static const InterfaceID IID_TrigNtLvl1Tool("Trig::TrigNtLvl1Tool", 1, 0); 
  
  class TrigNtLvl1Tool : virtual public Trig::ITrigNtTool, public AthAlgTool {
  public:
    
    static const InterfaceID& interfaceID() { return IID_TrigNtLvl1Tool; }
    
    TrigNtLvl1Tool(const std::string &, const std::string &, const IInterface *);
    virtual ~TrigNtLvl1Tool() {}
    
    StatusCode initialize();
    StatusCode finalize();
    
    bool Fill(TrigMonConfig *confg);
    bool Fill(TrigMonEvent &event);
    
  private:
        
    bool FillFromL1Result(TrigMonEvent &event);
    bool FillFromRBResult(TrigMonEvent &event);

    bool Fill(const LVL1CTP::Lvl1Result &l1Result, TrigMonEvent &event);
    
  private:
    
    // Tool properties
    std::string  m_keyL1Result;      // Key to retrieve the L1Result from SG
    std::string  m_keyRBResult;      // Key to retrieve the RoIBResult from SG

  };
}

#endif
