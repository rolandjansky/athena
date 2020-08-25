/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#include "ClusterTimeProjectionMMClusterBuilderTool.h"

#include <algorithm>
#include <numeric>

#include "GaudiKernel/PhysicalConstants.h"
#include "GaudiKernel/SystemOfUnits.h"

namespace {
    static constexpr double halfGapWidth = 2.52;
}


Muon::ClusterTimeProjectionMMClusterBuilderTool::ClusterTimeProjectionMMClusterBuilderTool(
                    const std::string& t,
                    const std::string& n, const IInterface* p):
                    AthAlgTool(t, n, p) {
    declareInterface<IMMClusterBuilderTool>(this);
    declareProperty("writeStripProperties", m_writeStripProperties = true ); // true  for debugging; needs to become false for large productions
    declareProperty("maxHoleSize", m_maxHoleSize = 1);
}

StatusCode Muon::ClusterTimeProjectionMMClusterBuilderTool::initialize() {
    ATH_CHECK(m_idHelperSvc.retrieve());
    return StatusCode::SUCCESS;
}

StatusCode Muon::ClusterTimeProjectionMMClusterBuilderTool::getClusters(
                std::vector<Muon::MMPrepData>& MMprds,
                std::vector<std::unique_ptr<Muon::MMPrepData>>& clustersVec) const {
    std::vector<std::vector<Muon::MMPrepData>> prdsPerLayer(8, std::vector<Muon::MMPrepData>(0));
    ATH_CHECK(sortHitsToLayer(MMprds, prdsPerLayer));
    for (const auto& prdsOfLayer : prdsPerLayer) {
        if (prdsOfLayer.size() < 2) continue;  // require at least two strips per layer
        std::vector<std::vector<uint>> idxClusters;  // index of strips in cluster
        StatusCode sc = clusterLayer(prdsOfLayer, idxClusters);
        if (sc.isFailure()) continue;
        for (uint i_cluster = 0; i_cluster < idxClusters.size(); i_cluster++) {
            double clusterPosition, clusterPositionErrorSq;
            sc = getClusterPosition(prdsOfLayer, idxClusters.at(i_cluster),
                                    clusterPosition, clusterPositionErrorSq);
            if (sc.isFailure()) continue;
            sc = writeClusterPrd(prdsOfLayer, idxClusters.at(i_cluster),
                                 clusterPosition, clusterPositionErrorSq, clustersVec);
            if (sc.isFailure()) continue;
        }
    }
    return StatusCode::SUCCESS;
}


StatusCode Muon::ClusterTimeProjectionMMClusterBuilderTool::sortHitsToLayer(
                const std::vector<Muon::MMPrepData>& MMprds,
                std::vector<std::vector<Muon::MMPrepData>>& prdsPerLayer)const {
        // sorting hits by gas gap
    for (const auto& prd : MMprds) {
        Identifier id = prd.identify();
        int layer = 4*(m_idHelperSvc->mmIdHelper().multilayer(id)-1) +
                        (m_idHelperSvc->mmIdHelper().gasGap(id)-1);
        prdsPerLayer.at(layer).push_back(prd);
    }

    ATH_MSG_DEBUG("sorted hist");
    // sort MMPrds by channel
    for (unsigned int i_layer = 0; i_layer < prdsPerLayer.size(); i_layer++) {
               std::sort(prdsPerLayer.at(i_layer).begin(), prdsPerLayer.at(i_layer).end(),
                        [this](const MMPrepData &a, const MMPrepData &b){
                            return this-> channel(a) < this->channel(b);});
    }
    return StatusCode::SUCCESS;
}

