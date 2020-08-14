/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONREFITTOOL_H
#define MUONREFITTOOL_H

#include "MuonRecToolInterfaces/IMuonRefitTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "MuonRecHelperTools/IMuonEDMHelperSvc.h"
#include "MuonRecHelperTools/MuonEDMPrinterTool.h"
#include "TrkDriftCircleMath/DCSLFitter.h"
#include "TrkDriftCircleMath/SegmentFinder.h"
#include "TrkParameters/TrackParameters.h"
#include "MuonRIO_OnTrack/MuonDriftCircleErrorStrategy.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MuonRecToolInterfaces/IMuonTrackExtrapolationTool.h"
#include "MuonRecToolInterfaces/IMdtDriftCircleOnTrackCreator.h"
#include "MuonRecToolInterfaces/IMuonCompetingClustersOnTrackCreator.h"
#include "TrkFitterInterfaces/ITrackFitter.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "MuonSegmentMakerInterfaces/IDCSLFitProvider.h"
#include "TrkToolInterfaces/ITrkAlignmentDeviationTool.h"

#include <string>
#include <vector>
#include <set>
#include <atomic>

namespace Muon {

  class MuonRefitTool : virtual public IMuonRefitTool, public AthAlgTool { 
  public:

    MuonRefitTool(const std::string& ty, const std::string& na, const IInterface* pa);
    virtual ~MuonRefitTool()=default; 
    virtual StatusCode initialize() override;
    virtual StatusCode finalize() override;

    /** refit a track */
    std::unique_ptr<Trk::Track> refit( Trk::Track* track, const Settings* settings = 0 ) const override;

    /** refit and back extrapolate a vector of track pairs */
    std::vector<std::unique_ptr<Trk::Track> > refit( std::vector<Trk::Track*>& tracks, const Settings* settings = 0 ) const override;

  protected:

    /** update errors on a muon track */
    std::unique_ptr<Trk::Track> updateErrors( Trk::Track* track, const Settings& settings ) const;

    std::unique_ptr<Trk::Track> updateMdtErrors( Trk::Track* track, const Settings& settings ) const;

    std::unique_ptr<Trk::Track> updateAlignmentErrors( Trk::Track* track, const Settings& settings ) const;

    std::unique_ptr<Trk::Track> makeAEOTs( Trk::Track* track ) const;

    std::unique_ptr<Trk::Track> makeSimpleAEOTs( Trk::Track* track ) const;

    std::unique_ptr<Trk::Track> removeOutliers( Trk::Track* track,const Settings& settings ) const;

    bool removeMdtOutliers( const Trk::TrackParameters& pars, const std::vector<const MdtDriftCircleOnTrack*>& hits,   
			    std::set<Identifier>& removedIdentifiers, const Settings& settings ) const;

    struct State {
      State() : originalState(0), newState(0), copyOriginal(true) {}
      const Trk::TrackStateOnSurface* originalState;
      const Trk::TrackStateOnSurface* newState;
      bool copyOriginal;
    };

  private:
    ServiceHandle<IMuonEDMHelperSvc> m_edmHelperSvc {this, "edmHelper", "Muon::MuonEDMHelperSvc/MuonEDMHelperSvc", "Handle to the service providing the IMuonEDMHelperSvc interface" };
    ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
    
    ToolHandle<Trk::ITrkAlignmentDeviationTool> m_alignErrorTool {this, "AlignmentErrorTool", "MuonAlign::AlignmentErrorTool"};
    ToolHandle<MuonEDMPrinterTool> m_printer {this, "Printer", "Muon::MuonEDMPrinterTool/MuonEDMPrinterTool"};
    ToolHandle<Trk::ITrackFitter> m_trackFitter {this, "Fitter", "Trk::GlobalChi2Fitter/MCTBFitterMaterialFromTrack"};
    ToolHandle<Trk::IExtrapolator> m_extrapolator {this, "Extrapolator", "Trk::Extrapolator/AtlasExtrapolator"};
    ToolHandle<Trk::IExtrapolator> m_muonExtrapolator {this, "MuonExtrapolator", "Trk::Extrapolator/MuonExtrapolator"};
    ToolHandle<IMdtDriftCircleOnTrackCreator> m_mdtRotCreator {this, "MdtRotCreator", "Muon::MdtDriftCircleOnTrackCreator/MdtDriftCircleOnTrackCreator"};
    ToolHandle<IMuonCompetingClustersOnTrackCreator> m_compClusterCreator {this, "CompClusterCreator", "Muon::TriggerChamberClusterOnTrackCreator/TriggerChamberClusterOnTrackCreator"};
    ToolHandle<IDCSLFitProvider> m_t0Fitter {this, "T0Fitter", ""};
    ToolHandle<Muon::IMuonTrackExtrapolationTool> m_muonEntryTrackExtrapolator {this, "MuonEntryExtrapolationTool", "Muon::MuonTrackExtrapolationTool/MuonTrackExtrapolationTool"};

