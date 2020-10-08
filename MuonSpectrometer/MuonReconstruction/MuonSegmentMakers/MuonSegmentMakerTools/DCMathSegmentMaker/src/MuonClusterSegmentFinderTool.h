/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
    virtual ~MuonClusterSegmentFinderTool();

    static const InterfaceID& interfaceID();

    virtual StatusCode initialize(void) override;
    virtual StatusCode finalize(void) override;

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

    mutable ToolHandle<Trk::ITrackAmbiguityProcessorTool> m_ambiTool{
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
    ToolHandle<MuonEDMPrinterTool> m_printer{
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

    bool   m_ipConstraint;  // use a ip perigee(0,0) constraint in the segment fit
    double m_maxClustDist;

  public:
    // find segments given a list of MuonCluster
    // segments can be added directly to a SegmentCollection, if they are to be written to SG, or returned in a list for
    // further processing
    void find(std::vector<const Muon::MuonClusterOnTrack*>& clusters, std::vector<Muon::MuonSegment*>& segments,
              Trk::SegmentCollection* segColl = 0) const override;

  private:
    // reconstruct the segments in the precision (eta) plane
    void findPrecisionSegments(std::vector<const Muon::MuonClusterOnTrack*>& MuonClusters,
                               std::vector<Muon::MuonSegment*>&              etaSegs) const;
    // recontruct 3D segments
    void find3DSegments(std::vector<const Muon::MuonClusterOnTrack*>& MuonClusters,
                        std::vector<Muon::MuonSegment*>& etaSegs, std::vector<Muon::MuonSegment*>& segments,
                        Trk::SegmentCollection* segColl = 0) const;
    // clean the clusters
    std::vector<const Muon::MuonClusterOnTrack*> cleanClusters(
        std::vector<const Muon::MuonClusterOnTrack*>& MuonClusters, bool selectPhiClusters = false) const;
    // order the clusters
    std::vector<std::vector<const Muon::MuonClusterOnTrack*> > orderByLayer(
        std::vector<const Muon::MuonClusterOnTrack*>& clusters, bool usePads = false) const;
    // find segment seeds
    std::vector<std::pair<Amg::Vector3D, Amg::Vector3D> > segmentSeed(
        std::vector<std::vector<const Muon::MuonClusterOnTrack*> >& orderedClusters, bool usePhi) const;
    std::vector<std::pair<Amg::Vector3D, Amg::Vector3D> > segmentSeedFromPads(
        std::vector<std::vector<const Muon::MuonClusterOnTrack*> >& orderedClusters,
        const Muon::MuonSegment*                                    etaSeg) const;
    std::vector<std::pair<double, double> > getPadPhiOverlap(
        std::vector<std::vector<const Muon::MuonClusterOnTrack*> >& pads) const;
    // associate clusters to the segment seeds
    std::vector<const Muon::MuonClusterOnTrack*> getClustersOnSegment(
        std::vector<std::vector<const Muon::MuonClusterOnTrack*> >& clusters,
        std::pair<Amg::Vector3D, Amg::Vector3D>& seed, bool tight) const;
    // distance of cluster to segment seed
    double        clusterDistanceToSeed(const Muon::MuonClusterOnTrack*          clust,
                                        std::pair<Amg::Vector3D, Amg::Vector3D>& seed) const;
    Amg::Vector3D intersectPlane(const Trk::PlaneSurface& surf, const Amg::Vector3D& pos,
                                 const Amg::Vector3D& dir) const;
    Trk::Track*   fit(const std::vector<const Trk::MeasurementBase*>& vec2, const Trk::TrackParameters& startpar) const;

    // check if enough surfaces are hitted
    bool belowThreshold(std::vector<const Muon::MuonClusterOnTrack*>& muonClusters, int threshold) const;
};


}  // namespace Muon
#endif  // MUONROADFINDERTOOL_H