StatusCode Muon::ClusterTimeProjectionMMClusterBuilderTool::clusterLayer(
                const std::vector<Muon::MMPrepData> &MMPrdsPerLayer,
                std::vector<std::vector<uint>> &idxClusters) const {
    // get gas gap for later correction of the sign of the slope
    int gasGap = m_idHelperSvc->mmIdHelper().gasGap(MMPrdsPerLayer.at(0).identify());

    ATH_MSG_DEBUG("Scanning gas gap "<< gasGap);
    for (const auto &prd : MMPrdsPerLayer) {
        ATH_MSG_DEBUG("Hit channel "<< channel(prd)
                       << " local positionX " << prd.localPosition().x()
                       << " time " << prd.time()
                       << " corrected time " << prd.time()
                       << " angle to IP" << std::atan(prd.globalPosition().perp() /
                                std::abs(prd.globalPosition().z()))
                       << " "<< std::atan(prd.globalPosition().perp() /
                                std::abs(prd.globalPosition().z())) / Gaudi::Units::degree
                       << " stationEta " <<m_idHelperSvc->mmIdHelper().stationEta(prd.identify())
                       << " stationPhi "<< m_idHelperSvc->mmIdHelper().stationEta(prd.identify())
                       << " multilayer " <<m_idHelperSvc->mmIdHelper().multilayer(prd.identify()));
    }

    // simple grouping of strips using the fact that the strips where ordered by channel
    idxClusters.push_back(std::vector<uint>{0});  // assumes that mmPrdsPerLayer always has at least one strip
    for (uint i_strip = 1; i_strip < MMPrdsPerLayer.size(); i_strip++) {
        if (channel(MMPrdsPerLayer.at(i_strip)) -
                channel(MMPrdsPerLayer.at(i_strip-1)) <= m_maxHoleSize+1) {
            idxClusters.back().push_back(i_strip);
        } else {
            idxClusters.push_back(std::vector<uint>{i_strip});
        }
    }
    ATH_MSG_DEBUG("Found "<< idxClusters.size() <<" clusters");
    for (const auto& idxCluster : idxClusters) {
        ATH_MSG_DEBUG("cluster: "<< idxCluster);
    }
    return StatusCode::SUCCESS;
}  // end of cluster layer

StatusCode Muon::ClusterTimeProjectionMMClusterBuilderTool::getClusterPosition(
                const std::vector<Muon::MMPrepData> &MMPrdsOfLayer,
                const std::vector<uint> &idxCluster, double &clusterPosition,
                double &clusterPositionErrorSq) const {
    if (idxCluster.empty()) return StatusCode::FAILURE;
    double qtot = 0;
    double meanTheta = 0;
    double meanDriftDist = 0;
    double meanDriftDistError = 0;
    double meanPosX = 0;
    double meanPosXError = 0;
    for (auto idx : idxCluster) {
        double driftDist = MMPrdsOfLayer.at(idx).driftDist();
        double charge = MMPrdsOfLayer.at(idx).charge()*Gaudi::Units::perThousand; //  divide by 1000 to avoid overflow of variables
        qtot += charge;
        meanPosX += MMPrdsOfLayer.at(idx).localPosition().x()*charge;
        meanPosXError += MMPrdsOfLayer.at(idx).localCovariance()(1, 1)*charge*charge;
        meanDriftDist += driftDist*charge;
        meanDriftDistError += MMPrdsOfLayer.at(idx).localCovariance()(0, 0)*charge*charge;
        ATH_MSG_VERBOSE("Strip:"
                        <<" drift dist " << driftDist
                        <<" +- "<<  std::sqrt(MMPrdsOfLayer.at(idx).localCovariance()(0,0))
                        <<" xpos " << MMPrdsOfLayer.at(idx).localPosition().x()
                        << " +- "<< std::sqrt(MMPrdsOfLayer.at(idx).localCovariance()(1,1))
                        <<" xMeanPos " << meanPosX / qtot
                        <<" +- " << std::sqrt(meanPosXError) / qtot
                        << " meanPosXError " << meanPosXError
                        <<" meanDriftDist " << meanDriftDist/qtot
                        <<" meanDriftDist Error " <<  std::sqrt(meanDriftDistError)/qtot
                        <<" charge " << charge << " qtot " << qtot
                        << " theta " << std::atan(MMPrdsOfLayer.at(idx).globalPosition().perp() /
                        std::abs(MMPrdsOfLayer.at(idx).globalPosition().z()))
                        << " meanTheta " << meanTheta / qtot);

        meanTheta += std::abs(std::atan(MMPrdsOfLayer.at(idx).globalPosition().perp() /
                        std::abs(MMPrdsOfLayer.at(idx).globalPosition().z())))*charge;
    }
    meanPosX /= qtot;
    double meanPosXErrorSq = meanPosXError/(qtot*qtot);
    ATH_MSG_VERBOSE("meanPosXErrorSq" << meanPosXErrorSq
                    << "meanPosXError" << meanPosXError
                    << "qtot*qtot" << qtot*qtot);
    meanDriftDist /= qtot;
    double meanDriftDistErrorSq = meanDriftDistError/(qtot*qtot);
    meanTheta /= qtot;
    ATH_MSG_VERBOSE("Cluster: "<< "xmean" << meanPosX);
    ATH_MSG_VERBOSE("Cluster: " << "meanPosXErrorSq" << meanPosXErrorSq
                    << " sqrt "<< sqrt(meanPosXErrorSq));
    ATH_MSG_VERBOSE("Cluster: "<< " meanDriftDist" << meanDriftDist);
    ATH_MSG_VERBOSE("Cluster: "<< " meanDriftDistErrorSq " << meanDriftDistErrorSq
                               << " sqrt " << std::sqrt(meanDriftDistErrorSq));
    ATH_MSG_VERBOSE("Cluster " << " meanTheta " << meanTheta);

    double correction = std::tan(meanTheta)*(meanDriftDist-halfGapWidth);
    if (m_idHelperSvc->mmIdHelper().gasGap(MMPrdsOfLayer.at(0).identify())%2 == 0) {
     correction = -1. * correction;  // take care of inverted drif direction for even gaps
    }
    double correctionErrorSq = std::tan(meanTheta)*std::tan(meanTheta)*meanDriftDistErrorSq;


    clusterPosition = meanPosX + correction;
    clusterPositionErrorSq = correctionErrorSq + meanPosXErrorSq;

    ATH_MSG_DEBUG("Cluster Properties"
                  << " meanX " << meanPosX
                  << " +-" << std::sqrt(meanPosXErrorSq)
                  << "  mean drift dist " << meanDriftDist
                  <<" +- " << std::sqrt(meanDriftDistErrorSq)
                  << " correction " << correction
                  << " +- " << std::sqrt(correctionErrorSq)
                  << " final position " << clusterPosition
                  << " +- " << std::sqrt(clusterPositionErrorSq));
    return StatusCode::SUCCESS;
}


