/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**@class MuonSegmentRegionRecoveryTool
 Tool to recover segments using the region selector
 This tool takes a track as input, extrapolates from the first hit to the next station
 Opens a cone around the center and tries to recover segments in the chambers involved.
 It returns a new track with a refined segment finding
 @author Giuseppe.Salamanna@cern.ch
************************************************/

#ifndef MUON_MUONSEGMENTREGIONRECOVERYTOOL_H
#define MUON_MUONSEGMENTREGIONRECOVERYTOOL_H



#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "IRegionSelector/IRegSelTool.h"
#include "IRegionSelector/RegSelEnums.h"
#include "MuidInterfaces/ICombinedMuonTrackBuilder.h"
#include "MuonChamberHoleRecoveryTool.h"
#include "MuonCondData/MdtCondDbData.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MuonPrepRawData/CscPrepDataCollection.h"
#include "MuonPrepRawData/MMPrepDataCollection.h"
#include "MuonPrepRawData/MdtPrepDataCollection.h"
#include "MuonPrepRawData/MuonPrepDataContainer.h"
#include "MuonPrepRawData/RpcPrepDataCollection.h"
#include "MuonPrepRawData/TgcPrepDataCollection.h"
#include "MuonPrepRawData/sTgcPrepDataCollection.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonRecHelperTools/IMuonEDMHelperSvc.h"
#include "MuonRecHelperTools/MuonEDMPrinterTool.h"
#include "MuonRecToolInterfaces/IMdtDriftCircleOnTrackCreator.h"
#include "MuonRecToolInterfaces/IMuonClusterOnTrackCreator.h"
#include "MuonRecToolInterfaces/IMuonHitSummaryTool.h"
#include "MuonRecToolInterfaces/IMuonHoleRecoveryTool.h"
#include "MuonRecToolInterfaces/IMuonSeededSegmentFinder.h"
#include "MuonRecToolInterfaces/IMuonTrackSegmentMatchingTool.h"
#include "MuonStationIntersectSvc/MuonStationIntersectSvc.h"
#include "MuonTrackMakerUtils/TrackStateOnSurfaceComparisonFunction.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkFitterInterfaces/ITrackFitter.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkToolInterfaces/IExtendedTrackSummaryTool.h"
#include "TrkToolInterfaces/IResidualPullCalculator.h"
#include "TrkToolInterfaces/ITrackHoleSearchTool.h"
#include "TrkToolInterfaces/ITrackSelectorTool.h"
#include "TrkTrack/Track.h"
#include "TrkTrackSummary/MuonTrackSummary.h"

#include <map>
#include <set>
#include <string>
#include <vector>

class IRoiDescriptor;

namespace Trk {
    class Track;
    class TrkDetElementBase;
    class MeasurementBase;
}  // namespace Trk

namespace Muon {

    /**
       @brief Implementation of an IMuonHoleRecoveryTool

       It propagates the track to a chamber where no segments where found and runs the SeededSegmentFinder in a region selected by the
       region selector
    */

    class MuonSegmentRegionRecoveryTool : virtual public IMuonHoleRecoveryTool, public AthAlgTool {
    public:
        struct MuonData {
            std::set<IdentifierHash> mdt;
            std::map<MuonStationIndex::ChIndex, std::set<IdentifierHash> > mdtPerStation;
            std::set<IdentifierHash> mdtTrack;
            std::vector<const MdtPrepDataCollection*> mdtCols;
            std::set<IdentifierHash> rpc;
            std::set<IdentifierHash> rpcTrack;
            std::vector<const RpcPrepDataCollection*> rpcCols;
            std::set<IdentifierHash> tgc;
            std::set<IdentifierHash> tgcTrack;
            std::vector<const TgcPrepDataCollection*> tgcCols;
            std::set<IdentifierHash> csc;
            std::set<IdentifierHash> cscTrack;
            std::vector<const CscPrepDataCollection*> cscCols;
            // New Small Wheel
            std::set<IdentifierHash> stgc;
            std::set<IdentifierHash> stgcTrack;
            std::vector<const sTgcPrepDataCollection*> stgcCols;
            std::set<IdentifierHash> mm;
            std::set<IdentifierHash> mmTrack;
            std::vector<const MMPrepDataCollection*> mmCols;
        };

    public:
        /** @brief constructor */
        MuonSegmentRegionRecoveryTool(const std::string&, const std::string&, const IInterface*);

        /** @brief destructor */
        virtual ~MuonSegmentRegionRecoveryTool() = default;

        /** @brief AlgTool initialize */
        virtual StatusCode initialize() override;

        /** @brief returns a new track with segments recovered using RegionSelector*/
        virtual std::unique_ptr<Trk::Track> recover(const Trk::Track& track, const EventContext& ctx) const override;

        void createHoleTSOSsForClusterChamber(const Identifier& detElId, const EventContext& ctx, const Trk::TrackParameters& pars,
                                              std::set<Identifier>& layIds,
                                              std::vector<std::unique_ptr<const Trk::TrackStateOnSurface> >& states) const override;

