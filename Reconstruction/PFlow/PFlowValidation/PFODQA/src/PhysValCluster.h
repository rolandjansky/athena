/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PHYSVALCLUSTER_H
#define PHYSVALCLUSTER_H

#include "ClusterValidationPlots.h"
#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include <string>

class PhysValCluster : public ManagedMonitorToolBase {

 public:

  /** Standard Constructor */
  PhysValCluster (const std::string& type, const std::string& name, const IInterface* parent );

  /** Standard Destructor */
  virtual ~PhysValCluster();
  
  /** Standard AlgTool Functions */
  virtual StatusCode initialize();
  virtual StatusCode bookHistograms();
  virtual StatusCode fillHistograms();
  virtual StatusCode procHistograms();

 private:

  /** String that defines with Cluster container to use */
  std::string m_clusterContainerName;

  /** Pointer to class that defines which histogram blocks to fill */
  std::unique_ptr<ClusterValidationPlots> m_clusterValidationPlots;

};
#endif
