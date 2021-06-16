/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ACTSGEOMETRY_ATLASOURCELINK_H
#define ACTSGEOMETRY_ATLASOURCELINK_H


#pragma once

#include "TrkMeasurementBase/MeasurementBase.h"

#include "Acts/EventData/Measurement.hpp"
#include "Acts/Geometry/GeometryIdentifier.hpp"
#include "Acts/Surfaces/Surface.hpp"
#include "Acts/Definitions/TrackParametrization.hpp"

// add include when finished


#include <stdexcept>
#include <string>

/// Source link class for simulation in the acts-framework.
///
/// The source link stores the measuremts, surface, and the associated simulated
/// truth hit.
///
/// @todo Allow multiple truth hits e.g. for merged hits.
class ATLASSourceLink {
 public:
  ATLASSourceLink(const Acts::Surface& surface, const Trk::MeasurementBase& atlasHit,
                size_t dim, Acts::BoundVector values, Acts::BoundMatrix cov)
      : m_values(values),
        m_cov(cov),
        m_dim(dim),
        m_geometryId(surface.geometryId()),
        m_atlasHit(&atlasHit) {}
  /// Must be default_constructible to satisfy SourceLinkConcept.
  ATLASSourceLink() = default;
  ATLASSourceLink(ATLASSourceLink&&) = default;
  ATLASSourceLink(const ATLASSourceLink&) = default;
  ATLASSourceLink& operator=(ATLASSourceLink&&) = default;
  ATLASSourceLink& operator=(const ATLASSourceLink&) = default;

  Acts::BoundVector values() const { return m_values; }
  Acts::BoundMatrix cov() const { return m_cov; }
  constexpr size_t dim() const { return m_dim; }
  constexpr Acts::GeometryIdentifier geometryId() const { return m_geometryId; }
  constexpr const Trk::MeasurementBase& atlasHit() const { return *m_atlasHit; }


 private:
  Acts::BoundVector m_values;
  Acts::BoundMatrix m_cov;
  size_t m_dim = 0u;
  // store geo id copy to avoid indirection via truth hit
  Acts::GeometryIdentifier m_geometryId;
  // need to store pointers to make the object copyable
  const Trk::MeasurementBase* m_atlasHit;

  friend constexpr bool operator==(const ATLASSourceLink& lhs,
                                   const ATLASSourceLink& rhs) {
    return lhs.m_atlasHit == rhs.m_atlasHit;
  }

  friend constexpr bool operator!=(const ATLASSourceLink& lhs,
                                   const ATLASSourceLink& rhs) {
    return lhs.m_atlasHit != rhs.m_atlasHit;
  }  
};

/// A calibrator to extract the measurement from a ATLASSourceLink.
struct ATLASSourceLinkCalibrator final {
  /// Extract the measurement.
  ///
  /// @tparam track_parameters_t Type of the track parameters
  /// @param sourceLink Input source link
  /// @param parameters Input track parameters (unused)
  template <typename track_parameters_t>
  const Acts::BoundVariantMeasurement<ATLASSourceLink> operator()(
      const ATLASSourceLink& sourceLink,
      const track_parameters_t& /* parameters */) const {
    if (sourceLink.dim() == 0) {
      throw std::runtime_error("Cannot create dim 0 measurement");
    } else if (sourceLink.dim() == 1) {
      return Acts::makeMeasurement(sourceLink, sourceLink.values().head<1>(), sourceLink.cov().topLeftCorner<1, 1>(), Acts::eBoundLoc0);
    } else if (sourceLink.dim() == 2) {
      return Acts::makeMeasurement(sourceLink, sourceLink.values().head<2>(), sourceLink.cov().topLeftCorner<2, 2>(), Acts::eBoundLoc0, Acts::eBoundLoc1);
    } else {
      throw std::runtime_error("Dim " + std::to_string(sourceLink.dim()) +
                               " currently not supported.");
    }
  }
};

#endif
