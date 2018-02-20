/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PHYSVALPFO_H
#define PHYSVALPFO_H

#include "PFOChargedValidationPlots.h"
#include "PFONeutralValidationPlots.h"
#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include <string>
#include "PFlowUtils/IRetrievePFOTool.h"
#include "xAODTracking/VertexContainer.h"

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
  SG::ReadHandle<xAOD::VertexContainer> m_vertexContainerReadHandle;
  
  /** Pointer to class that deals with histograms for charged PFO */
  std::unique_ptr<PFOChargedValidationPlots> m_PFOChargedValidationPlots;

  /** Pointer to class that deals with histograms for neutral PFO */
  std::unique_ptr<PFONeutralValidationPlots> m_PFONeutralValidationPlots;
  
  /** Tool to retrieve PFO */
  ToolHandle<CP::IRetrievePFOTool> m_retrievePFOTool;

  /** Select which PFO setup to use - LC or EM */
  bool m_useLCScale;
  
  /** Select whether to use neutral or charged PFO */
  bool m_useNeutralPFO;

};
#endif
