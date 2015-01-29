/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIG_TRIGNTVARSTOOL_H
#define TRIG_TRIGNTVARSTOOL_H

//
// Collect extra variables for offline trigger analysis:
//  - number of interactions in pileup MC
//  - trigger decision bit using TrigDecisionTool
//  - some MC truth data
//

// Framework
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"

// Trigger
#include "TrigDecisionTool/TrigDecisionTool.h"

// Local
#include "TrigCostMonitor/ITrigNtTool.h"

namespace Trig
{
  static const InterfaceID IID_TrigNtVarsTool("Trig::TrigNtVarsTool", 1, 0); 
  
  class TrigNtVarsTool : virtual public Trig::ITrigNtTool, virtual public AthAlgTool {
  public:
    
    static const InterfaceID& interfaceID() { return IID_TrigNtVarsTool; }
    
    TrigNtVarsTool(const std::string &, const std::string &, const IInterface *);
    virtual ~TrigNtVarsTool() {}
    
    StatusCode initialize();
    StatusCode finalize();
    
    bool Fill(TrigMonConfig *confg);
    bool Fill(TrigMonEvent &event);
    
  private:

    bool CollectTD(TrigMonEvent &event);
    bool CollectMC(TrigMonEvent &event);
    bool CollectPU(TrigMonEvent &event);

  private:
       
    // Tool properties
    bool          m_collectMC;
    bool          m_collectPU;
    bool          m_collectTD;   
    std::string   m_keyMCEvent;
    std::string   m_keyPileUp;

    // Athena tool and service handles
    ToolHandle<Trig::TrigDecisionTool>   m_trigDec;

    // Tool variables
    TrigMonConfig                       *m_config;
  };
}

#endif
