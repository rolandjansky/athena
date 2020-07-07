/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// KalmanFitter.h
//   Header file for class KalmanFitter
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Markus.Elsing@cern.ch
///////////////////////////////////////////////////////////////////

#ifndef TRK_KALMANFITTER_H
#define TRK_KALMANFITTER_H

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/EventContext.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "TrkEventUtils/TrkParametersComparisonFunction.h"
#include "TrkFitterUtils/ProtoTrackStateOnSurface.h"
#include "TrkFitterUtils/FitterTypes.h"
#include "TrkFitterUtils/FitterStatusCode.h"
#include "TrkFitterInterfaces/ITrackFitter.h"

// Kalman Fitter's internal modules
#include "TrkFitterInterfaces/IForwardKalmanFitter.h"
#include "TrkFitterInterfaces/IKalmanSmoother.h"
#include "TrkFitterInterfaces/IKalmanOutlierLogic.h"
// Kalman Fitter's external modules
#include "TrkFitterInterfaces/IDynamicNoiseAdjustor.h"
#include "TrkFitterInterfaces/ITrackBreakpointAnalyser.h"
#include "TrkFitterInterfaces/IKalmanPiecewiseAnnealingFilter.h"

#include "GeoPrimitives/GeoPrimitives.h"
#include <array>
#include <memory>
class AtlasDetectorID;            //!< to identify measurements

namespace Trk {
	
  class Track;
  class TrackStateOnSurface;
  class TrackInfo;
  class PerigeeSurface;
  class FitQuality;

  class IValidationNtupleTool;   // validation tool to write intermediate results to ntuple
  class IAlignableSurfaceProvider; // tool to enable Kalman-alignment
    
  class IExtrapolator;           // Extrapolation Tool
  class IUpdator;                // If class for Estimator updating
  class IRIO_OnTrackCreator;     // Interface for creation of ROT
  class ProtoTrajectoryUtility;  // helper to analyse current trajectory
  class IMeasurementRecalibrator;// steering if and which ROTs to re-do
  class TrackFitInputPreparator; // helper to fill internal trajectories
  typedef std::vector<Trk::ProtoTrackStateOnSurface> Trajectory;
  typedef DataVector<const TrackStateOnSurface>::const_iterator TS_iterator;
				
  /** @brief Main Fitter tool providing the implementation for the different
      fitting, extension and refitting use cases.

      It manages and calls
      the other tools in this package, such as ForwardFitter, Smoother 
      and OutlierLogic.
      @author M. Elsing, W. Liebig
   */
  class KalmanFitter : virtual public ITrackFitter, public AthAlgTool {
  public:
	// standard AlgTool methods
    KalmanFitter(const std::string&,const std::string&,const IInterface*);
    virtual ~KalmanFitter();

    // standard Athena methods
    virtual StatusCode initialize() override;
    virtual StatusCode finalize() override;
    /*
     * Bring in default impl with
     * EventContext for now
     */
    using ITrackFitter::fit;

    //! refit a track
    virtual std::unique_ptr<Track> fit(
      const EventContext& ctx,
      const Track&,
      const RunOutlierRemoval runOutlier = false,
      const ParticleHypothesis matEffects = Trk::nonInteracting) const override;

    //! fit a set of PrepRawData objects
    virtual std::unique_ptr<Track> fit(
      const EventContext& ctx,
      const PrepRawDataSet&,
      const TrackParameters&,
      const RunOutlierRemoval runOutlier = false,
      const ParticleHypothesis matEffects = Trk::nonInteracting) const override;

    //! fit a set of MeasurementBase objects
    virtual std::unique_ptr<Track> fit(
      const EventContext& ctx,
      const MeasurementSet&,
      const TrackParameters&,
      const RunOutlierRemoval runOutlier = false,
      const ParticleHypothesis matEffects = Trk::nonInteracting) const override;

