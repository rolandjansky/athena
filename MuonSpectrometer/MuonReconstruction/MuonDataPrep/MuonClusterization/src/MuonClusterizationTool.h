/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCLUSTERIZATIONTOOL_H
#define MUONCLUSTERIZATIONTOOL_H

#include <string>

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuonPrepRawData/MuonPrepDataContainer.h"
#include "MuonClusterization/HitClustering.h"
#include "MuonClusterization/RpcHitClustering.h"
#include "MuonClusterization/IMuonClusterizationTool.h"

namespace Muon {
  class MuonIdHelperTool;
}

namespace Muon{ 

  class MuonClusterizationTool : virtual public IMuonClusterizationTool, public AthAlgTool
  {
  public:
    MuonClusterizationTool(const std::string& t,const std::string& n, const IInterface*  p );
    virtual ~MuonClusterizationTool();

    virtual StatusCode initialize();
    virtual StatusCode finalize();

    Muon::TgcPrepDataContainer*  cluster( const Muon::TgcPrepDataContainer& prdContainer );
    Muon::TgcPrepDataCollection* cluster( const Muon::TgcPrepDataCollection& col );

    Muon::RpcPrepDataContainer*  cluster( const Muon::RpcPrepDataContainer& prdContainer );
    Muon::RpcPrepDataCollection* cluster( const Muon::RpcPrepDataCollection& col );

  private:
    void addClusters( const std::vector<Muon::ClusterObj>& clusters, Muon::TgcPrepDataCollection* collection );
    void addClusters( const std::vector<Muon::ClusterObj>& clusters, Muon::RpcPrepDataCollection* collection );

    ToolHandle<Muon::MuonIdHelperTool> m_idHelper;    //!< id helper Tool 

    Muon::HitClusteringObj* m_tgcClustering;
    Muon::HitClusteringObj* m_rpcClustering;
    bool m_combineGasGaps;
  };
}

#endif 
