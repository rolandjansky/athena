/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ACTSGEOMETRY_ACTSDETECTORELEMENT_H
#define ACTSGEOMETRY_ACTSDETECTORELEMENT_H

// Amg Eigen plugin includes
#include "EventPrimitives/EventPrimitives.h"
#include "GeoPrimitives/GeoPrimitives.h"

// ATHENA INCLUDES

#include "InDetIdentifier/PixelID.h"
#include "InDetIdentifier/SCT_ID.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "TRT_ReadoutGeometry/TRT_BaseElement.h"
#include "TrkSurfaces/AnnulusBounds.h"

// ACTS
#include "Acts/Geometry/DetectorElementBase.hpp"
#include "Acts/Geometry/GeometryContext.hpp"

// STL
#include <iostream>
#include <mutex>

namespace Acts {
class SurfaceBounds;
}

class ActsTrackingGeometrySvc;
class ActsAlignmentStore;

class IdentityHelper;

/// @class ActsDetectorElement
///
class ActsDetectorElement : public Acts::DetectorElementBase {
public:
  ActsDetectorElement(const InDetDD::SiDetectorElement &detElem);

  /// Constructor for a straw surface.
  /// @param transform Transform to the straw system
  ActsDetectorElement(
      const Acts::Transform3 &trf, const InDetDD::TRT_BaseElement &detElem,
      const Identifier &id // we need explicit ID here b/c of straws
  );

  ///  Destructor
  virtual ~ActsDetectorElement() {}

  /// Identifier
  Identifier identify() const;

  /// Return local to global transform associated with this identifier

  void storeTransform(ActsAlignmentStore *gas) const;
  virtual const Acts::Transform3 &
  transform(const Acts::GeometryContext &gctx) const final override;

  /// Return surface associated with this identifier, which should come from the
  virtual const Acts::Surface &surface() const final override;

  /// Return a shared pointer on the ATLAS surface associated with this
  /// identifier,
  const Trk::Surface &atlasSurface() const;

  /// Returns the thickness of the module
  virtual double thickness() const final override;

  IdentityHelper identityHelper() const;

  /// Returns default transform. For TRT this is static and set in constructor.
  /// For silicon detectors it is calulated from GM, and stored. Thus the method
  /// is not const. The store is mutexed.
  const Acts::Transform3 &getDefaultTransformMutexed() const;

  /// Returns the underllying GeoModel detectorelement that this one
  /// is based on.
  const GeoVDetectorElement *upstreamDetectorElement() const;

private:
  /// Detector element as variant
  const GeoVDetectorElement *m_detElement;
  /// Boundaries of the detector element
  std::shared_ptr<const Acts::SurfaceBounds> m_bounds;
  ///  Thickness of this detector element
  double m_thickness;
  /// Corresponding Surface
  std::shared_ptr<const Acts::Surface> m_surface;
  std::vector<std::shared_ptr<const Acts::Surface>> m_surfaces;

  // this is pretty much only used single threaded, so
  // the mutex does not hurt
  mutable std::mutex m_cacheMutex;
  mutable std::optional<Acts::Transform3> m_defTransform;

  Acts::Transform3 m_extraTransform{Acts::Transform3::Identity()};

  Identifier m_explicitIdentifier;
};

#endif
