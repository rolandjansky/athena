/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef AFP_SICLUSTERTOOLS_AFPSICLUSTER_H
#define AFP_SICLUSTERTOOLS_AFPSICLUSTER_H

// Framework includes
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthenaKernel/errorcheck.h" // CHECK( )
#include "AthenaBaseComps/AthMsgStreamMacros.h" 

// STL includes
#include <string>

#include "AFP_SiClusterTools/IAFPSiClusterTool.h"
#include "xAODForward/AFPSiHitsClusterContainer.h"
#include "xAODForward/AFPSiHitsClusterAuxContainer.h"


class AFPSiCluster : public AthReentrantAlgorithm {
public:
  AFPSiCluster(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~AFPSiCluster() override;

  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext& ctx) const override;
  virtual StatusCode finalize() override;

private:
  SG::WriteHandleKey<xAOD::AFPSiHitsClusterContainer> m_clusterContainerKey{this, "AFPSiHitsClusterContainerKey", "AFPSiHitsClusterContainer", "name of AFPSiHitsCluster container"};
  
  // Tool that does the clustering
  ToolHandle<IAFPSiClusterTool> m_clusterRecoTool{this, "clusterRecoTool", "", "AFP cluster reco tool"};
};

#endif // AFP_SICLUSTERTOOLS_AFPSICLUSTER_H
