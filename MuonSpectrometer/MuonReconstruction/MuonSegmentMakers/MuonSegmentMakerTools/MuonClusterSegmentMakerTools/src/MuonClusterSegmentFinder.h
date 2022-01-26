/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_MUONCLUSTERSEGMENTFINDER_H
#define MUON_MUONCLUSTERSEGMENTFINDER_H

#include <string>
#include <vector>

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuonClusterization/IMuonClusterizationTool.h"
#include "MuonDetDescrUtils/MuonSectorMapping.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MuonLinearSegmentMakerUtilities/ClusterNtuple.h"
#include "MuonPrepRawData/MuonCluster.h"
#include "MuonPrepRawData/RpcPrepDataCollection.h"
#include "MuonPrepRawData/TgcPrepDataCollection.h"
#include "MuonPrepRawDataProviderTools/MuonLayerHashProviderTool.h"
#include "MuonRIO_OnTrack/MuonClusterOnTrack.h"
#include "MuonRecHelperTools/IMuonEDMHelperSvc.h"
#include "MuonRecHelperTools/MuonEDMPrinterTool.h"
#include "MuonRecToolInterfaces/IMuonClusterOnTrackCreator.h"
#include "MuonRecToolInterfaces/IMuonPRDSelectionTool.h"
#include "MuonRecToolInterfaces/IMuonSegmentMaker.h"
#include "MuonRecToolInterfaces/IMuonTrackCleaner.h"
#include "MuonRecToolInterfaces/IMuonTrackToSegmentTool.h"
#include "MuonSegment/MuonSegment.h"
#include "MuonSegmentMakerToolInterfaces/IMuonClusterSegmentFinder.h"
#include "MuonSegmentMakerToolInterfaces/IMuonSegmentOverlapRemovalTool.h"
#include "MuonSegmentMakerUtils/CompareMuonSegmentKeys.h"
#include "MuonSegmentMakerUtils/MuonSegmentKey.h"
#include "TrkFitterInterfaces/ITrackFitter.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkToolInterfaces/ITrackAmbiguityProcessorTool.h"
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkTruthData/PRD_MultiTruthCollection.h"

class TTree;
class TFile;

namespace Trk {
    class MeasurementBase;
}  // namespace Trk

namespace Muon {

    struct candEvent {
        candEvent() {
            m_segTrkColl = std::make_unique<TrackCollection>();
            m_resolvedTracks = std::make_unique<TrackCollection>();
        }
        ~candEvent() = default;

        TrackCollection* segTrkColl() const { return m_segTrkColl.get(); }
        std::vector<const MuonClusterOnTrack*>& clusters() { return m_clusters; }
        std::vector<ClusterSeg::Cluster*>& Clust() { return m_Clust; }
        std::vector<std::vector<const MuonClusterOnTrack*>>& hits() { return m_hits; }
        std::vector<std::pair<Amg::Vector3D, Amg::Vector3D>>& trackSeeds() { return m_trackSeeds; }
        std::vector<MuonSegmentKey>& keyVector() { return m_keyVector; }
        std::vector<std::vector<ClusterSeg::SpacePoint>>& SPoints() { return m_SPoints; }
        MuonSectorMapping& sectorMapping() { return m_sectorMapping; }
        TrackCollection* resolvedTracks() const { return m_resolvedTracks.get(); }
        std::vector<std::pair<Amg::Vector3D, Amg::Vector3D>>& resolvedTrackSeeds() { return m_resolvedTrackSeeds; }
        std::vector<std::vector<const MuonClusterOnTrack*>>& resolvedhits() { return m_resolvedhits; }

        std::unique_ptr<TrackCollection> m_segTrkColl{nullptr};
        std::unique_ptr<TrackCollection> m_resolvedTracks{nullptr};

        std::vector<const MuonClusterOnTrack*> m_clusters;
        std::vector<ClusterSeg::Cluster*> m_Clust;
        std::vector<std::vector<const MuonClusterOnTrack*>> m_hits;
        std::vector<std::pair<Amg::Vector3D, Amg::Vector3D>> m_trackSeeds;
        std::vector<MuonSegmentKey> m_keyVector;
        std::vector<std::vector<ClusterSeg::SpacePoint>> m_SPoints;
        MuonSectorMapping m_sectorMapping;
        std::vector<std::pair<Amg::Vector3D, Amg::Vector3D>> m_resolvedTrackSeeds;
        std::vector<std::vector<const MuonClusterOnTrack*>> m_resolvedhits;
    };

    class MuonClusterSegmentFinder : virtual public IMuonClusterSegmentFinder, public AthAlgTool {
    public:
        /** Default AlgTool functions */
        MuonClusterSegmentFinder(const std::string& type, const std::string& name, const IInterface* parent);
        virtual ~MuonClusterSegmentFinder() = default;
        StatusCode initialize();
        StatusCode finalize();

        void getClusterSegments(const Muon::MdtPrepDataContainer* mdtPrdCont, const Muon::RpcPrepDataContainer* rpcPrdCont,
                                const Muon::TgcPrepDataContainer* tgcPrdCont, const PRD_MultiTruthCollection* tgcTruthColl,
                                const PRD_MultiTruthCollection* rpcTruthColl, Trk::SegmentCollection* segColl) const;

        void getClusterSegments(const Muon::MdtPrepDataContainer* mdtPrdCont, std::vector<const Muon::TgcPrepDataCollection*>* tgcCols,
                                std::vector<const Muon::RpcPrepDataCollection*>* rpcCols, const PRD_MultiTruthCollection* tgcTruthColl,
                                const PRD_MultiTruthCollection* rpcTruthColl, Trk::SegmentCollection* segColl) const;

