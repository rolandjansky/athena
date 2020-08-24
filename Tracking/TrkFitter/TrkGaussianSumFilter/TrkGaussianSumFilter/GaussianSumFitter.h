/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   GaussianSumFitter.h
 * @date   Monday 7th March 2005
 * @author Tom Athkinson, Anthony Morley, Christos Anastopoulos
 * @brief  Class for fitting according to the Gaussian Sum Filter  formalism
 */

#include "TrkEventPrimitives/PropDirection.h"
#include "TrkEventUtils/TrkParametersComparisonFunction.h"
#include "TrkFitterInterfaces/ITrackFitter.h"
#include "TrkFitterUtils/FitterTypes.h"
#include "TrkFitterUtils/TrackFitInputPreparator.h"
#include "TrkGaussianSumFilter/IMultiStateExtrapolator.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkToolInterfaces/IRIO_OnTrackCreator.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/ToolHandle.h"
#include <atomic>

namespace Trk {
class IMultiStateMeasurementUpdator;
class MultiComponentStateOnSurface;
class IForwardGsfFitter;
class IGsfSmoother;
class FitQuality;
class Track;

class GaussianSumFitter
  : virtual public ITrackFitter
  , public AthAlgTool
{
public:
  /** Constructor with parameters to be passed to AlgTool */
  GaussianSumFitter(const std::string&, const std::string&, const IInterface*);

  /** Virtual destructor */
  virtual ~GaussianSumFitter() = default;

  /** AlgTool initialise method */
  virtual StatusCode initialize() override final;

  /** AlgTool finalise method */
  virtual StatusCode finalize() override final;

  using ITrackFitter::fit;

  /** Refit a track using the Gaussian Sum Filter */

  virtual std::unique_ptr<Track> fit(
    const EventContext& ctx,
    const Track&,
    const RunOutlierRemoval outlierRemoval = false,
    const ParticleHypothesis particleHypothesis =
      nonInteracting) const override final;

  /** Fit a collection of 'PrepRawData' objects using the Gaussian Sum Filter
      - This requires that an trackParameters object be supplied also as an
     initial guess */
  virtual std::unique_ptr<Track> fit(
    const EventContext& ctx,
    const PrepRawDataSet&,
    const TrackParameters&,
    const RunOutlierRemoval outlierRemoval = false,
    const ParticleHypothesis particleHypothesis =
      nonInteracting) const override final;

  /** Fit a collection of 'RIO_OnTrack' objects using the Gaussian Sum Filter
      - This requires that an trackParameters object be supplied also as an
     initial guess */
  virtual std::unique_ptr<Track> fit(
    const EventContext& ctx,
    const MeasurementSet&,
    const TrackParameters&,
    const RunOutlierRemoval outlierRemoval = false,
    const ParticleHypothesis particleHypothesis =
      nonInteracting) const override final;

  /** Refit a track adding a PrepRawDataSet - Not done! */
  virtual std::unique_ptr<Track> fit(
    const EventContext& ctx,
    const Track&,
    const PrepRawDataSet&,
    const RunOutlierRemoval runOutlier = false,
    const ParticleHypothesis matEffects = nonInteracting) const override final;

  /** Refit a track adding a RIO_OnTrack set
      - This has no form of outlier rejection and will use all hits on orginal
     track... i.e. very basic impleneation at the moment*/
  virtual std::unique_ptr<Track> fit(
    const EventContext& ctx,
    const Track&,
    const MeasurementSet&,
    const RunOutlierRemoval runOutlier = false,
    const ParticleHypothesis matEffects = nonInteracting) const override final;

  /** Combine two tracks by refitting - Not done! */
  virtual std::unique_ptr<Track> fit(
    const EventContext& ctx,
    const Track&,
    const Track&,
    const RunOutlierRemoval runOutlier = false,
    const ParticleHypothesis matEffects = nonInteracting) const override final;

private:
  /** Produces a perigee from a smoothed trajectory */
  const MultiComponentStateOnSurface* makePerigee(
    const EventContext& ctx,
    Trk::IMultiStateExtrapolator::Cache&,
    const SmoothedTrajectory*,
    const ParticleHypothesis particleHypothesis = nonInteracting) const;

  //* Calculate the fit quality */
  const Trk::FitQuality* buildFitQuality(const Trk::SmoothedTrajectory&) const;

private:
  ToolHandle<IMultiStateExtrapolator> m_extrapolator{
    this,
    "ToolForExtrapolation",
    "Trk::GsfExtrapolator/GsfExtrapolator",
    ""
  };
  ToolHandle<IRIO_OnTrackCreator> m_rioOnTrackCreator{
    this,
    "ToolForROTCreation",
    "Trk::RioOnTrackCreator/RIO_OnTrackCreator",
    ""
  };
  ToolHandle<IForwardGsfFitter> m_forwardGsfFitter{
    this,
    "ForwardGsfFitter",
    "Trk::ForwardGsfFitter/ForwardGsfFitter",
    ""
  };
  ToolHandle<IGsfSmoother> m_gsfSmoother{ this,
                                          "GsfSmoother",
                                          "Trk::GsfSmoother/GsfSmoother",
                                          "" };

  Gaudi::Property<bool> m_StoreMCSOS{
    this,
    "StoreMCSOS",
    true,
    "Store Multicomponent State (preferred if we slim later on) or Single "
    "state in final trajectory"
  };

  Gaudi::Property<bool> m_reintegrateOutliers{ this,
                                               "ReintegrateOutliers",
                                               true,
                                               "Reintegrate Outliers" };

  Gaudi::Property<bool> m_makePerigee{ this,
                                       "MakePerigee",
                                       true,
                                       "Make Perigee" };

  Gaudi::Property<bool> m_refitOnMeasurementBase{ this,
                                                  "RefitOnMeasurementBase",
                                                  true,
                                                  "Refit On Measurement Base" };

  Gaudi::Property<bool> m_doHitSorting{ this,
                                        "DoHitSorting",
                                        true,
                                        "Do Hit Sorting" };

  PropDirection m_directionToPerigee;
  std::unique_ptr<TrkParametersComparisonFunction>
    m_trkParametersComparisonFunction;
  std::unique_ptr<TrackFitInputPreparator> m_inputPreparator;
  std::vector<double> m_sortingReferencePoint;

  // Number of Fit PrepRawData Calls
  mutable std::atomic<int> m_FitPRD;
  // Number of Fit MeasurementBase Calls
  mutable std::atomic<int> m_FitMeasurementBase;
  // Number of Foward Fit Failures
  mutable std::atomic<int> m_ForwardFailure;
  // Number of Smoother Failures
  mutable std::atomic<int> m_SmootherFailure;
  // Number of MakePerigee Failures
  mutable std::atomic<int> m_PerigeeFailure;
  // Number of Tracks that fail fit Quailty test
  mutable std::atomic<int> m_fitQualityFailure;
  // Number of Tracks that are successfull
  mutable std::atomic<int> m_fitSuccess;
};

} // end Trk namespace
