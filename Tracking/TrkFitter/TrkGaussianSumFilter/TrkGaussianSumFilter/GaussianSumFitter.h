/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/* *******************************************************************************
      GaussianSumFitter.h  -  description
      -----------------------------------
begin                : Monday 7th March 2005
author               : amorley, atkinson
email                : Anthony.Morley@cern.ch, Tom.Atkinson@cern.ch
decription           : Class for fitting according to the Gaussian Sum Filter
                       formalisation.
**********************************************************************************
*/

#include "TrkEventPrimitives/PropDirection.h"
#include "TrkEventUtils/TrkParametersComparisonFunction.h"
#include "TrkFitterInterfaces/ITrackFitter.h"
#include "TrkFitterUtils/FitterTypes.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkFitterUtils/TrackFitInputPreparator.h"
#include "TrkToolInterfaces/IRIO_OnTrackCreator.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/IChronoStatSvc.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"


#include <atomic>


namespace Trk {
class IMultiStateMeasurementUpdator;
class MultiComponentStateOnSurface;
class IMultiStateExtrapolator;
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

  virtual Track* fit(const Track&,
                     const RunOutlierRemoval outlierRemoval = false,
                     const ParticleHypothesis particleHypothesis =
                       nonInteracting) const override final;

  /** Fit a collection of 'PrepRawData' objects using the Gaussian Sum Filter
      - This requires that an trackParameters object be supplied also as an
     initial guess */
  virtual Track* fit(const PrepRawDataSet&,
                     const TrackParameters&,
                     const RunOutlierRemoval outlierRemoval = false,
                     const ParticleHypothesis particleHypothesis =
                       nonInteracting) const override final;

  /** Fit a collection of 'RIO_OnTrack' objects using the Gaussian Sum Filter
      - This requires that an trackParameters object be supplied also as an
     initial guess */
  virtual Track* fit(const MeasurementSet&,
                     const TrackParameters&,
                     const RunOutlierRemoval outlierRemoval = false,
                     const ParticleHypothesis particleHypothesis =
                       nonInteracting) const override final;

  /** Refit a track adding a PrepRawDataSet - Not done! */
  virtual Track* fit(
    const Track&,
    const PrepRawDataSet&,
    const RunOutlierRemoval runOutlier = false,
    const ParticleHypothesis matEffects = nonInteracting) const override final;

  /** Refit a track adding a RIO_OnTrack set
      - This has no form of outlier rejection and will use all hits on orginal
     track... i.e. very basic impleneation at the moment*/
  virtual Track* fit(
    const Track&,
    const MeasurementSet&,
    const RunOutlierRemoval runOutlier = false,
    const ParticleHypothesis matEffects = nonInteracting) const override final;

  /** Combine two tracks by refitting - Not done! */
  virtual Track* fit(
    const Track&,
    const Track&,
    const RunOutlierRemoval runOutlier = false,
    const ParticleHypothesis matEffects = nonInteracting) const override final;

private:
  /** Produces a perigee from a smoothed trajectory */
  const MultiComponentStateOnSurface* makePerigee(
    const EventContext& ctx,
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
  ToolHandle<IMultiStateMeasurementUpdator> m_updator{
    this,
    "MeasurementUpdatorType",
    "Trk::GsfMeasurementUpdator/GsfMeasurementUpdator",
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

  bool m_reintegrateOutliers;
  bool m_makePerigee;
  bool m_refitOnMeasurementBase;
  bool m_doHitSorting;
  PropDirection m_directionToPerigee;
  std::unique_ptr<TrkParametersComparisonFunction> m_trkParametersComparisonFunction;
  std::unique_ptr<TrackFitInputPreparator> m_inputPreparator;
  std::vector<double> m_sortingReferencePoint;
  ServiceHandle<IChronoStatSvc> m_chronoSvc;

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
};

} // end Trk namespace
