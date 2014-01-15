/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIG_TRIGNTANPITOOL_H
#define TRIG_TRIGNTANPITOOL_H

/**********************************************************************************
 * @Package: TrigCostAthena
 * @Class  : TrigNtAnpITool
 * @Author : Rustem Ospanov
 *
 * @Brief  :  
 *
 * Interface class for connecting TrigCostMonitor to TrigCostBase
 *  - run PhysicsNtuple algorithms within athena
 *  - set TTree for NtupleSvc for output
 *
 **********************************************************************************/

// Framework
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "StoreGate/StoreGateSvc.h"

// Trigger
#include "TrigMonitoringEvent/TrigMonConfig.h"
#include "TrigMonitoringEvent/TrigMonEvent.h"

// PhysicsNtuple
#include "TrigCostBase/AlgEvent.h"
#include "TrigCostBase/NtupleSvc.h"
#include "TrigCostBase/Registry.h"

// Local
#include "TrigCostMonitor/ITrigNtTool.h"

class TFile;
class TTree;
class ITHistSvc;

namespace Trig
{
  static const InterfaceID IID_TrigNtAnpITool("Trig::TrigNtAnpITool", 1, 0); 
  
  class TrigNtAnpITool : 
    virtual public Trig::ITrigNtTool, 
    virtual public AlgTool {
  public:
    
    static const InterfaceID& interfaceID() { return IID_TrigNtAnpITool; }
    
    TrigNtAnpITool(const std::string &, const std::string &, const IInterface *);
    virtual ~TrigNtAnpITool() {}
    
    StatusCode initialize();
    StatusCode finalize();
    
    bool Fill(TrigMonConfig *config);
    bool Fill(TrigMonEvent  &event);
    
  private:
    
    TTree* MakeTree(const std::string &tree_name, const std::string &stream_name);

    MsgStream& log() const { return *m_log; }

  private:
       
    // Tool and service handles:
    MsgStream                           *m_log;
    ServiceHandle<ITHistSvc>             m_histSvc;
    Anp::Handle<Anp::AlgEvent>           m_alg;
    
    // Properties:
    std::string                          m_algType;       // AlgEvent algorithm type
    std::string                          m_algName;       // AlgEvent algorithm name
    std::string                          m_outputFile;    // Output ROOT file
    std::string                          m_registryXML;   // Path to Registry XML file
    std::string                          m_streamConfig;  // THistSvc stream for config tree
    std::string                          m_streamEvent;   // THistSvc stream for event  tree

    // Data:    
    TFile                               *m_file;          // Output file for trees and hists
    TTree                               *m_treeConfig;    // Configuration data tree
    TTree                               *m_treeEvent;     // Event data tree

    Anp::Registry                        m_reg;           // Global Registry configuration
  };
}

#endif
