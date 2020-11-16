/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PHYSVALFE_H
#define PHYSVALFE_H

#include "PFOChargedValidationPlots.h"
#include "PFONeutralValidationPlots.h"
#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include <string>
#include "xAODTracking/VertexContainer.h"
#include "xAODPFlow/PFOContainer.h"
#include "xAODPFlow/FlowElementContainer.h"
#include "StoreGate/ReadHandleKey.h"

class PhysValFE : public ManagedMonitorToolBase {

public:

  /** Standard Constructor */
  PhysValFE (const std::string& type, const std::string& name, const IInterface* parent );

  /** Standard Destructor */
  virtual ~PhysValFE();
  
  /** Standard AlgTool Functions */
  virtual StatusCode initialize();
  virtual StatusCode bookHistograms();
  virtual StatusCode fillHistograms();
  virtual StatusCode procHistograms();

 private:
  
  /** ReadHandle to retrieve xAOD::VertexContainer */
  SG::ReadHandleKey<xAOD::VertexContainer> m_vertexContainerReadHandleKey{this,"primaryVerticesName","PrimaryVertices","ReadHandleKey for the PrimaryVertices container"};

  /** ReadHandle to retrieve xAOD::FlowElementContainer (charged) */
  SG::ReadHandleKey<xAOD::FlowElementContainer> m_FEContainerHandleKey{this,"FlowElementContainerName","JetETMissChargedFlowElements","ReadHandleKey for the FE container"};
   
  
  /** ReadHandleKey to retrieve xAOD::PFOContainer */
  SG::ReadHandleKey<xAOD::PFOContainer> m_PFOContainerHandleKey{this,"PFOContainerName","JetETMissChargedParticleFlowObjects","ReadHandleKey for the PFO container"};

  /** Pointer to class that deals with histograms for charged FE */
  std::unique_ptr<PFOChargedValidationPlots> m_FEChargedValidationPlots;

  /** Pointer to class that deals with histograms for neutral FE */
  std::unique_ptr<PFONeutralValidationPlots> m_FENeutralValidationPlots;
  
  /** Select whether to use neutral or charged FE */
  bool m_useNeutralFE;

};
#endif
