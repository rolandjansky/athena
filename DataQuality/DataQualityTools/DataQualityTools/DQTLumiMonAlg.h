/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @class DQTLumiMonAlg
 * @author C. D. Burton <burton@utexas.edu>
 * @date 2019/08/09
 */

#ifndef DQTLUMIMONALG_H
#define DQTLUMIMONALG_H

#include "AthenaMonitoring/AthMonitorAlgorithm.h"

#include "xAODTracking/VertexContainer.h"
#include "InDetPrepRawData/PixelClusterContainer.h"
#include "InDetIdentifier/PixelID.h"

class DQTLumiMonAlg : public AthMonitorAlgorithm {
public:
    DQTLumiMonAlg(const std::string& name, ISvcLocator* pSvcLocator);
    virtual ~DQTLumiMonAlg();
    virtual StatusCode initialize() override;
    virtual StatusCode fillHistograms(const EventContext& ctx) const override;
private:
    template <class T> using RH = SG::ReadHandle<T>;
    template <class T> using RHK = SG::ReadHandleKey<T>;
    template <class T> using GP = Gaudi::Property<T>;

    RHK<xAOD::VertexContainer> m_VertexContainerKey {this, "VertexContainerKey", "PrimaryVertices", ""};
    RHK<InDet::PixelClusterContainer> m_PixelClustersKey {this, "PixelClustersKey", "PixelClusters", ""};
    RHK<PixelID> m_PixelIDKey {this, "PixelIDKey", "DetectorStore+PixelID", ""};

    GP<float> m_tightTrackWeight {this, "TightTrackWeight", 0.01};
    GP<int> m_tightNTracks {this, "TightNTracks", 4};
};
#endif
