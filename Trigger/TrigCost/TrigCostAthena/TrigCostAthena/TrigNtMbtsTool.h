/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIG_TRIGNTMBTSTOOL_H
#define TRIG_TRIGNTMBTSTOOL_H

/**********************************************************************************
 * @Package: TrigCostAthena
 * @Class  : TrigNtMbtsTool
 * @Author : Josh Kunkle, Rustem Ospanov
 *
 * @Brief  : Tool for recording MBTS hit information
 *
 **********************************************************************************/

// Framework
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ITHistSvc.h"
#include "StoreGate/StoreGateSvc.h"

// Detectors
#include "TileIdentifier/TileTBID.h"

// Trigger
#include "TrigCostMonitor/ITrigNtTool.h"

// Local
#include "TrigCostBase/NtupleSvc.h"
#include "TrigCostData/EventMBTS.h"

class TTree;
class ITHistSvc;

namespace Trig
{
  static const InterfaceID IID_TrigNtMbtsTool("Trig::TrigNtMbtsTool", 1, 0); 
  
  class TrigNtMbtsTool : virtual public Trig::ITrigNtTool, virtual public AlgTool {
  public:
    
    static const InterfaceID& interfaceID() { return IID_TrigNtMbtsTool; }
    
    TrigNtMbtsTool(const std::string &, const std::string &, const IInterface *);
    virtual ~TrigNtMbtsTool() {}
    
    StatusCode initialize();
    StatusCode finalize();
    
    bool Fill(TrigMonConfig *confg);
    bool Fill(TrigMonEvent &event);
    
  private:

    MsgStream& log() const { return *m_log; }

  private:
       
    // Properties
    std::string                          m_mbtsContainerName;
    bool                                 m_debug;
    float                                m_mbtsThreshold;

    // Athena tool and service handles
    MsgStream                           *m_log;
    ServiceHandle<StoreGateSvc>          m_storeGate;
    ServiceHandle<StoreGateSvc>          m_detStore;
    ServiceHandle<ITHistSvc>             m_histSvc;
    const TileTBID                      *m_tileTBID;

    // Anp interface objects:
    Anp::Branch<Anp::EventMBTS>          m_mbtsEvent;
  };
}

#endif
