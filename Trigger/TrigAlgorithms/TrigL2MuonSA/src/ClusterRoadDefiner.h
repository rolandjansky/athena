/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef  TRIGL2MUONSA_CLUSTERROADDEFINER_H
#define  TRIGL2MUONSA_CLUSTERROADDEFINER_H

#include <string>
#include <vector>

#include "AthenaBaseComps/AthAlgTool.h"

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "TrigMuonToolInterfaces/ITrigMuonBackExtrapolator.h"

#include "MuonIdHelpers/IMuonIdHelperSvc.h"

#include "ClusterPatFinder.h"
#include "MuonRoad.h"
#include "MdtRegion.h"
#include "RpcFitResult.h"
#include "TrigT1Interfaces/RecMuonRoI.h"
#include "xAODTrigger/MuonRoI.h"

#include "IRegionSelector/IRegSelTool.h"

namespace TrigL2MuonSA {

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

class ClusterRoadDefiner: public AthAlgTool
{
 public:
  ClusterRoadDefiner(const std::string& type,
                     const std::string& name,
                     const IInterface*  parent);

  virtual StatusCode initialize() override;
 public:
  StatusCode defineRoad(const LVL1::RecMuonRoI*                   p_roi,
			std::vector<TrigL2MuonSA::MuonRoad>&      clusterRoad,
                        TrigL2MuonSA::RpcLayerClusters&           rpcLayerClusters,
			ToolHandle<ClusterPatFinder>*             clusterPatFinder,
			std::vector<TrigL2MuonSA::RpcFitResult>&  clusterFitResults,
			double                                    roiEtaMinLow,
			double                                    roiEtaMaxLow,
			double                                    roiEtaMinHigh,
			double                                    roiEtaMaxHigh) const;
  StatusCode defineRoad(const xAOD::MuonRoI*                      p_roi,
			std::vector<TrigL2MuonSA::MuonRoad>&      clusterRoad,
                        TrigL2MuonSA::RpcLayerClusters&           rpcLayerClusters,
			ToolHandle<ClusterPatFinder>*             clusterPatFinder,
			std::vector<TrigL2MuonSA::RpcFitResult>&  clusterFitResults,
			double                                    roiEtaMinLow,
			double                                    roiEtaMaxLow,
			double                                    roiEtaMinHigh,
			double                                    roiEtaMaxHigh) const;

  void setRoadWidthForFailure(double rWidth_RPC_Failed){ m_rWidth_RPC_Failed = rWidth_RPC_Failed; };
  void setRpcGeometry(bool use_rpc){ m_use_rpc = use_rpc; };

 private:
  double m_rWidth_RPC_Failed{0};
  bool m_use_rpc{true};

  ToolHandle<IRegSelTool> m_regionSelector{this, "RegionSelectionTool", "RegSelTool/RegSelTool_MDT", "MDT Region Selector Tool"};
  ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc{this, "MuonIdHelperSvc", "Muon::MuonIdHleperSvc/MuonIdHelperSvc"};
};

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

}
#endif // TRIGL2MUONSA_CLUSTERROADDEFINER_H
