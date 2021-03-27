/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PHYSVALCLUSTER_H
#define PHYSVALCLUSTER_H

#include "ClusterValidationPlots.h"
#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/ReadHandleKey.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODCaloEvent/CaloClusterContainer.h"

class PhysValCluster : public ManagedMonitorToolBase {

 public:

  /** Standard Constructor */
  PhysValCluster (const std::string& type, const std::string& name, const IInterface* parent );

  /** Standard AlgTool Functions */
  virtual StatusCode initialize();
  virtual StatusCode bookHistograms();
  virtual StatusCode fillHistograms();

 private:

  /** String that defines with Cluster container to use */
  SG::ReadHandleKey<xAOD::CaloClusterContainer> m_clusterContainerName{this, "ClusterContainerName", "", "Validated cluster container key"};
  /** Event info key */
  SG::ReadHandleKey<xAOD::EventInfo> m_eventInfoName{this, "EventInfoName", "EventInfo", "Key for event info"};


  /** Pointer to class that defines which histogram blocks to fill */
  std::unique_ptr<ClusterValidationPlots> m_clusterValidationPlots;

};
#endif
