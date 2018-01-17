/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PHYSVALPFO_H
#define PHYSVALPFO_H

#include "PFOChargedValidationPlots.h"
#include "PFONeutralValidationPlots.h"
#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include <string>
#include "xAODTracking/VertexContainer.h"
#include "xAODPFlow/PFOContainer.h"
#include "StoreGate/ReadHandleKey.h"

class PhysValPFO : public ManagedMonitorToolBase {

public:

  /** Standard Constructor */
  PhysValPFO (const std::string& type, const std::string& name, const IInterface* parent );

  /** Standard Destructor */
  virtual ~PhysValPFO();
  
  /** Standard AlgTool Functions */
  virtual StatusCode initialize();
  virtual StatusCode bookHistograms();
  virtual StatusCode fillHistograms();
  virtual StatusCode procHistograms();

 private:

  /** ReadHandle to retrieve xAOD::VertexContainer */
  SG::ReadHandleKey<xAOD::VertexContainer> m_vertexContainerReadHandleKey{this,"primaryVerticesName","PrimaryVertices","ReadHandleKey for the PrimaryVertices container"};

  /** ReadHandle to retrieve xAOD::PFOContainer */
  SG::ReadHandleKey<xAOD::PFOContainer> m_PFOContainerHandleKey{this,"PFOContainerName","JetETMissChargedParticleFlowObjects","ReadHandleKey for the PFO container"};
    
  /** Pointer to class that deals with histograms for charged PFO */
  std::unique_ptr<PFOChargedValidationPlots> m_PFOChargedValidationPlots;

  /** Pointer to class that deals with histograms for neutral PFO */
  std::unique_ptr<PFONeutralValidationPlots> m_PFONeutralValidationPlots;
  
  /** Select which PFO setup to use - LC or EM */
  bool m_useLCScale;
  
  /** Select whether to use neutral or charged PFO */
  bool m_useNeutralPFO;

};
#endif