    private:
        std::unique_ptr<const Trk::TrackParameters> reachableDetEl(const EventContext& ctx, const Trk::Track& track,
                                                                   const Trk::TrkDetElementBase& detEl, bool smallerBounds = false) const;
        /** methods used by recover*/

        // Collect hashes through extrapolation
        void collectCrossedChambers(const Trk::Track& track, MuonData& data) const;

        // Fill already on track chamber std::set
        void fillOnTrackChambers(const Trk::Track& theTrack, MuonData& data) const;
        // Select hashes of chambers not yet on track
        std::unique_ptr<Trk::Track> addMissingChambers(const EventContext& ctx, const Trk::Track& track, MuonData& data, bool addMdt) const;

        void addHashes(DETID type, const IRoiDescriptor& roi, std::set<IdentifierHash>& hashes,
                       const std::set<IdentifierHash>& exclusion) const;

        std::unique_ptr<Trk::Track> findHoles(const EventContext& ctx, const Trk::Track& track, MuonData& data) const;

        SG::ReadCondHandleKey<MuonGM::MuonDetectorManager> m_DetectorManagerKey{this, "DetectorManagerKey", "MuonDetectorManager",
                                                                                "Key of input MuonDetectorManager condition data"};

        ServiceHandle<IMuonEDMHelperSvc> m_edmHelperSvc{this, "edmHelper", "Muon::MuonEDMHelperSvc/MuonEDMHelperSvc",
                                                        "Handle to the service providing the IMuonEDMHelperSvc interface"};
        ServiceHandle<MuonStationIntersectSvc> m_intersectSvc{this, "MuonStationIntersectSvc", "MuonStationIntersectSvc"};
        ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc{this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};

        ToolHandle<IMuonSeededSegmentFinder> m_seededSegmentFinder{this, "SeededSegmentFinder",
                                                                   "Muon::MuonSeededSegmentFinder/MuonSeededSegmentFinder"};
        ToolHandle<IMuonTrackSegmentMatchingTool> m_trackSegmentMatchingTool{this, "TrackSegmentMatchingTool",
                                                                             "Muon::MooCandidateMatchingTool/MooCandidateMatchingTool"};
       
        ToolHandle<MuonChamberHoleRecoveryTool> m_chamberHoleRecoveryTool{
            this, "ChamberHoleRecoveryTool", "", "hit-based hole search"};
        ToolHandle<Trk::IExtrapolator> m_extrapolator{this, "Extrapolator", "Trk::Extrapolator/MuonExtrapolator"};
        ToolHandle<Rec::ICombinedMuonTrackBuilder> m_builder{this, "Builder", ""};
        ToolHandle<Trk::ITrackFitter> m_fitter{this, "Fitter", "Trk::GlobalChi2Fitter/MCTBSLFitter"};
        ToolHandle<IMuonHitSummaryTool> m_hitSummaryTool{this, "HitSummaryTool", "Muon::MuonHitSummaryTool/MuonHitSummaryTool"};
        ToolHandle<IRegSelTool> m_regsel_mdt{this, "MDTRegionSelector", "RegSelTool/RegSelTool_MDT"};
        ToolHandle<IRegSelTool> m_regsel_csc{this, "CSCRegionSelector", "RegSelTool/RegSelTool_CSC"};
        ToolHandle<IRegSelTool> m_regsel_rpc{this, "RPCRegionSelector", "RegSelTool/RegSelTool_RPC"};
        ToolHandle<IRegSelTool> m_regsel_tgc{this, "TGCRegionSelector", "RegSelTool/RegSelTool_TGC"};
        ToolHandle<IRegSelTool> m_regsel_stgc{this, "STGCRegionSelector", "RegSelTool/RegSelTool_STGC"};
        ToolHandle<IRegSelTool> m_regsel_mm{this, "MMRegionSelector", "RegSelTool/RegSelTool_MM"};
        PublicToolHandle<MuonEDMPrinterTool> m_printer{this, "EDMPrinter", "Muon::MuonEDMPrinterTool/MuonEDMPrinterTool"};
        ToolHandle<Trk::IExtendedTrackSummaryTool> m_trackSummaryTool{this, "TrackSummaryTool", "MuonTrackSummaryTool"};

        SG::ReadCondHandleKey<MdtCondDbData> m_condKey{this, "MdtCondKey", "MdtCondDbData", "Key of MdtCondDbData"};
        // properties
        Gaudi::Property<double> m_deta{this, "DeltaEtaRegion", 0.05};
        Gaudi::Property<double> m_dphi{this, "DeltaPhiRegion", 0.1};
        Gaudi::Property<bool> m_excludeEES{this, "ExcludeEES", true};
        Gaudi::Property<bool> m_onlyEO{this, "OnlyEO", false};
        Gaudi::Property<bool> m_useFitterOutlierLogic{this, "UseFitterOutlierLogic", true};

        Gaudi::Property<bool> m_recoverMM{this, "RecoverMM", true, "Pick up dropped micromega chambers"};
        Gaudi::Property<bool> m_recoverSTGC{this, "RecoverSTGC", true, "Pick up dropped sTGC chambers" };

    };
}  // namespace Muon

#endif
