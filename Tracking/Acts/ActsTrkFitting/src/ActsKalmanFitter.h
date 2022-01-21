/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ACTSGEOMETRY_ACTSKALMANFITTER_H
#define ACTSGEOMETRY_ACTSKALMANFITTER_H

#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/EventContext.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "TrkFitterInterfaces/ITrackFitter.h"

#include "TrkToolInterfaces/IExtendedTrackSummaryTool.h"
#include "TrkToolInterfaces/IBoundaryCheckTool.h"

// ACTS
#include "Acts/EventData/TrackParameters.hpp"
#include "Acts/TrackFitting/KalmanFitter.hpp"
#include "Acts/MagneticField/MagneticFieldProvider.hpp"
#include "Acts/Propagator/EigenStepper.hpp"
#include "Acts/Propagator/Propagator.hpp"

// PACKAGE
#include "ActsGeometry/ATLASSourceLink.h"
#include "ActsGeometry/ATLASMagneticFieldWrapper.h"

#include "ActsGeometryInterfaces/IActsATLASConverterTool.h"
#include "ActsGeometryInterfaces/IActsExtrapolationTool.h"
#include "ActsGeometryInterfaces/IActsTrackingGeometryTool.h"

// STL
#include <memory>
#include <vector>
#include <fstream>
#include <mutex>

class EventContext;

class ActsKalmanFitter : virtual public Trk::ITrackFitter, public AthAlgTool {
public:

  ActsKalmanFitter(const std::string&,const std::string&,const IInterface*);
  virtual ~ActsKalmanFitter();

  // standard Athena methods
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;
  /*
   * Bring in default impl with
   * EventContext for now
  */
  using Trk::ITrackFitter::fit;

  //! refit a track
  virtual std::unique_ptr<Trk::Track> fit(
    const EventContext& ctx,
    const Trk::Track&,
    const Trk::RunOutlierRemoval runOutlier = false,
    const Trk::ParticleHypothesis matEffects = Trk::nonInteracting) const override;

  //! fit a set of PrepRawData objects
  virtual std::unique_ptr<Trk::Track> fit(
    const EventContext& ctx,
    const Trk::PrepRawDataSet&,
    const Trk::TrackParameters&,
    const Trk::RunOutlierRemoval runOutlier = false,
    const Trk::ParticleHypothesis matEffects = Trk::nonInteracting) const override;

  //! fit a set of MeasurementBase objects
  virtual std::unique_ptr<Trk::Track> fit(
    const EventContext& ctx,
    const Trk::MeasurementSet&,
    const Trk::TrackParameters&,
    const Trk::RunOutlierRemoval runOutlier = false,
    const Trk::ParticleHypothesis matEffects = Trk::nonInteracting) const override;

  //! extend a track fit including a new set of PrepRawData objects
  virtual std::unique_ptr<Trk::Track> fit(
    const EventContext& ctx,
    const Trk::Track&,
    const Trk::PrepRawDataSet&,
    const Trk::RunOutlierRemoval runOutlier = false,
    const Trk::ParticleHypothesis matEffects = Trk::nonInteracting) const override;

  //! extend a track fit including a new set of MeasurementBase objects
  virtual std::unique_ptr<Trk::Track> fit(
    const EventContext& ctx,
    const Trk::Track&,
    const Trk::MeasurementSet&,
    const Trk::RunOutlierRemoval runOutlier = false,
    const Trk::ParticleHypothesis matEffects = Trk::nonInteracting) const override;

  //! combined track fit
  virtual std::unique_ptr<Trk::Track> fit(
    const EventContext& ctx,
    const Trk::Track&,
    const Trk::Track&,
    const Trk::RunOutlierRemoval runOutlier = false,
    const Trk::ParticleHypothesis matEffects = Trk::nonInteracting) const override;

