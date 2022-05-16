/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#include "ClusterTimeProjectionMMClusterBuilderTool.h"

#include <algorithm>
#include <numeric>

#include "GaudiKernel/PhysicalConstants.h"
#include "GaudiKernel/SystemOfUnits.h"

namespace {
    constexpr double halfGapWidth = 2.52;
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
            std::vector<Identifier> rdoList;
            std::vector<int> stripCharges;
            std::vector<float> stripPos;
            std::vector<float> stripDriftDists;
            std::vector<Amg::MatrixX> stripDriftDistErrors;

            rdoList.reserve(idxClusters.at(i_cluster).size());
            stripCharges.reserve(idxClusters.at(i_cluster).size());
            stripPos.reserve(idxClusters.at(i_cluster).size());
            
            stripDriftDists.reserve(idxClusters.at(i_cluster).size());
            stripDriftDistErrors.reserve(idxClusters.at(i_cluster).size());

            float thetaEstimate = 0;
        
            for(auto idx:idxClusters.at(i_cluster)){
                Identifier id = prdsOfLayer.at(idx).identify();
                rdoList.push_back(id);
                stripCharges.push_back(prdsOfLayer.at(idx).charge());
                stripPos.push_back(prdsOfLayer.at(idx).localPosition().x());              
                thetaEstimate += std::atan(prdsOfLayer.at(idx).globalPosition().perp() / std::abs(prdsOfLayer.at(idx).globalPosition().z())) * prdsOfLayer.at(idx).charge();
                stripDriftDists.push_back(prdsOfLayer.at(idx).driftDist());
                stripDriftDistErrors.push_back(prdsOfLayer.at(idx).localCovariance());                     
            }
            thetaEstimate /= std::accumulate(stripCharges.begin(),stripCharges.end(),0.0);

