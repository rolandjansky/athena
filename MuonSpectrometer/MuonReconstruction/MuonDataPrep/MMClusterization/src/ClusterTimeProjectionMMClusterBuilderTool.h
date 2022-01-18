/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef ClusterTimeProjectionMMClusterBuilderTool_h
#define ClusterTimeProjectionMMClusterBuilderTool_h

#include <vector>
#include <string>

#include "GaudiKernel/ServiceHandle.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"

#include "MMClusterization/IMMClusterBuilderTool.h"
#include "MuonPrepRawData/MMPrepData.h"
#include "AthenaBaseComps/AthAlgTool.h"

namespace Muon {
class ClusterTimeProjectionMMClusterBuilderTool :
  virtual public IMMClusterBuilderTool,
  public AthAlgTool {
 public:
  ClusterTimeProjectionMMClusterBuilderTool(const std::string&,
      const std::string&, const IInterface*);

  virtual ~ClusterTimeProjectionMMClusterBuilderTool() = default;
  StatusCode initialize() override;

  StatusCode getClusters(std::vector<Muon::MMPrepData>& MMprds,
      std::vector<std::unique_ptr<Muon::MMPrepData>>& clustersVec) const override;

  StatusCode getCalibratedClusterPosition(const Muon::MMPrepData* cluster, std::vector<NSWCalib::CalibratedStrip>&, 
	   const float theta, Amg::Vector2D& clusterLocalPosition, Amg::MatrixX& covMatrix) const;

 private:
    /// Muon Detector Descriptor
    ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};

    bool m_writeStripProperties;
    uint m_maxHoleSize;


    StatusCode sortHitsToLayer(const std::vector<Muon::MMPrepData>& MMprds,
                               std::vector<std::vector<Muon::MMPrepData>>& prdsPerLayer) const;

    StatusCode clusterLayer(const std::vector<Muon::MMPrepData> &MMPrdsPerLayer,
                            std::vector<std::vector<uint>> &idxClusters) const;


    StatusCode getClusterPositionPRD(const std::vector<Identifier>& ids, 
                                const std::vector<float>& stripsPos, const std::vector<float>& driftDists, 
                                const std::vector<Amg::MatrixX> driftDistErrors, const std::vector<int> &charges, const float thetaEstimate, 
								double &clusterPosition,
								double &clusterPositionErrorSq) const;

    StatusCode writeClusterPrd(const std::vector<Muon::MMPrepData> &MMPrdsOfLayer,
                               const std::vector<uint> &idxCluster,
                               const double &clustersPosition,
                               const double &clustersPositionErrorSq,
                               std::vector<std::unique_ptr<Muon::MMPrepData>>& clustersVec) const;

    uint channel(const Identifier &id) const {
        return m_idHelperSvc->mmIdHelper().channel(id);
    }
    uint channel(const MMPrepData &strip) const {
        return channel(strip.identify());
    }
};  // class ClusterTimeProjectionMMClusterBuilderTool


}  // namespace Muon
#endif  // ClusterTimeProjectionMMClusterBuilderTool_h
