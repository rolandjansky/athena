/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
#include "SimpleMMClusterBuilderTool.h"

#include "MuonPrepRawData/MMPrepData.h"

using namespace Muon;

Muon::SimpleMMClusterBuilderTool::SimpleMMClusterBuilderTool(const std::string& t, const std::string& n, const IInterface* p) :
    AthAlgTool(t, n, p) {
    declareInterface<IMMClusterBuilderTool>(this);
}

StatusCode Muon::SimpleMMClusterBuilderTool::initialize() {
    ATH_CHECK(m_idHelperSvc.retrieve());
    return StatusCode::SUCCESS;
}

StatusCode Muon::SimpleMMClusterBuilderTool::getClusters(std::vector<Muon::MMPrepData>& MMprds,
                                                         std::vector<std::unique_ptr<Muon::MMPrepData>>& clustersVect) const

{
    ATH_MSG_DEBUG("Size of the input vector: " << MMprds.size());
    ATH_MSG_DEBUG("Size of the output vector: " << clustersVect.size());
  
    if (MMprds.empty()) {
        ATH_MSG_DEBUG("Empty PRD collection: no clusterization");
        return StatusCode::SUCCESS;
    }

    std::sort(MMprds.begin(), MMprds.end(), 
      [&](const Muon::MMPrepData& a, const Muon::MMPrepData& b){
        const Identifier ida = a.identify(), idb = b.identify();
        const int mla = m_idHelperSvc->mmIdHelper().multilayer(ida);
        const int mlb = m_idHelperSvc->mmIdHelper().multilayer(idb);
        
        const int gga = m_idHelperSvc->mmIdHelper().gasGap(ida);
        const int ggb = m_idHelperSvc->mmIdHelper().gasGap(idb);
        
        const int cha = m_idHelperSvc->mmIdHelper().channel(ida);
        const int chb = m_idHelperSvc->mmIdHelper().channel(idb);

        if(mla!=mlb) return mla<mlb;
        if(gga!=ggb) return gga<ggb;
        return cha<chb;
      }
    );

    
    const IdentifierHash hash = MMprds.at(0).collectionHash();
 
    const unsigned int n_input_prds =  MMprds.size();
    std::vector<int> MMflag(n_input_prds, 0);
    
    for (unsigned int i = 0; i < n_input_prds; ++i) {
        // skip the merged prds
        if (MMflag[i] == 1) continue;

        unsigned int jmerge = n_input_prds;
        const Muon::MMPrepData& primary_prd = MMprds[i];
        const Identifier id_prd = primary_prd.identify();
        const uint16_t strip = m_idHelperSvc->mmIdHelper().channel(id_prd);
        const unsigned int gasGap = m_idHelperSvc->mmIdHelper().gasGap(id_prd);
        const unsigned int layer = m_idHelperSvc->mmIdHelper().multilayer(id_prd);
        
        ATH_MSG_VERBOSE("  MMprds " << n_input_prds << " index " << i << " strip " << strip << " gasGap " << gasGap << " layer " << layer
                                    << " z " << primary_prd.globalPosition().z());
        /// Find a strip in the same gas gap that's close around our seed
        for (unsigned int j = i + 1; j < n_input_prds; ++j) {
            const Identifier id_prdN = MMprds[j].identify();
            const uint16_t stripN = m_idHelperSvc->mmIdHelper().channel(id_prdN);
            const unsigned int gasGapN = m_idHelperSvc->mmIdHelper().gasGap(id_prdN);
            const unsigned int layerN = m_idHelperSvc->mmIdHelper().multilayer(id_prdN);
            if (gasGapN == gasGap && layerN == layer) {
                ATH_MSG_VERBOSE(" next MMprds strip same gasGap and layer index " << j << " strip " << stripN << " gasGap " << gasGapN
                                                                                  << " layer " << layerN);
                if (static_cast<unsigned int>(std::abs(strip - stripN)) <= m_maxHoleSize + 1) {
                    jmerge = j;
                    break;
                }
            }
        }

        unsigned int nmerge = 0;
        std::vector<Identifier> rdoList{id_prd};
        std::vector<unsigned int> mergeIndices{i};
        std::vector<uint16_t> mergeStrips{strip};
        std::vector<short int> mergeStripsTime{primary_prd.time()};
        std::vector<int> mergeStripsCharge{primary_prd.charge()};
        std::vector<float> mergeStripsDriftDists{primary_prd.driftDist()};
        std::vector<Amg::MatrixX> mergeStripsDriftDistErrors{primary_prd.localCovariance()};

        // Mark the hit as used
        MMflag[i] = 1;
       

        constexpr size_t nmergeStripsMax = 50;
        for (unsigned int k = 0; k < std::min(nmergeStripsMax, mergeIndices.size()); ++k) {            
            for (unsigned int j = jmerge; j < n_input_prds; ++j) {
                /// Skip used channels
                if (MMflag[j] == 1) continue;

                const Muon::MMPrepData& to_merge  = MMprds[j];
                const Identifier id_prdN = to_merge.identify();

                const uint16_t stripN = m_idHelperSvc->mmIdHelper().channel(id_prdN);
                // Take the last merged strip as a reference to evaluate whether 
                /// it's in the window. Btw does this allow for a super cluster?
                if (static_cast<unsigned int>(std::abs(mergeStrips[k] - stripN)) > m_maxHoleSize + 1) {
                    continue;
                }
                const unsigned int gasGapN = m_idHelperSvc->mmIdHelper().gasGap(id_prdN);
                const unsigned int layerN = m_idHelperSvc->mmIdHelper().multilayer(id_prdN);
                /// Wrong gasgap
                if (gasGapN != gasGap || layerN != layer) { continue; }
                /// Strip already merged
                if (mergeStrips[k] == stripN) {
                       MMflag[j] = 1;
                       continue;
                }
                ++nmerge;
                MMflag[j] = 1;
              
                rdoList.push_back(id_prdN);
                mergeIndices.push_back(j);
                mergeStrips.push_back(stripN);
                mergeStripsTime.push_back(to_merge.time());
                mergeStripsCharge.push_back(to_merge.charge());

                mergeStripsDriftDists.push_back(to_merge.driftDist());
                mergeStripsDriftDistErrors.push_back(to_merge.localCovariance());               
               /// Ideally we should update jmerge here but that strongly depends on the ordering of the input
            }
            if (k >= nmergeStripsMax) break;
        }
        ATH_MSG_VERBOSE(" add merged MMprds nmerge " << nmerge << " strip " << strip << " gasGap " << gasGap << " layer " << layer);

       

        // start off from strip in the middle
        unsigned int stripSum = 0;
        for (unsigned short strip : mergeStrips) stripSum += strip;
        stripSum /= mergeStrips.size();        
      
        unsigned int j = jmerge;
        for (unsigned int k = 0; k < mergeStrips.size(); ++k) {
            if (mergeStrips[k] != stripSum) continue;
            j = mergeIndices[k];
            ATH_MSG_VERBOSE(" merged strip nr " << k << " strip " << mergeStrips[k] << " index " << mergeIndices[k]);
            break;
        }
        const Muon::MMPrepData& central_prd = j < n_input_prds ? MMprds.at(j) : primary_prd;
        ATH_MSG_VERBOSE(" Look for strip nr " << stripSum << " found at index " << j);

        ///
        /// get the local position from the cluster centroid
        ///       
       
        std::vector<Muon::MMPrepData> stripsVec;
        Amg::Vector2D clusterLocalPosition{0., 0.};
        
        double totalCharge = 0.0;       
        for (const unsigned int merge_idx : mergeIndices) {
            const Muon::MMPrepData& merge_it = MMprds.at(merge_idx);
            stripsVec.emplace_back(merge_it);
            totalCharge += merge_it.charge();
        }
        ///
        /// memory allocated dynamically for the PrepRawData is managed by Event Store
        ///
         Amg::MatrixX covMatrix(1,1);
        getClusterPosition(stripsVec, clusterLocalPosition, covMatrix);

        ///
        /// memory allocated dynamically for the PrepRawData is managed by Event Store
        ///
        if (!m_writeStripProperties) mergeStrips.clear();
        std::unique_ptr<Muon::MMPrepData> prdN = std::make_unique<MMPrepData>(
            central_prd.identify(), hash, clusterLocalPosition, rdoList, std::move(covMatrix), central_prd.detectorElement(),
            0, totalCharge, 0.f, mergeStrips, mergeStripsTime, mergeStripsCharge);
        prdN->setDriftDist(mergeStripsDriftDists, mergeStripsDriftDistErrors);
        prdN->setAuthor(Muon::MMPrepData::Author::SimpleClusterBuilder);

        clustersVect.push_back(std::move(prdN));
        

    }  // end loop MMprds[i]
    // clear vector and delete elements
    MMprds.clear();

    return StatusCode::SUCCESS;
}

