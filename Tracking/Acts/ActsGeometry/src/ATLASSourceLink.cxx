/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "ActsGeometry/ATLASSourceLink.h"

ATLASSourceLink::ATLASSourceLink(const Acts::Surface& surface, const Trk::MeasurementBase& atlasHit,
                size_t dim, Acts::BoundVector values, Acts::BoundMatrix cov)
      : Acts::SourceLink{surface.geometryId()},
        m_values(values),
        m_cov(cov),
        m_dim(dim),
        m_atlasHit(&atlasHit) {}


void ATLASSourceLinkCalibrator::calibrate(const Acts::GeometryContext& /*gctx*/,
                   Acts::MultiTrajectory::TrackStateProxy trackState) {
  const auto& sourceLink = static_cast<const ATLASSourceLink&>(trackState.uncalibrated());
  if (sourceLink.dim() == 0) {
    throw std::runtime_error("Cannot create dim 0 measurement");
  } else if (sourceLink.dim() == 1) {
    // return Acts::makeMeasurement(sourceLink, sourceLink.values().head<1>(), sourceLink.cov().topLeftCorner<1, 1>(), Acts::eBoundLoc0);
    trackState.calibrated().head<1>() = sourceLink.values().head<1>();
    trackState.calibratedCovariance().topLeftCorner<1, 1>() = sourceLink.cov().topLeftCorner<1, 1>();
    trackState.data().measdim = sourceLink.dim();
  } else if (sourceLink.dim() == 2) {
    // return Acts::makeMeasurement(sourceLink, sourceLink.values().head<2>(), sourceLink.cov().topLeftCorner<2, 2>(), Acts::eBoundLoc0, Acts::eBoundLoc1);
    trackState.calibrated().head<2>() = sourceLink.values().head<2>();
    trackState.calibratedCovariance().topLeftCorner<2, 2>() = sourceLink.cov().topLeftCorner<2, 2>();
    trackState.data().measdim = sourceLink.dim();
  } else {
    throw std::runtime_error("Dim " + std::to_string(sourceLink.dim()) +
                             " currently not supported.");
  }
}
