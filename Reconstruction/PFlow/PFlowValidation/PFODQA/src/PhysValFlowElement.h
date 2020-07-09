/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PHYSVALFLOWELEMENT_H
#define PHYSVALFLOWELEMENT_H

#include "PFChargedFlowElementValidationPlots.h"
#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include <string>
#include "xAODPFlow/FlowElementContainer.h"
#include "StoreGate/ReadHandleKey.h"

class PhysValFlowElement : public ManagedMonitorToolBase {

public:

  /** Standard Constructor */
  PhysValFlowElement (const std::string& type, const std::string& name, const IInterface* parent );

  /** Standard Destructor */
  virtual ~PhysValFlowElement();
  
  /** Standard AlgTool Functions */
  virtual StatusCode initialize();
  virtual StatusCode bookHistograms();
  virtual StatusCode fillHistograms();
  virtual StatusCode procHistograms();

 private:

  /** ReadHandle to retrieve xAOD::PFOContainer */
  SG::ReadHandleKey<xAOD::FlowElementContainer> m_PFFlowElementContainerHandleKey{this,"FlowElementContainerName","JetETMissChargedFlowElements","ReadHandleKey for the FlowElement container"};
    
  /** Pointer to class that deals with histograms for charged FlowElements */
  std::unique_ptr<PFChargedFlowElementValidationPlots> m_PFChargedFlowElementValidationPlots;

};
#endif
