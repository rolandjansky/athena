/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_MUONTRUTHTOOLS_MUONTRUTHTRACKBUILDER_H
#define MUON_MUONTRUTHTOOLS_MUONTRUTHTRACKBUILDER_H

// Gaudi
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
// Trk includes
#include <vector>

#include "GeoPrimitives/GeoPrimitives.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MuonRecHelperTools/IMuonEDMHelperSvc.h"
#include "MuonRecHelperTools/MuonEDMPrinterTool.h"
#include "MuonRecToolInterfaces/IMdtDriftCircleOnTrackCreator.h"
#include "MuonRecToolInterfaces/IMuonClusterOnTrackCreator.h"
#include "MuonRecToolInterfaces/IMuonCompetingClustersOnTrackCreator.h"
#include "MuonRecToolInterfaces/IMuonTrackCleaner.h"
#include "MuonRecToolInterfaces/IMuonTrackExtrapolationTool.h"
#include "MuonRecToolInterfaces/IMuonTrackToSegmentTool.h"
#include "MuonSimData/CscSimDataCollection.h"
#include "MuonSimData/MuonSimDataCollection.h"
#include "TrackRecord/TrackRecord.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkFitterInterfaces/ITrackFitter.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkSegment/SegmentCollection.h"
#include "TrkTruthTrackInterfaces/ITruthTrackBuilder.h"

namespace Trk {
class Track;
class MeasurementBase;
class PseudoMeasurementOnTrack;
}  // namespace Trk

namespace Muon {

class MuonSegment;
/**
   @class MuonTruthTrackBuilder

   @brief The class builds a track from a list of true hits in the muon system

   @author Niels van Eldik
*/

class MuonTruthTrackBuilder : public AthAlgTool, virtual public Trk::ITruthTrackBuilder {
  private:
    struct DetectorLayer {
        DetectorLayer()
            : stIndex(MuonStationIndex::BI),
              isEndcap(false),
              minPos(1e9),
              maxPos(-1e9),
              nnsw(0),
              nmdtS(0),
              nmdtL(0),
              nphi(0)
        {
        }
        MuonStationIndex::StIndex                stIndex;
        bool                                     isEndcap;
        double                                   minPos;  // flag whether first and second globalpos have been filled
        double                                   maxPos;
        int                                      nnsw;
        int                                      nmdtS;
        int                                      nmdtL;
        int                                      nphi;
        Amg::Vector3D                            first3D;
        Amg::Vector3D                            last3D;
        std::vector<const Trk::MeasurementBase*> meas;
    };

    struct SortDetectorLayers {
        SortDetectorLayers(bool isEndcap_) : isEndcap(isEndcap_) {}

        bool operator()(const DetectorLayer* d1, const DetectorLayer* d2) const
        {
            Amg::Vector3D p1 = d1->minPos < 1e8 ? d1->first3D : d1->meas.front()->globalPosition();
            Amg::Vector3D p2 = d2->minPos < 1e8 ? d2->first3D : d2->meas.front()->globalPosition();
            if (isEndcap) return fabs(p1.z()) < fabs(p2.z());
            return p1.perp() < p2.perp();
        }

        bool isEndcap;
    };

  public:
    //** Constructor with parameters */
    MuonTruthTrackBuilder(const std::string& t, const std::string& n, const IInterface* p);

    // Athena algtool's Hooks
    StatusCode initialize();
    StatusCode finalize();

    /** return a map of GenParticles to PRDs for further processing **/
    Trk::Track* createTrack(const Trk::PRD_TruthTrajectory& prdTraj, Trk::SegmentCollection* segs = 0) const;

  private:
    MuonSegment* createSegment(const Trk::TrackParameters& pars, std::vector<const Trk::MeasurementBase*>& hits) const;
    Trk::PseudoMeasurementOnTrack* createPseudo(const Trk::TrackParameters& pars,
                                                const Trk::MeasurementBase& meas) const;
    const TrackRecord*             getEntryRecord(const HepMC::GenParticle& genPart) const;
    const MuonSimData::Deposit*    getDeposit(const MuonSimDataCollection& simCol, const HepMC::GenParticle& genPart,
                                              const Identifier& id) const;
    void                           createSegments(DetectorLayer& layer, Trk::SegmentCollection& segs) const;
    const MuonSimDataCollection*   retrieveTruthCollection(std::string colName) const;
    const CscSimDataCollection*    retrieveCscTruthCollection(std::string colName) const;

    void sortLayer(DetectorLayer& layer) const;

    // extract the measurements to be used in a segment or track fit and add pseudo measurements if needed
    // splitSL == -1, no splitting, == 0 large sectors, == 1 small sectors
    // splitNSWEI == -1 no splitting, == 0 MDT/TGC hits, == 1 STGC/MM
    bool extractMeasurements(const Trk::TrackParameters& pars, const std::vector<const Trk::MeasurementBase*>& hitsIn,
                             std::vector<const Trk::MeasurementBase*>& hitsOut, int splitSL, int splitNSWEI,
                             bool isEndcap, double phiDist) const;

    ToolHandle<Trk::ITrackFitter>                    m_trackFitter;
    ToolHandle<Trk::ITrackFitter>                    m_slTrackFitter;
    ToolHandle<Trk::IExtrapolator>                   m_extrapolator;
    ToolHandle<IMuonTrackToSegmentTool>              m_trackToSegmentTool;
    ToolHandle<IMdtDriftCircleOnTrackCreator>        m_mdtCreator;
    ToolHandle<IMuonClusterOnTrackCreator>           m_muonClusterCreator;
    ToolHandle<IMuonCompetingClustersOnTrackCreator> m_muonCompRotCreator;
    ToolHandle<MuonEDMPrinterTool>                   m_printer;
    ToolHandle<IMuonTrackExtrapolationTool>          m_trackExtrapolationTool;
    ToolHandle<IMuonTrackCleaner>                    m_trackCleaner;

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
    };


    size_t      m_minNdof;               //!< checks min degrees of freedom if bigger -1
    bool        m_onlyPrimaries;         //!< restrict track creation to primaries
    int         m_primaryBarcodeCutOff;  //!< primary barcode cut off
    bool        m_buildCompRots;
    bool        m_usePadPhiHits;
    std::string m_MDT_SimDataMapName;
    std::string m_MM_SimDataMapName;
    std::string m_STGC_SimDataMapName;
    std::string m_CSC_SimDataMapName;
    std::string m_RPC_SimDataMapName;
    std::string m_TGC_SimDataMapName;

    mutable std::vector<int> m_nsegmentFits;
    mutable std::vector<int> m_nsegmentCleaned;
    mutable std::vector<int> m_nfailedSegmentFits;
    mutable std::vector<int> m_nfailedSegmentCleaning;
    mutable std::vector<int> m_ngoodSegmentFits;
    mutable std::vector<int> m_nallSegments;
    mutable int              m_truth;
    mutable int              m_trackFits;
    mutable int              m_trackCleaned;
    mutable int              m_failedTrackFits;
    mutable int              m_failedTrackCleaning;
    mutable int              m_goodTrackFits;
    mutable int              m_truthSL;
    mutable int              m_trackSLFits;
    mutable int              m_trackCleanedSL;
    mutable int              m_failedTrackSLFits;
    mutable int              m_failedTrackCleaningSL;
    mutable int              m_goodTrackSLFits;
};


}  // namespace Muon

#endif  // TRK_TRUTHTRACKTOOLS_TRUTHTRACKBUILDER_H
