/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

///////////////////////////////////////////////////////////////////
// InDetExtensionProcessor.h, (c) ATLAS Detector Softwareop
///////////////////////////////////////////////////////////////////

#ifndef INDETEXTENSIONPROCESSOR_H
#define INDETEXTENSIONPROCESSOR_H

// Base class

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "CxxUtils/checker_macros.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkTrack/TrackExtensionMap.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"
#include "TrkFitterInterfaces/ITrackFitter.h"
#include "TrkToolInterfaces/ITrackScoringTool.h"
#include "TrkToolInterfaces/IExtendedTrackSummaryTool.h"

#include "GaudiKernel/ToolHandle.h"

#include <vector>
#include <array>
#include <mutex>

namespace InDet {
  /** @brief Algorithm to process a TrackExtensionMap, refit the
      extended tracks and decide if the old track is to be
      replaced by the extended one.

      @author Markus.Elsing@cern.ch
   */

  class InDetExtensionProcessor: public AthReentrantAlgorithm  {
  public:
    //! Default Algorithm constructor with parameters
    InDetExtensionProcessor(const std::string& name, ISvcLocator* pSvcLocator);

    virtual StatusCode initialize();
    virtual StatusCode execute(const EventContext& ctx) const;
    virtual StatusCode finalize();
  private:
    MsgStream &dumpStat(MsgStream &out) const;

    //! process events
    TrackCollection* createExtendedTracks(const EventContext& ctx,
                                          const TrackCollection* tracks_in,
                                          const TrackExtensionMap* track_extension_map) const;

    //! internal structuring: creates new track with original one plus extension as outliers
    Trk::Track* trackPlusExtension(const EventContext& ctx,
                                   const Trk::Track*,
                                   const std::vector<const Trk::MeasurementBase*>&) const;

    //
    // --- job options
    //
    SG::ReadHandleKey<TrackCollection>    m_trackName;        //!< Name of input Trackcollection
    bool m_cosmics;          //!< whether we are running cosmics
    SG::ReadHandleKey<TrackExtensionMap>  m_extensionMapName; //!< Name of input extension map
    SG::WriteHandleKey<TrackCollection>   m_newTrackName;     //!< Name of output Trackcollection
    ToolHandle<Trk::ITrackFitter>         m_trackFitter;      //!< track fitter tool handle

    ToolHandle<Trk::IExtendedTrackSummaryTool> m_trackSummaryTool
    //      {this, "TrackSummaryTool", "InDetTrackSummaryTool"};
    {
      this, "TrackSummaryTool", ""
    };

    ToolHandle<Trk::ITrackScoringTool>    m_scoringTool;      //!< track scorer tool handle
    Trk::RunOutlierRemoval m_runOutlier;               //!< whether to run outlier logics
    //! switch whether to append a rejected extension as outlier trajectory
    bool m_keepFailedExtensionOnTrack;
    //! fitter steering: whether to do the fit with re-calibrated clusters (true) or not
    bool m_refitPrds;
    //! type of material interaction in extrapolation
    int m_matEffects;
    bool m_suppressHoleSearch;  //!< no hole search ?
    bool m_tryBremFit;          //!< brem recovery mode ?
    bool m_caloSeededBrem;      //!< run in Calo seeded mode
    float m_pTminBrem;           //!< min pt for a brem fit

    // -- algorithm members
    Trk::ParticleHypothesis m_particleHypothesis; //!< nomen est omen

    //! internal monitoring: categories for counting different types of extension results
    enum StatIndex {
      iAll, iBarrel, iTransi, iEndcap, Nregions
    };
    typedef std::array<int, Nregions> CounterArray_t;
    // -- statistics protected by mutex
    mutable std::mutex m_statMutex;
    mutable int m_Nevents ATLAS_THREAD_SAFE;
    mutable CounterArray_t m_Ninput ATLAS_THREAD_SAFE;
    mutable CounterArray_t m_Nrecognised ATLAS_THREAD_SAFE;
    mutable CounterArray_t m_Nextended ATLAS_THREAD_SAFE;
    mutable CounterArray_t m_Nrejected ATLAS_THREAD_SAFE;
    mutable CounterArray_t m_Nfailed ATLAS_THREAD_SAFE;
    mutable CounterArray_t m_NrecoveryBremFits ATLAS_THREAD_SAFE;
    mutable CounterArray_t m_NbremFits ATLAS_THREAD_SAFE;
    mutable CounterArray_t m_Nfits ATLAS_THREAD_SAFE;
    mutable CounterArray_t m_NnotExtended ATLAS_THREAD_SAFE;
    mutable CounterArray_t m_NlowScoreBremFits ATLAS_THREAD_SAFE;
    mutable CounterArray_t m_NextendedBrem ATLAS_THREAD_SAFE;


    std::vector<float>  m_etabounds;           //!< eta intervals for internal monitoring

    //! monitoring and validation: does success/failure counting for each detector region
    void incrementRegionCounter(std::array<int, 4>&, const Trk::Track*, bool = true) const;
  };
}

#endif //INDETEXTENSIONPROCESSOR_H
