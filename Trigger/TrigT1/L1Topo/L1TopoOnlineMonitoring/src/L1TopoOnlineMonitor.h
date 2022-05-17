/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef L1TOPOONLINEMONITORING_L1TopoOnlineMonitor_h
#define L1TOPOONLINEMONITORING_L1TopoOnlineMonitor_h

// Trigger includes
#include "xAODTrigger/L1TopoSimResultsContainer.h"


// Athena includes
#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "AthenaMonitoringKernel/Monitored.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"

#include "GaudiKernel/LockedHandle.h"


// System includes

/**
 *  @class L1TopoOnlineMonitor
 *  @brief Algorithm to monitor L1Topo transmission and simulation errors for the (Run-3) L1Topo boards
 *  @author Anil Sonay
 *
 *  This algorithm tend to catch L1Topo sim xAOD to record decision bits into online monitoring
 *  Decision bits could be from Legacy, phase1 simulations or hardware
 **/
class L1TopoOnlineMonitor : public AthMonitorAlgorithm {
public:
  L1TopoOnlineMonitor(const std::string& name, ISvcLocator* svcLoc);
  
  // ------------------------- AthReentrantAlgorithm methods -------------------
  virtual StatusCode initialize() override;
  virtual StatusCode fillHistograms( const EventContext& ctx ) const override;

private:
  // ------------------------- Private types and constants ---------------------
  
  // ------------------------- Properties and handles --------------------------
  ToolHandle<GenericMonitoringTool> m_monTool {
    this, "MonTool", "" ,
    "Monitoring tool to create online histograms"};
  Gaudi::Property<bool> m_doSimMon {
    this, "doSimMon", true, "Enable L1Topo hardware vs simulation comparison"};
  SG::ReadHandleKey<xAOD::L1TopoSimResultsContainer> m_l1topoKey {
    this, "L1_TopoKey", "L1_TopoSimResults", "l1topo EDM"};

  // ------------------------- Private methods ---------------------------------

};

#endif // L1TOPOONLINEMONITORING_L1TopoOnlineMonitor_h
