/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonLayerSegmentFinderTool.h"

#include "EventPrimitives/EventPrimitivesHelpers.h"
#include "FourMomUtils/xAODP4Helpers.h"
#include "MuonLayerHough/MuonLayerHough.h"
#include "MuonPrepRawDataProviderTools/MuonPrepRawDataCollectionProviderTool.h"
#include "MuonRIO_OnTrack/MdtDriftCircleOnTrack.h"
#include "MuonRIO_OnTrack/MuonClusterOnTrack.h"
#include "MuonSegment/MuonSegment.h"
#include "TrkSegment/SegmentCollection.h"
namespace {
    static const float OneOverSqrt12 = 1. / std::sqrt(12);
}
namespace Muon {

    MuonLayerSegmentFinderTool::MuonLayerSegmentFinderTool(const std::string& type, const std::string& name, const IInterface* parent) :
        AthAlgTool(type, name, parent) {
        declareInterface<IMuonLayerSegmentFinderTool>(this);
    }

    StatusCode MuonLayerSegmentFinderTool::initialize() {
        ATH_CHECK(m_idHelperSvc.retrieve());
        ATH_CHECK(m_printer.retrieve());
        ATH_CHECK(m_muonPRDSelectionTool.retrieve());
        ATH_CHECK(m_segmentMaker.retrieve());
        if (m_idHelperSvc->hasCSC() && !m_csc2dSegmentFinder.empty()) ATH_CHECK(m_csc2dSegmentFinder.retrieve());
        if (m_idHelperSvc->hasCSC() && !m_csc4dSegmentFinder.empty()) ATH_CHECK(m_csc4dSegmentFinder.retrieve());
        ATH_CHECK(m_clusterSegMakerNSW.retrieve());
        return StatusCode::SUCCESS;
    }

    void MuonLayerSegmentFinderTool::find(const EventContext& ctx, const MuonSystemExtension::Intersection& intersection, 
                                          const MuonLayerPrepRawData& layerPrepRawData, std::vector<std::shared_ptr<const Muon::MuonSegment> >& segments) const {
        ATH_MSG_VERBOSE(
            " Running segment finding in sector "
            << intersection.layerSurface.sector << " region " << MuonStationIndex::regionName(intersection.layerSurface.regionIndex)
            << " layer " << MuonStationIndex::layerName(intersection.layerSurface.layerIndex) << " intersection position: r "
            << intersection.trackParameters->position().perp() << " z " << intersection.trackParameters->position().z() << " locX "
            << intersection.trackParameters->parameters()[Trk::locX] << " locY " << intersection.trackParameters->parameters()[Trk::locY]
            << " phi " << intersection.trackParameters->position().phi());

        // run cluster hit based segment finding on PRDs
        findClusterSegments(ctx, intersection, layerPrepRawData, segments);
        ATH_MSG_VERBOSE(" findClusterSegments " << segments.size());

        // run standard MDT/Trigger hit segment finding either from Hough or hits
        findMdtSegments(intersection, layerPrepRawData, segments);
    }

    void MuonLayerSegmentFinderTool::findMdtSegments(const MuonSystemExtension::Intersection& intersection,
                                                     const MuonLayerPrepRawData& layerPrepRawData,
                                                     std::vector<std::shared_ptr<const Muon::MuonSegment>>& segments) const {
        // calibrate what is already there
        MuonLayerROTs layerROTs;
        if (!m_muonPRDSelectionTool->calibrateAndSelect(intersection, layerPrepRawData, layerROTs)) {
            ATH_MSG_WARNING("Failed to calibrate and select layer data");
            return;
        }

        // get hits
        MuonStationIndex::TechnologyIndex clusterTech =
            intersection.layerSurface.regionIndex == MuonStationIndex::Barrel ? MuonStationIndex::RPC : MuonStationIndex::TGC;
        const std::vector<const MdtDriftCircleOnTrack*>& mdts = layerROTs.getMdts();
        const std::vector<const MuonClusterOnTrack*>& clusters = layerROTs.getClusters(clusterTech);

        findMdtSegments(intersection, mdts, clusters, segments);
    }