StatusCode Muon::ClusterTimeProjectionMMClusterBuilderTool::writeClusterPrd(
                           const std::vector<Muon::MMPrepData> &MMPrdsOfLayer,
                           const std::vector<uint> &idxCluster,
                           const double &clusterPosition,
                           const double &clusterPositionErrorSq,
                           std::vector<std::unique_ptr<Muon::MMPrepData>>& clustersVec) const {
    std::vector<Identifier> rdoList;
    std::vector<int> stripCharges;
    std::vector<short int> stripTimes;
    std::vector<uint16_t> stripNumbers;
    std::vector<float> stripDriftDists;
    std::vector<Amg::MatrixX> stripDriftDistErrors;

    rdoList.reserve(idxCluster.size());
    if(m_writeStripProperties) {
        stripCharges.reserve(idxCluster.size());
        stripTimes.reserve(idxCluster.size());
        stripNumbers.reserve(idxCluster.size());
    }
    stripDriftDists.reserve(idxCluster.size());
    stripDriftDistErrors.reserve(idxCluster.size());

    for (auto &idx : idxCluster) {
        Identifier id = MMPrdsOfLayer.at(idx).identify();
        rdoList.push_back(id);
        if(m_writeStripProperties) {
            stripCharges.push_back(MMPrdsOfLayer.at(idx).charge());
            stripTimes.push_back(MMPrdsOfLayer.at(idx).time());
            stripNumbers.push_back(channel(id));
        }
        stripDriftDists.push_back(MMPrdsOfLayer.at(idx).driftDist());
        stripDriftDistErrors.push_back(MMPrdsOfLayer.at(idx).localCovariance());
    }

    Amg::MatrixX* covN = new Amg::MatrixX(1, 1);
    covN -> coeffRef(0, 0) = clusterPositionErrorSq;
    Amg::Vector2D localClusterPositionV(clusterPosition,
            MMPrdsOfLayer.at(idxCluster.at(0)).localPosition().y());
    Identifier idStrip0 = MMPrdsOfLayer.at(idxCluster.at(0)).identify();

    std::unique_ptr<MMPrepData> prdN = std::make_unique<MMPrepData>(idStrip0,
                   MMPrdsOfLayer.at(idxCluster.at(0)).collectionHash(),
                   localClusterPositionV, rdoList, covN,
                   MMPrdsOfLayer.at(idxCluster.at(0)).detectorElement(),
                   (short int) 0,  // drift dist
                   std::accumulate(stripCharges.begin(), stripCharges.end(), 0),
                   0.0/*drift dist*/,
                   stripNumbers, stripTimes, stripCharges);

    prdN->setDriftDist(stripDriftDists, stripDriftDistErrors);
    prdN->setAuthor(Muon::MMPrepData::Author::ClusterTimeProjectionClusterBuilder);

    clustersVec.push_back(std::move(prdN));
    return StatusCode::SUCCESS;
}
