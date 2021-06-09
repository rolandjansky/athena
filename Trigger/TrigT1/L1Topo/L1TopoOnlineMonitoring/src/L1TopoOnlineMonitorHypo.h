/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef L1TOPOONLINEMONITORING_L1TopoOnlineMonitorHypo_h
#define L1TOPOONLINEMONITORING_L1TopoOnlineMonitorHypo_h

#include "L1TopoOnlineMonitorHypoTool.h"
#include "DecisionHandling/HypoBase.h"

/**
 *  @class L1TopoOnlineMonitorHypo
 *  @brief Hypo algorithm for L1Topo online monitoring chains accepting events with
 *         potential hardware, decoding or simulation issues
 **/
class L1TopoOnlineMonitorHypo : public HypoBase {
public:
  L1TopoOnlineMonitorHypo(const std::string& name, ISvcLocator* svcLoc);

  // ------------------------- AthReentrantAlgorithm methods -------------------
  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext& eventContext) const override;

private:
  ToolHandleArray<L1TopoOnlineMonitorHypoTool> m_hypoTools{
    this, "HypoTools", {},
    "Tools to decide on accepting the event"};

};

#endif // L1TOPOONLINEMONITORING_L1TopoOnlineMonitorHypo_h