            sc = getClusterPositionPRD(rdoList, stripPos, stripDriftDists, stripDriftDistErrors, stripCharges, thetaEstimate , clusterPosition, clusterPositionErrorSq);

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

StatusCode Muon::ClusterTimeProjectionMMClusterBuilderTool::getClusterPositionPRD(const std::vector<Identifier>& ids,
                                          const std::vector<float>& stripsPos, const std::vector<float>& driftDists, 
                                          const std::vector<Amg::MatrixX> driftDistErrors, const std::vector<int> &charges, const float thetaEstimate, 
										  double &clusterPosition,
										  double &clusterPositionErrorSq) const {
    if (ids.empty()) return StatusCode::FAILURE;
    double qtot = 0;
    double meanDriftDist = 0;
    double meanDriftDistError = 0;
    double meanPosX = 0;
    double meanPosXError = 0;
    for (uint i_strip=0;i_strip<ids.size(); i_strip++) {
        double driftDist = driftDists.at(i_strip);
        double charge = charges.at(i_strip)*Gaudi::Units::perThousand; //  divide by 1000 to avoid overflow of variables
        qtot += charge;
        meanPosX += stripsPos.at(i_strip)*charge;
        meanPosXError += driftDistErrors.at(i_strip)(1, 1)*charge*charge;
        meanDriftDist += driftDist*charge;
        meanDriftDistError += driftDistErrors.at(i_strip)(0, 0)*charge*charge;
        ATH_MSG_VERBOSE("Strip:"
                        <<" drift dist " << driftDist
                        <<" +- "<<  std::sqrt(driftDistErrors.at(i_strip)(0,0))
                        <<" xpos " << stripsPos.at(i_strip)
                        << " +- "<< std::sqrt(driftDistErrors.at(i_strip)(1,1))
                        <<" xMeanPos " << meanPosX / qtot
                        <<" +- " << std::sqrt(meanPosXError) / qtot
                        << " meanPosXError " << meanPosXError
                        <<" meanDriftDist " << meanDriftDist/qtot
                        <<" meanDriftDist Error " <<  std::sqrt(meanDriftDistError)/qtot
                        <<" charge " << charge << " qtot " << qtot
                        );
    }
    meanPosX /= qtot;
    double meanPosXErrorSq = meanPosXError/(qtot*qtot);
    ATH_MSG_VERBOSE("meanPosXErrorSq" << meanPosXErrorSq
                    << "meanPosXError" << meanPosXError
                    << "qtot*qtot" << qtot*qtot);
    meanDriftDist /= qtot;
    double meanDriftDistErrorSq = meanDriftDistError/(qtot*qtot);
    ATH_MSG_VERBOSE("Cluster: "<< "xmean" << meanPosX);
    ATH_MSG_VERBOSE("Cluster: " << "meanPosXErrorSq" << meanPosXErrorSq
                    << " sqrt "<< sqrt(meanPosXErrorSq));
    ATH_MSG_VERBOSE("Cluster: "<< " meanDriftDist" << meanDriftDist);
    ATH_MSG_VERBOSE("Cluster: "<< " meanDriftDistErrorSq " << meanDriftDistErrorSq
                               << " sqrt " << std::sqrt(meanDriftDistErrorSq));
    ATH_MSG_VERBOSE("Cluster " << " thetaEstimate " << thetaEstimate);

    double correction = std::tan(thetaEstimate)*(meanDriftDist-halfGapWidth);
    if (m_idHelperSvc->mmIdHelper().gasGap(ids.at(0))%2 == 0) {
     correction = -1. * correction;  // take care of inverted drif direction for even gaps
    }
    double correctionErrorSq = std::tan(thetaEstimate)*std::tan(thetaEstimate)*meanDriftDistErrorSq;


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
    stripCharges.reserve(idxCluster.size());
    stripTimes.reserve(idxCluster.size());
    if(m_writeStripProperties) {
        stripNumbers.reserve(idxCluster.size());
    }
    stripDriftDists.reserve(idxCluster.size());
    stripDriftDistErrors.reserve(idxCluster.size());

    for (const auto &idx : idxCluster) {
        Identifier id = MMPrdsOfLayer.at(idx).identify();
        rdoList.push_back(id);
        if(m_writeStripProperties) {
            stripNumbers.push_back(channel(id));
        }
        stripTimes.push_back(MMPrdsOfLayer.at(idx).time());
        stripCharges.push_back(MMPrdsOfLayer.at(idx).charge());
        stripDriftDists.push_back(MMPrdsOfLayer.at(idx).driftDist());
        stripDriftDistErrors.push_back(MMPrdsOfLayer.at(idx).localCovariance());
    }

    auto covN =Amg::MatrixX(1, 1);
    covN.coeffRef(0, 0) = clusterPositionErrorSq;
    Amg::Vector2D localClusterPositionV(clusterPosition,
            MMPrdsOfLayer.at(idxCluster.at(0)).localPosition().y());
    Identifier idStrip0 = MMPrdsOfLayer.at(idxCluster.at(0)).identify();

    std::unique_ptr<MMPrepData> prdN = std::make_unique<MMPrepData>(idStrip0,
                   MMPrdsOfLayer.at(idxCluster.at(0)).collectionHash(),
                   localClusterPositionV, rdoList, std::move(covN),
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

StatusCode Muon::ClusterTimeProjectionMMClusterBuilderTool::getCalibratedClusterPosition (const Muon::MMPrepData* cluster, std::vector<NSWCalib::CalibratedStrip>& strips,
        const float theta, Amg::Vector2D& clusterLocalPosition, Amg::MatrixX& covMatrix) const {
    
    std::vector<Identifier> ids;
    std::vector<float> stripsPos;
    std::vector<float> driftDists;
    std::vector<Amg::MatrixX> driftDistErrors;

    for (const NSWCalib::CalibratedStrip &strip : strips)  {
            ids.push_back(strip.identifier);
            stripsPos.push_back(strip.locPos.x());
            driftDists.push_back(strip.distDrift);

            Amg::MatrixX cov(2,2);
            cov.setIdentity();
            cov(0,0) = strip.resTransDistDrift;  
            cov(1,1) = strip.resLongDistDrift; 
            driftDistErrors.push_back(cov);    
    }

    double localClusterPosition=-9999;
    double sigmaLocalClusterPosition=0;

    StatusCode sc = getClusterPositionPRD(ids, stripsPos, driftDists, driftDistErrors, cluster->stripCharges() , theta, localClusterPosition, sigmaLocalClusterPosition);

    if(sc==StatusCode::FAILURE) return sc;

    clusterLocalPosition[Trk::locX] =  localClusterPosition;
    
    Amg::MatrixX covN (1,1);
    covN.coeffRef(0,0)=sigmaLocalClusterPosition;
    covMatrix=covN;

    return StatusCode::SUCCESS;


    }