    Gaudi::Property<int> m_finderDebugLevel {this, "FinderDebugLevel", -1};

    Gaudi::Property<double> m_minMuonMomentum {this, "MinMuonMom", 4000};
    Gaudi::Property<double> m_fixedError {this, "UsedFixedError", 1};
    Gaudi::Property<double> m_flagT0FitRange {this, "FlagT0FitRange", 0.00005};

    Gaudi::Property<float> m_alignmentDelta {this, "AlignmentDelta", 0};
    Gaudi::Property<float> m_alignmentAngle {this, "AlignmentAngle", 0};
    Gaudi::Property<float> m_alignmentDeltaError {this, "AlignmentDeltaError", 0.1};
    Gaudi::Property<float> m_alignmentAngleError {this, "AlignmentAngleError", 0.001};

    Gaudi::Property<bool> m_alignmentErrors {this, "AlignmentErrors", false};
    Gaudi::Property<bool> m_simpleAEOTs {this, "SimpleAEOTs", false};
    Gaudi::Property<bool> m_addAll {this, "AddAll", false};
    Gaudi::Property<bool> m_addInner {this, "AddInner", false};
    Gaudi::Property<bool> m_addMiddle {this, "AddMiddle", false};
    Gaudi::Property<bool> m_addTwo {this, "AddTwo", false};
    Gaudi::Property<bool> m_deweightBEE {this, "DeweightBEE", false};
    Gaudi::Property<bool> m_deweightEE {this, "DeweightEE", false};
    Gaudi::Property<bool> m_deweightBIS78 {this, "DeweightBIS78", true};
    Gaudi::Property<bool> m_deweightBME {this, "DeweightBME", true};
    Gaudi::Property<bool> m_deweightBOE {this, "DeweightBOE", true};
    Gaudi::Property<bool> m_deweightEEL1C05 {this, "DeweightEEL1C05", false};
    Gaudi::Property<bool> m_deweightTwoStationTracks {this, "DeweightTwoStationTracks", false};

    MuonDriftCircleErrorStrategy m_errorStrategyBEE;
    MuonDriftCircleErrorStrategy m_errorStrategyEE;
    MuonDriftCircleErrorStrategy m_errorStrategyBIS78;
    MuonDriftCircleErrorStrategy m_errorStrategyBXE;
    MuonDriftCircleErrorStrategy m_errorStrategyEEL1C05;
    MuonDriftCircleErrorStrategy m_errorStrategyBarEnd;
    MuonDriftCircleErrorStrategy m_errorStrategySL;
    MuonDriftCircleErrorStrategy m_errorStrategyTwoStations;
    MuonDriftCircleErrorStrategy m_errorStrategy;
    MuonDriftCircleErrorStrategy m_muonErrorStrategy;

    Settings m_defaultSettings;

    mutable std::atomic<unsigned int> m_nrefits;
    mutable std::atomic<unsigned int> m_ngoodRefits;
    mutable std::atomic<unsigned int> m_failedOutlierRemoval;
    mutable std::atomic<unsigned int> m_failedErrorUpdate;
    mutable std::atomic<unsigned int> m_failedRefit;
    mutable std::atomic<unsigned int> m_failedExtrapolationMuonEntry;

    std::unique_ptr<const Trk::Perigee> createPerigee( const Trk::TrackParameters& pars ) const;
  }; 
}

#endif 