/// get cluster local position and covariance matrix
void SimpleMMClusterBuilderTool::getClusterPosition(std::vector<Muon::MMPrepData>& stripsVec, Amg::Vector2D& clusterLocalPosition,
                                                          Amg::MatrixX& covMatrix) const {
    covMatrix.setIdentity();
    if (stripsVec.empty()) return;
    double weightedPosX{0.}, posY{0.}, totalCharge{0.}, theta{0.};

    /// get the Y local position from the first strip ( it's the same for all strips in the cluster)
    posY = stripsVec[0].localPosition().y();
    for ( const Muon::MMPrepData& strip : stripsVec) {
        const double posX = strip.localPosition().x();
        const double charge = strip.charge();
        weightedPosX += posX * charge;
        totalCharge += charge;
        const Amg::Vector3D global_pos = strip.globalPosition();
        /// Note that the uncertainty terms are derived for the positive side. 
        /// For the C-side we need to mirror the vector and recalculte theta
        //theta += std::atan2(global_pos.perp() ,std::abs(global_pos.z())) * charge;
        theta += std::atan(global_pos.perp() / std::abs(global_pos.z())) * charge;
        
        ATH_MSG_VERBOSE("Adding a strip to the centroid calculation: charge=" << charge);
    }
    weightedPosX = weightedPosX / totalCharge;
    theta /= totalCharge;

    clusterLocalPosition = Amg::Vector2D(weightedPosX, posY);
    /// Is this parametrization documented somewhere?    
    const double localUncertainty = m_scaleClusterError * (0.074 + 0.66 * theta - 0.15 * theta * theta);
    covMatrix(0, 0) = localUncertainty * localUncertainty;
}

StatusCode SimpleMMClusterBuilderTool::getCalibratedClusterPosition(const Muon::MMPrepData* cluster,
                                                                    std::vector<NSWCalib::CalibratedStrip>& strips, const float /*theta*/,
                                                                    Amg::Vector2D& clusterLocalPosition, Amg::MatrixX& covMatrix) const

{
    /// correct the precision coordinate of the local position based on the centroid calibration
    double xPosCalib = 0.0;
    double totalCharge = 0.0;
    for (const auto& it : strips) {
        xPosCalib += it.charge * it.dx;
        totalCharge += it.charge;
    }

    xPosCalib = xPosCalib / totalCharge;

    ATH_MSG_DEBUG("position before calibration and correction: " << clusterLocalPosition[Trk::locX] << " " << xPosCalib);

    clusterLocalPosition[Trk::locX] = clusterLocalPosition[Trk::locX] + xPosCalib;

    /// for the centroid, keep the covariance matrix as it is
    covMatrix = cluster->localCovariance();

    return StatusCode::SUCCESS;
}
