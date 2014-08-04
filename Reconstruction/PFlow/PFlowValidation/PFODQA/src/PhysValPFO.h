/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PHYSVALPFO_H
#define PHYSVALPFO_H

#include "PFOValidationPlots.h"
#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include <string>
#include "PFlowUtils/IRetrievePFOTool.h"

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

  /** String that defines with PFO container to use */
  std::string m_PFOContainerName;

  /** Pointer to class that defines which histogram blocks to fill */
  std::unique_ptr<PFOValidationPlots> m_PFOValidationPlots;

  /** Tool to retrieve PFO */
  ToolHandle<CP::IRetrievePFOTool> m_retrievePFOTool;

  /** Select which PFO setup to use - LC or EM */
  bool m_useLCScale;
  
  /** Select whether to use neutral or charged PFO */
  bool m_useNeutralPFO;

  /** Select whether to fill EM scale histograms up */
  bool m_fillEMHistograms;

};
#endif