    //! extend a track fit including a new set of PrepRawData objects
    virtual std::unique_ptr<Track> fit(
      const EventContext& ctx,
      const Track&,
      const PrepRawDataSet&,
      const RunOutlierRemoval runOutlier = false,
      const ParticleHypothesis matEffects = Trk::nonInteracting) const override;

    //! extend a track fit including a new set of MeasurementBase objects
    virtual std::unique_ptr<Track> fit(
      const EventContext& ctx,
      const Track&,
      const MeasurementSet&,
      const RunOutlierRemoval runOutlier = false,
      const ParticleHypothesis matEffects = Trk::nonInteracting) const override;

    //! combined track fit
    virtual std::unique_ptr<Track> fit(
      const EventContext& ctx,
      const Track&,
      const Track&,
      const RunOutlierRemoval runOutlier = false,
      const ParticleHypothesis matEffects = Trk::nonInteracting) const override;

    /** @brief retrieve statuscode of last fit.

        particularly designed for accessing a more detailed reason for
        failed fits, which otherwise are indicated only by the returned
        NULL pointer. */
    virtual Trk::FitterStatusCode statusCodeOfLastFit() const override;

  ///////////////////////////////////////////////////////////////////
  // Private methods:
  ///////////////////////////////////////////////////////////////////
private:
    //! method providing common code to validate fitter input
    bool                       check_operability(int, const RunOutlierRemoval&, 
                                                 const Trk::ParticleHypothesis&, bool
                                                 ) const;

    //! method providing the filter code common to all interfaces
    bool iterateKalmanFilter(const EventContext& ctx,
                             const Trk::TrackParameters*&,
                             FitQuality*&,
                             const RunOutlierRemoval,
                             const Trk::KalmanMatEffectsController&,
                             const double& this_eta = 0.0) const;

    //! method providing a 2nd concept for iterating, by internal annealing
    bool                       invokeAnnealingFilter(const Trk::TrackParameters*&,
                                                     FitQuality*&,
                                                     const RunOutlierRemoval,
                                                     const Trk::KalmanMatEffectsController&,
                                                     const double& this_eta=0.0) const;

    bool                       prepareNextIteration(const unsigned int& upcomingIteration,
                                                    FitQuality*&,
                                                    int& firststate,
                                                    const TrackParameters&) const;

    //! method to create a track which is common to all interfaces
    Trk::Track* makeTrack(const EventContext& ctx,
                          const Trk::FitQuality*,
                          const Trk::TrackParameters&,
                          const Trk::KalmanMatEffectsController*,
                          const double&,
                          const Trk::TrackInfo*) const;

    //! special method to build Perigee parameters from the track
    const TrackStateOnSurface* makePerigee(const SmoothedTrajectory*,
                                           const PerigeeSurface&,
                                           const ParticleHypothesis matEffects=Trk::nonInteracting) const;

    //! special method to build Perigee parameters from the internal KF trajectory
    const TrackStateOnSurface* internallyMakePerigee(
      const EventContext& ctx,
      const PerigeeSurface&,
      const ParticleHypothesis) const;

    //! special method to build reference parameters at other surface than PerigeeSf
    const TrackStateOnSurface* makeReferenceState(
      const EventContext& ctx,
      const Surface&,
      const ParticleHypothesis) const;

    //! call a validation tool from the TrkValidation package
    void callValidation(
      const EventContext& ctx,
      int iterationIndex,
      const Trk::ParticleHypothesis matEffects,
      FitterStatusCode fitStatCode = Trk::FitterStatusCode::Success) const;
    ///////////////////////////////////////////////////////////////////
    // Private data:
    ///////////////////////////////////////////////////////////////////
    mutable MsgStream             m_log;         //!< msgstream as private member (-> speed)

