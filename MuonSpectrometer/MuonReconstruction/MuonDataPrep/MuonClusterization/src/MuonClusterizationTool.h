/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCLUSTERIZATIONTOOL_H
#define MUONCLUSTERIZATIONTOOL_H


#include "MuonClusterization/IMuonClusterizationTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"

#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MuonPrepRawData/MuonPrepDataContainer.h"
#include "MuonClusterization/HitClustering.h"
#include "MuonClusterization/RpcHitClustering.h"

#include <string>

namespace Muon{ 

  class MuonClusterizationTool : virtual public IMuonClusterizationTool, public AthAlgTool
  {
  public:
    MuonClusterizationTool(const std::string& t, const std::string& n, const IInterface* p);
    virtual ~MuonClusterizationTool()=default;

    virtual StatusCode initialize();
    virtual StatusCode finalize();

    Muon::TgcPrepDataContainer*  cluster( const Muon::TgcPrepDataContainer& prdContainer ) const ;
    Muon::TgcPrepDataCollection* cluster( const Muon::TgcPrepDataCollection& col ) const ;

    Muon::RpcPrepDataContainer*  cluster( const Muon::RpcPrepDataContainer& prdContainer ) const;
    Muon::RpcPrepDataCollection* cluster( const Muon::RpcPrepDataCollection& col ) const;

  private:
    void addClusters( const std::vector<Muon::ClusterObj>& clusters, Muon::TgcPrepDataCollection* collection ) const ;
    void addClusters( const std::vector<Muon::ClusterObj>& clusters, Muon::RpcPrepDataCollection* collection ) const;

    ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};

    Muon::HitClusteringObj* m_tgcClustering;
    Muon::HitClusteringObj* m_rpcClustering;
    bool m_combineGasGaps;
  };
}

#endif 