    void MuonLayerSegmentFinderTool::findMdtSegments(const MuonSystemExtension::Intersection& intersection,
                                                     const std::vector<const MdtDriftCircleOnTrack*>& mdts,
                                                     const std::vector<const MuonClusterOnTrack*>& clusters,
                                                     std::vector<std::shared_ptr<const Muon::MuonSegment>>& segments) const {
        // require at least 2 MDT hits
        if (mdts.size() <= 2) return;
        // run segment finder
        std::unique_ptr<Trk::SegmentCollection> segColl = std::make_unique<Trk::SegmentCollection>(SG::VIEW_ELEMENTS);
        m_segmentMaker->find(intersection.trackParameters->position(), intersection.trackParameters->momentum(), mdts, clusters,
                             !clusters.empty(), segColl.get(), intersection.trackParameters->momentum().mag());

        for (Trk::Segment* tseg : *segColl) {
            MuonSegment* mseg = dynamic_cast<MuonSegment*>(tseg);
            ATH_MSG_DEBUG(" " << m_printer->print(*mseg));
            segments.emplace_back(mseg);
        }
    }

    void MuonLayerSegmentFinderTool::findClusterSegments(const EventContext& ctx, const MuonSystemExtension::Intersection& intersection,
                                                         const MuonLayerPrepRawData& layerPrepRawData,
                                                         std::vector<std::shared_ptr<const Muon::MuonSegment>>& segments) const {
        // if there are CSC hits run CSC segment finding
        if (!layerPrepRawData.cscs.empty()) findCscSegments(ctx, layerPrepRawData, segments);

        // No need to call the NSW segment finding
        if (layerPrepRawData.mms.empty() && layerPrepRawData.stgcs.empty()) return;
        // NSW segment finding
        MuonLayerROTs layerROTs;
        if (!m_muonPRDSelectionTool->calibrateAndSelect(intersection, layerPrepRawData, layerROTs)) {
            ATH_MSG_WARNING("Failed to calibrate and select layer data");
            return;
        }

        ATH_MSG_DEBUG(" MM prds " << layerPrepRawData.mms.size() << " STGC prds " << layerPrepRawData.stgcs.size());

        // get STGC and MM clusters
        const std::vector<const MuonClusterOnTrack*>& clustersSTGC = layerROTs.getClusters(MuonStationIndex::STGC);
        const std::vector<const MuonClusterOnTrack*>& clustersMM = layerROTs.getClusters(MuonStationIndex::MM);

        std::vector<const MuonClusterOnTrack*> clusters;
        if (!clustersSTGC.empty()) {
            ATH_MSG_DEBUG(" STGC clusters " << clustersSTGC.size());
            clusters.insert(clusters.end(), std::make_move_iterator(clustersSTGC.begin()), std::make_move_iterator(clustersSTGC.end()));
        }
        if (!clustersMM.empty()) {
            ATH_MSG_DEBUG(" MM clusters " << clustersMM.size());
            clusters.insert(clusters.end(), std::make_move_iterator(clustersMM.begin()), std::make_move_iterator(clustersMM.end()));
        }

        std::vector<std::unique_ptr<MuonSegment>> foundSegments;
        m_clusterSegMakerNSW->find(ctx, clusters, foundSegments, nullptr);
        
        for (std::unique_ptr<MuonSegment>& seg : foundSegments) {
            ATH_MSG_DEBUG(" NSW segment " << m_printer->print(*seg));
            segments.emplace_back(std::move(seg));
        }        
    }

    void MuonLayerSegmentFinderTool::findCscSegments(const EventContext& ctx, const MuonLayerPrepRawData& layerPrepRawData,
                                                     std::vector<std::shared_ptr<const Muon::MuonSegment>>& segments) const {
        // run 2d segment finder
        std::unique_ptr<MuonSegmentCombinationCollection> combi2D = m_csc2dSegmentFinder->find(layerPrepRawData.cscs, ctx);
        if (!combi2D) return;

        // run 4d segment finder
        std::unique_ptr<MuonSegmentCombinationCollection> combi4D = m_csc4dSegmentFinder->find(*combi2D, ctx);
        if (!combi4D) return;

        // extract segments and clean-up memory
        for (auto com : *combi4D) {
            const Muon::MuonSegmentCombination& combi = *com;
            unsigned int nstations = combi.numberOfStations();

            // loop over chambers in combi and extract segments
            for (unsigned int i = 0; i < nstations; ++i) {
                // loop over segments in station
                Muon::MuonSegmentCombination::SegmentVec* segs = combi.stationSegments(i);

                // check if not empty
                if (!segs || segs->empty()) continue;
                // loop over new segments, copy them into collection
                for (std::unique_ptr<MuonSegment>& seg_it : *segs) {
                    ATH_MSG_DEBUG(" " << m_printer->print(*seg_it));
                    segments.emplace_back(std::move(seg_it));
                }
            }
        }
    }
}  // namespace Muon