        /** tgc segment finding */
        void findSegments(std::vector<const TgcPrepDataCollection*>& tgcCols, const Muon::MdtPrepDataContainer* mdtPrdCont,
                          Trk::SegmentCollection* segColl, const PRD_MultiTruthCollection* tgcTruthColl) const;
        /** rpc segment finding */
        void findSegments(std::vector<const RpcPrepDataCollection*>& rpcCols, const Muon::MdtPrepDataContainer* mdtPrdCont,
                          Trk::SegmentCollection* segColl, const PRD_MultiTruthCollection* tgcTruthColl) const;

    private:
        ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc{
            this,
            "MuonIdHelperSvc",
            "Muon::MuonIdHelperSvc/MuonIdHelperSvc",
        };
        ServiceHandle<IMuonEDMHelperSvc> m_edmHelperSvc{
            this,
            "edmHelper",
            "Muon::MuonEDMHelperSvc/MuonEDMHelperSvc",
            "Handle to the service providing the IMuonEDMHelperSvc interface",
        };  //<! Id helper tool

        ToolHandle<MuonEDMPrinterTool> m_printer{
            this,
            "MuonEDMPrinterTool",
            "Muon::MuonEDMPrinterTool/MuonEDMPrinterTool",
        };
        ToolHandle<MuonLayerHashProviderTool> m_layerHashProvider{
            this,
            "MuonLayerHashProviderTool",
            "Muon::MuonLayerHashProviderTool/MuonLayerHashProviderTool",
        };
        ToolHandle<IMuonPRDSelectionTool> m_muonPRDSelectionTool{
            this,
            "MuonPRDSelectionTool",
            "Muon::MuonPRDSelectionTool/MuonPRDSelectionTool",
        };
        ToolHandle<IMuonSegmentMaker> m_segmentMaker{
            this,
            "MdtSegmentMaker",
            "Muon::DCMathSegmentMaker/DCMathSegmentMaker",
        };
        ToolHandle<Muon::IMuonClusterizationTool> m_clusterTool{
            this,
            "MuonClusterizationTool",
            "Muon::MuonClusterizationTool/MuonClusterizationTool",
        };  //<! clustering tool
        ToolHandle<IMuonClusterOnTrackCreator> m_clusterCreator{
            this,
            "MuonClusterOnTrackCreator",
            "Muon::MuonClusterOnTrackCreator/MuonClusterOnTrackCreator",
        };
        ToolHandle<IMuonTrackToSegmentTool> m_trackToSegmentTool{
            this,
            "TrackToSegmentTool",
            "Muon::MuonTrackToSegmentTool/MuonTrackToSegmentTool",
        };  //<! track to segment converter
        ToolHandle<Trk::ITrackFitter> m_slTrackFitter{
            this,
            "SLFitter",
            "Trk::GlobalChi2Fitter/MCTBSLFitter",
        };  //<! fitter, always use straightline
        ToolHandle<Trk::ITrackAmbiguityProcessorTool> m_ambiguityProcessor{
            this,
            "AmbiguityProcessor",
            "Trk::TrackSelectionProcessorTool/MuonAmbiProcessor",
        };  //!< Tool for ambiguity solving
        ToolHandle<IMuonTrackCleaner> m_trackCleaner{
            this,
            "TrackCleaner",
            "Muon::MuonTrackCleaner/MuonTrackCleaner",
        };
        ToolHandle<IMuonSegmentOverlapRemovalTool> m_segmentOverlapRemovalTool{
            this,
            "MuonSegmentOverlapRemovalTool",
            "Muon::MuonSegmentOverlapRemovalTool/MuonSegmentOverlapRemovalTool",
        };

        bool m_doNtuple{false};
        struct Tree {
          TFile* m_file{nullptr};
          TTree* m_tree{nullptr};
          ClusterSeg::ClusterNtuple* m_ntuple{nullptr};
        };
        std::unique_ptr<Tree> m_tree;

        bool matchTruth(const PRD_MultiTruthCollection& truthCol, const Identifier& id, int& barcode) const;
        Trk::Track* fit(const std::vector<const Trk::MeasurementBase*>& vec2, const Trk::TrackParameters& startpar) const;
        void makeClusterVecs(const std::vector<const Muon::MuonClusterOnTrack*>& clustCol, candEvent* theEvent) const;
        void makeClusterVecs(const PRD_MultiTruthCollection* truthCollectionTGC, const std::vector<const TgcPrepDataCollection*>& tgcCols,
                             candEvent* theEvent) const;
        void makeClusterVecs(const PRD_MultiTruthCollection* truthCollectionRPC, const std::vector<const RpcPrepDataCollection*>& rpcCols,
                             candEvent* theEvent) const;
        void findOverlap(std::map<int, bool>& themap, candEvent* theEvent) const;
        void processSpacePoints(candEvent* theEvent, std::vector<std::vector<ClusterSeg::SpacePoint>>& sPoints) const;
        void resolveCollections(std::map<int, bool> themap, candEvent* theEvent) const;
        void getSegments(candEvent* theEvent, const Muon::MdtPrepDataContainer* mdtPrdCont, Trk::SegmentCollection* segColl) const;
        bool getLayerData(int sector, MuonStationIndex::DetectorRegionIndex regionIndex, MuonStationIndex::LayerIndex layerIndex,
                          const Muon::MdtPrepDataContainer* input, std::vector<const MdtPrepDataCollection*>& output) const;

      Tree& getTree() const;
    };

}  // namespace Muon

#endif