  /// Outlier finder using a Chi2 cut.
  struct ATLASOutlierFinder {
    double StateChiSquaredPerNumberDoFCut = std::numeric_limits<double>::max();
    /// Classify a measurement as a valid one or an outlier.
    ///
    /// @tparam track_state_t Type of the track state
    /// @param state The track state to classify
    /// @retval False if the measurement is not an outlier
    /// @retval True if the measurement is an outlier
    bool operator()(Acts::MultiTrajectory::ConstTrackStateProxy state) const {
      // can't determine an outlier w/o a measurement or predicted parameters
      if (not state.hasCalibrated() or not state.hasPredicted()) {
        return false;
      }
      return Acts::visit_measurement(
          state.calibrated(), state.calibratedCovariance(),
          state.calibratedSize(),
          [&](const auto calibrated, const auto calibratedCovariance) {
            // Determine the mesurement size
            constexpr size_t kMeasurementSize =
                decltype(calibrated)::RowsAtCompileTime;
            // Resize the projector with the measurement size
            const auto H =
                state.projector()
                    .template topLeftCorner<kMeasurementSize, Acts::BoundIndices::eBoundSize>()
                    .eval();
            const auto residual = calibrated - H * state.predicted(); 
            double chi2 = (residual.transpose() * ((calibratedCovariance + H * state.predictedCovariance() * H.transpose())).inverse() * residual).value();
            return bool(chi2 > StateChiSquaredPerNumberDoFCut * kMeasurementSize);
          });
    }
  };

  /// Determine if the smoothing of a track should be done with or without reverse
  /// filtering
  struct ReverseFilteringLogic {
    double momentumMax = std::numeric_limits<double>::max();

    /// Determine if the smoothing of a track should be done with or without reverse
    /// filtering
    ///
    /// @tparam track_state_t Type of the track state
    /// @param trackState The trackState of the last measurement
    /// @retval False if we don't use the reverse filtering for the smoothing of the track
    /// @retval True if we use the reverse filtering for the smoothing of the track
    bool operator()(Acts::MultiTrajectory::ConstTrackStateProxy trackState) const {
      // can't determine an outlier w/o a measurement or predicted parameters
      auto momentum = fabs(1 / trackState.filtered()[Acts::eBoundQOverP]);
      return (momentum <= momentumMax);
    }
  };


  using TrackFitterResult =
      Acts::Result<Acts::KalmanFitterResult>;



  ///////////////////////////////////////////////////////////////////
  // Private methods:
  ///////////////////////////////////////////////////////////////////
private:

  static Acts::Result<void> gainMatrixUpdate(const Acts::GeometryContext& gctx,
      Acts::MultiTrajectory::TrackStateProxy trackState, Acts::NavigationDirection direction, Acts::LoggerWrapper logger);

  static Acts::Result<void> gainMatrixSmoother(const Acts::GeometryContext& gctx,
      Acts::MultiTrajectory& trajectory, size_t entryIndex, Acts::LoggerWrapper logger);

  // Create a track from the fitter result
  std::unique_ptr<Trk::Track> makeTrack(const EventContext& ctx, Acts::GeometryContext& tgContext, TrackFitterResult& fitResult) const;

  /// Create the track fitter function implementation.
  ///
  /// The magnetic field is intentionally given by-value since the variant
  /// contains shared_ptr anyways.


  ToolHandle<IActsExtrapolationTool> m_extrapolationTool{this, "ExtrapolationTool", "ActsExtrapolationTool"};
  ToolHandle<IActsTrackingGeometryTool> m_trackingGeometryTool{this, "TrackingGeometryTool", "ActsTrackingGeometryTool"};
  ToolHandle<IActsATLASConverterTool> m_ATLASConverterTool{this, "ATLASConverterTool", "ActsATLASConverterTool"};
  ToolHandle<Trk::IExtendedTrackSummaryTool> m_trkSummaryTool;
  ToolHandle<Trk::IBoundaryCheckTool> m_boundaryCheckTool {this, 
                                                           "BoundaryCheckTool", 
                                                           "InDet::InDetBoundaryCheckTool", 
                                                           "Boundary checking tool for detector sensitivities"};

    // the settable job options
    double m_option_outlierChi2Cut;
    double m_option_ReverseFilteringPt;
    int    m_option_maxPropagationStep;
    double m_option_seedCovarianceScale;



  /// Type erased track fitter function.
    using Fitter = Acts::KalmanFitter<Acts::Propagator<Acts::EigenStepper<>, Acts::Navigator>>;
    std::unique_ptr<Fitter> m_fitter;

    Acts::KalmanFitterExtensions getExtensions();

    ATLASOutlierFinder m_outlierFinder{0};
    ReverseFilteringLogic m_reverseFilteringLogic{0};
    Acts::KalmanFitterExtensions m_kfExtensions;

  /// Private access to the logger
  const Acts::Logger& logger() const {
    return *m_logger;
  }

  /// logging instance
  std::unique_ptr<const Acts::Logger> m_logger;

}; // end of namespace

#endif

