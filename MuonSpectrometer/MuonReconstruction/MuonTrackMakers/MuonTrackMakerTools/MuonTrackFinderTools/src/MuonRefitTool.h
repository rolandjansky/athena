/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONREFITTOOL_H
#define MUONREFITTOOL_H

// STL includes
#include <string>
#include <vector>
#include <set>

// FrameWork includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuonRecToolInterfaces/IMuonRefitTool.h"

#include "TrkDriftCircleMath/DCSLFitter.h"
#include "TrkDriftCircleMath/SegmentFinder.h"
#include "Identifier/Identifier.h"
#include "TrkParameters/TrackParameters.h"
#include "MuonRIO_OnTrack/MuonDriftCircleErrorStrategy.h"

namespace Muon {
  class MuonEDMPrinterTool;
  class MuonEDMHelperTool;
  class MuonIdHelperTool;
  class IMdtDriftCircleOnTrackCreator;
  class IMuonClusterOnTrackCreator;
  class IMuonCompetingClustersOnTrackCreator;
  class IMuonTrackExtrapolationTool;
  class MdtDriftCircleOnTrack;
  class IDCSLFitProvider;
}
class MuonESDAnalysisHelperTool;

namespace Trk {
  class Track;
  class ITrackFitter;
  class IExtrapolator;
  class MeasurementBase;
  class ITrkAlignmentDeviationTool;
}

namespace Muon {

  class MuonRefitTool : virtual public IMuonRefitTool, public AthAlgTool { 
  public:

    /** Constructor with parameters: */
    MuonRefitTool( const std::string& ty,const std::string& na,const IInterface* pa);

    /** Destructor: */
    virtual ~MuonRefitTool(); 

    virtual StatusCode  initialize();
    virtual StatusCode  finalize();


    /** refit a track */
    const Trk::Track* refit( const Trk::Track& track, const Settings* settings = 0 ) const;

    /** refit and back extrapolate a vector of track pairs */
    std::vector<const Trk::Track*> refit( const std::vector<const Trk::Track*>& tracks, const Settings* settings = 0 ) const;

  protected:

    /** update errors on a muon track */
    Trk::Track* updateErrors( const Trk::Track& track, const Settings& settings ) const;

    const Trk::Track* removeOutliers( const Trk::Track& track,const Settings& settings ) const;

    bool removeMdtOutliers( const Trk::TrackParameters& pars, const std::vector<const MdtDriftCircleOnTrack*>& hits,   
			    std::set<Identifier>& removedIdentifiers, const Settings& settings ) const;

    ToolHandle<MuonEDMPrinterTool>  m_printer; //<! helper to nicely print out tracks
    ToolHandle<MuonEDMHelperTool>   m_helper; //<! muon EDM helper
    ToolHandle<MuonIdHelperTool>    m_idHelper; //<! muon id helper
    ToolHandle<Trk::ITrkAlignmentDeviationTool>  m_alignErrorTool; //<! alignment error tool

    struct State {
      State() : originalState(0), newState(0), copyOriginal(true) {}
      const Trk::TrackStateOnSurface* originalState;
      const Trk::TrackStateOnSurface* newState;
      bool copyOriginal;
    };

    ToolHandle<Trk::ITrackFitter>                    m_trackFitter;
    ToolHandle<Trk::IExtrapolator>                   m_extrapolator;
    ToolHandle<IMdtDriftCircleOnTrackCreator>        m_mdtRotCreator;
    ToolHandle<IMuonClusterOnTrackCreator>           m_cscRotCreator;
    ToolHandle<IMuonClusterOnTrackCreator>           m_triggerRotCreator;
    ToolHandle<IMuonCompetingClustersOnTrackCreator> m_compClusterCreator;
    ToolHandle<IDCSLFitProvider>                     m_t0Fitter;
    ToolHandle<Muon::IMuonTrackExtrapolationTool>    m_muonEntryTrackExtrapolator;

    MuonDriftCircleErrorStrategy m_errorStrategyBEE;
    MuonDriftCircleErrorStrategy m_errorStrategyEE;
    MuonDriftCircleErrorStrategy m_errorStrategyBIS78;
    MuonDriftCircleErrorStrategy m_errorStrategyBarEnd;
    MuonDriftCircleErrorStrategy m_errorStrategySL;
    MuonDriftCircleErrorStrategy m_errorStrategyTwoStations;
    MuonDriftCircleErrorStrategy m_errorStrategy;

    mutable TrkDriftCircleMath::DCSLFitter    m_fitter;
    mutable TrkDriftCircleMath::SegmentFinder m_finder;
    mutable TrkDriftCircleMath::SegmentFinder m_finderCleaning;

    int m_finderDebugLevel;

    Settings m_defaultSettings;

    mutable unsigned int m_nrefits;
    mutable unsigned int m_ngoodRefits;
    mutable unsigned int m_failedErrorUpdateOutlierRemoval;
    mutable unsigned int m_failedOutlierRemoval;
    mutable unsigned int m_failedErrorUpdate;
    mutable unsigned int m_failedRefit;
    mutable unsigned int m_failedExtrapolationMuonEntry;

    mutable unsigned int m_nbackExtrapolations;
    mutable unsigned int m_ngoodBackExtrapolations;
    mutable unsigned int m_failedExtrapolationIP;

    mutable unsigned int m_nupdates;
    mutable unsigned int m_ngoodUpdates;
    mutable unsigned int m_failedUpdates;

    double m_minMuonMomentum;
    double m_fixedError;
    double m_flagT0FitRange;
    bool   m_deweightBEE;
    bool   m_deweightEE;
    bool   m_deweightBIS78;
    bool   m_deweightTwoStationTracks;
  }; 
}

#endif 
