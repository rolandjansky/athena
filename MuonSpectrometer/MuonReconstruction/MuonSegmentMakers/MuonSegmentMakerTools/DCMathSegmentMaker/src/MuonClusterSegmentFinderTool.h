/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONROADFINDERTOOL_H
#define MUONROADFINDERTOOL_H

#include <utility>
#include <vector>

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MuonRIO_OnTrack/MuonClusterOnTrack.h"
#include "MuonRecHelperTools/IMuonEDMHelperSvc.h"
#include "MuonRecHelperTools/MuonEDMPrinterTool.h"
#include "MuonRecToolInterfaces/IMuonClusterOnTrackCreator.h"
#include "MuonRecToolInterfaces/IMuonTrackToSegmentTool.h"
#include "MuonSegment/MuonSegment.h"
#include "MuonSegmentMakerToolInterfaces/IMuonClusterSegmentFinderTool.h"
#include "TrkFitterInterfaces/ITrackFitter.h"
#include "TrkPseudoMeasurementOnTrack/PseudoMeasurementOnTrack.h"
#include "TrkToolInterfaces/ITrackAmbiguityProcessorTool.h"
#include "TrkToolInterfaces/ITrackSummaryTool.h"

namespace Muon {

    class IMuonTrackCleaner;

    class MuonClusterSegmentFinderTool : virtual public IMuonClusterSegmentFinderTool, public AthAlgTool {
    public:
        /** default constructor */
        MuonClusterSegmentFinderTool(const std::string& type, const std::string& name, const IInterface* parent);
        /** destructor */
        virtual ~MuonClusterSegmentFinderTool() = default;
  
        virtual StatusCode initialize() override;

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

        ToolHandle<Trk::ITrackAmbiguityProcessorTool> m_ambiTool{
            this,
            "SegmentAmbiguityTool",
            "Trk::SimpleAmbiguityProcessorTool/MuonAmbiProcessor",
        };  //!< Tool for ambiguity solving
        ToolHandle<Trk::ITrackFitter> m_slTrackFitter{
            this,
            "SLFitter",
            "Trk::GlobalChi2Fitter/MCTBSLFitter",
        };  //<! fitter, always use straightline
        ToolHandle<IMuonTrackToSegmentTool> m_trackToSegmentTool{
            this,
            "TrackToSegmentTool",
            "Muon::MuonTrackToSegmentTool/MuonTrackToSegmentTool",
        };  //<! track to segment converter
	PublicToolHandle<MuonEDMPrinterTool> m_printer{
            this,
            "Printer",
            "Muon::MuonEDMPrinterTool/MuonEDMPrinterTool",
        };
        ToolHandle<IMuonTrackCleaner> m_trackCleaner{
            this,
            "TrackCleaner",
            "Muon::MuonTrackCleaner/MuonTrackCleaner",
        };
        ToolHandle<Trk::ITrackSummaryTool> m_trackSummary{
            this,
            "TrackSummaryTool",
            "Trk::TrackSummaryTool/MuidTrackSummaryTool",
        };
        ToolHandle<IMuonClusterOnTrackCreator> m_mmClusterCreator{this, "MMClusterCreator",
                                                                  "Muon::MMClusterOnTrackCreator/MMClusterOnTrackCreator"};

        ToolHandle<IMuonClusterOnTrackCreator> m_stgcClusterCreator{this, "MuonClusterCreator",
                                                                  "Muon::MuonClusterOnTrackCreator/MuonClusterOnTrackCreator"};
     
        Gaudi::Property<bool> m_ipConstraint{this, "IPConstraint", true};  // use a ip perigee(0,0) constraint in the segment fit
        Gaudi::Property<double> m_maxClustDist{this, "ClusterDistance", 5.};
        Gaudi::Property<int> m_nOfSeedLayers{this, "NOfSeedLayers", 1};

    public:
        // find segments given a list of MuonCluster
        // segments can be added directly to a SegmentCollection, if they are to be written to SG, or returned in a list for
        // further processing
        void find(const EventContext& ctx, std::vector<const Muon::MuonClusterOnTrack*>& clusters, std::vector<std::unique_ptr<Muon::MuonSegment>>& segments,
                  Trk::SegmentCollection* segColl) const override;

    private:
        // reconstruct the segments in the precision (eta) plane
        void findPrecisionSegments(const EventContext& ctx, std::vector<const Muon::MuonClusterOnTrack*>& MuonClusters,
                                   std::vector<std::unique_ptr<Muon::MuonSegment>>& etaSegs) const;
        // recontruct 3D segments
        void find3DSegments(const EventContext& ctx, std::vector<const Muon::MuonClusterOnTrack*>& MuonClusters, std::vector<std::unique_ptr<Muon::MuonSegment>> & etaSegs,
                            std::vector<std::unique_ptr<Muon::MuonSegment>> & segments, Trk::SegmentCollection* segColl) const;
        // clean the clusters
        std::vector<const Muon::MuonClusterOnTrack*> cleanClusters(std::vector<const Muon::MuonClusterOnTrack*>& MuonClusters,
                                                                   bool selectPhiClusters ) const;

        // classify clusters by layer (one vector per layer)
        std::vector<std::vector<const Muon::MuonClusterOnTrack*> > classifyByLayer(std::vector<const Muon::MuonClusterOnTrack*>& clusters, bool usePads) const;

        // find segment seeds
        std::vector<std::pair<Amg::Vector3D, Amg::Vector3D> > segmentSeed(
            std::vector<std::vector<const Muon::MuonClusterOnTrack*> >& orderedClusters, bool usePhi) const;
	std::vector<std::pair<Amg::Vector3D,Amg::Vector3D> > segmentSeedFromMM(
	    std::vector< Amg::Vector3D >& phiStereo) const;
        std::vector<std::pair<Amg::Vector3D, Amg::Vector3D> > segmentSeedFromPads(
            std::vector<std::vector<const Muon::MuonClusterOnTrack*> >& orderedClusters, const Muon::MuonSegment* etaSeg) const;
        std::vector<std::pair<double, double> > getPadPhiOverlap(std::vector<std::vector<const Muon::MuonClusterOnTrack*> >& pads) const;
        
        // remove identical seeds from a vector
        void segmentSeedPhiOR(std::vector<std::pair<Amg::Vector3D, Amg::Vector3D> >& seeds) const;

        // associate clusters to the segment seeds
        int getClustersOnSegment(std::vector<std::vector<const Muon::MuonClusterOnTrack*> >& orderedclusters, 
                                 std::pair<Amg::Vector3D, Amg::Vector3D>& seed, 
                                 std::vector<const Muon::MuonClusterOnTrack*>& clusOnSeg) const;
        // get the clusters after calibration
        std::vector<const Muon::MuonClusterOnTrack*> getCalibratedClusters(std::vector<const Muon::MuonClusterOnTrack*>& clusters,
                                                                           std::pair<Amg::Vector3D, Amg::Vector3D>& seed) const;
        // distance of cluster to segment seed
        double clusterDistanceToSeed(const Muon::MuonClusterOnTrack* clust, std::pair<Amg::Vector3D, Amg::Vector3D>& seed) const;
        std::unique_ptr<Trk::Track> fit(const EventContext& ctx, const std::vector<const Trk::MeasurementBase*>& vec2, const Trk::TrackParameters& startpar) const;

	//get the phi-superpoint between two MM stereo layers
	std::vector<Amg::Vector3D> getPhiFromStereo(std::vector<const Muon::MuonClusterOnTrack*>& clusters) const;

    };

}  // namespace Muon
#endif  // MUONROADFINDERTOOL_H
