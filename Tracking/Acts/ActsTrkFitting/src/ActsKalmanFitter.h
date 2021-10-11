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
    template <typename track_state_t>
    bool operator()(const track_state_t& state) const {
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

  /// Track fitter function that takes input measurements, initial trackstate
  //  and fitter options and returns some track-fitter-specific result.
  using TrackFitterOptions = Acts::KalmanFitterOptions<ATLASSourceLinkCalibrator,
                                                       ATLASOutlierFinder>;
  using TrackFitterResult =
      Acts::Result<Acts::KalmanFitterResult<ATLASSourceLink>>;

  using TrackFitterFunction = std::function<TrackFitterResult(
      const std::vector<ATLASSourceLink>&, const Acts::BoundTrackParameters&,
      const TrackFitterOptions&)>;


  ///////////////////////////////////////////////////////////////////
  // Private methods:
  ///////////////////////////////////////////////////////////////////
private:

  // Create a track from the fitter result
  std::unique_ptr<Trk::Track> makeTrack(const EventContext& ctx, Acts::GeometryContext& tgContext, TrackFitterResult& fitResult) const;

  /// Create the track fitter function implementation.
  ///
  /// The magnetic field is intentionally given by-value since the variant
  /// contains shared_ptr anyways.
  static TrackFitterFunction makeTrackFitterFunction(
      std::shared_ptr<const Acts::TrackingGeometry> trackingGeometry);


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
    int    m_option_maxPropagationStep;
    double m_option_seedCovarianceScale;



  /// Type erased track fitter function.
  TrackFitterFunction m_fit;

  /// Private access to the logger
  const Acts::Logger& logger() const {
    return *m_logger;
  }

  /// logging instance
  std::unique_ptr<const Acts::Logger> m_logger;

}; // end of namespace

#endif