    //! extrapolation tool: does propagation and applies material effects
    ToolHandle< IExtrapolator >             m_extrapolator;
    //! measurement updator: implements the Kalman filtering formulae
    ToolHandle< IUpdator >                  m_updator;
    //! Trk::RIO_OnTrack creation: re-calibrates hits (special interface/option only)
    ToolHandle< IRIO_OnTrackCreator >       m_ROTcreator;
    //! dynamic noise adjustment tool: adds momentum noise for electron brem
    ToolHandle< IDynamicNoiseAdjustor >     m_dynamicNoiseAdjustor;
    //! dynamic noise adjustment tool: confirm brem breakpoint or not
    ToolHandle< ITrackBreakpointAnalyser >  m_brempointAnalyser;
    //! tool to extend KF to be Kalman-Alignment-Fitter
    ToolHandle< IAlignableSurfaceProvider > m_alignableSfProvider;
    //! tool to remake ROTs if configured
    ToolHandle< IMeasurementRecalibrator >  m_recalibrator;
    //! tool to do L/R driftcircle solving
    ToolHandle< IKalmanPiecewiseAnnealingFilter > m_internalDAF;
    //! forward filtering tool: code structuring
    ToolHandle< IForwardKalmanFitter >      m_forwardFitter;
    //! backward filtering and smoothing tool: code structuring
    ToolHandle< IKalmanSmoother >           m_smoother;
    //! outlier logic tool: track quality and cleaning
    ToolHandle< IKalmanOutlierLogic >       m_outlierLogic;
    //! outlier logic tool: outlier recovery and tuning
    ToolHandle< IKalmanOutlierLogic >       m_outlierRecovery;
    //! performance/error validation tool
    ToolHandle< IValidationNtupleTool >     m_FitterValidationTool;

    // the settable job options
    bool                          m_option_enforceSorting;
    bool                          m_option_doSmoothing;
    int                           m_option_max_N_iterations;
    bool                          m_option_PerigeeAtOrigin;
    bool                          m_option_reintegrateOutliers;
    bool                          m_option_doValidationAction;
    bool                          m_option_callValidationToolForFailedFitsOnly;
    std::vector<double>           m_option_sortingRefPoint;

    mutable bool                  m_callValidationTool;
    mutable bool                  m_haveValidationTool;
    bool                          m_doDNAForElectronsOnly;
// the private vectors and helpers
    mutable Trajectory            m_trajectory;
    TrkParametersComparisonFunction* m_tparScaleSetter;
    Amg::Vector3D                 m_sortingRefPoint;
    const AtlasDetectorID*        m_idHelper;
    ProtoTrajectoryUtility*       m_utility;
    TrackFitInputPreparator*      m_inputPreparator;
    std::vector<double>           m_cov0;

    /** variables allowing to monitor the fit problems.
        joins info for all interfaces (PRD -- MB -- TRACK).*/
    mutable FitterStatusCode      m_fitStatus; //!< member to publish fitStatus later
    mutable int                   m_maximalNdof;

    /** fitter-internal statistics and validation */
    enum FitStatisticsCode {Call, Success, InternalDafUsed, DafNoImprovement, DNABremFit,DNAFoundBrem,
			     StraightTrackModel, UpdateFailure, ExtrapolationFailure, IterationsNoConvergence,
			     MinimalTrackFailure, PerigeeMakingFailure, BadInput, nFitStatsCodes};
    mutable std::vector< std::vector<int> > m_fitStatistics;
    enum StatIndex {iAll = 0, iBarrel = 1, iTransi = 2, iEndcap = 3, nStatIndex = 4};
    mutable std::array<double, nStatIndex> m_chiSquaredAfb, m_chiSquaredAfbNontriviality;

    //! methods to do bookkeeping about fitter calls, error situations and chiSquared
    void monitorTrackFits(FitStatisticsCode, const double&) const;
    void updateChi2Asymmetry(std::vector<int>&, const Trk::FitQuality&, const double&) const;
    
};

inline  Trk::FitterStatusCode Trk::KalmanFitter::statusCodeOfLastFit() const
  { return m_fitStatus; }

} // end of namespace



#endif // TRK_KALMANFITTER_H
