/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ACTSGEOMETRY_ACTSDETECTORELEMENT_H
#define ACTSGEOMETRY_ACTSDETECTORELEMENT_H

// ATHENA INCLUDES
#include "InDetIdentifier/PixelID.h"
#include "InDetIdentifier/SCT_ID.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "TRT_ReadoutGeometry/TRT_BaseElement.h"

// ACTS
#include "Acts/Geometry/DetectorElementBase.hpp"
#include "Acts/Geometry/GeometryContext.hpp"

// STL
#include <mutex>
#include <iostream>

// BOOST
#include <boost/variant.hpp>

namespace Acts {
  class SurfaceBounds;
}

class ActsTrackingGeometrySvc;
class ActsAlignmentStore;

class IdentityHelper;

/// @class ActsDetectorElement
///
class ActsDetectorElement : public Acts::DetectorElementBase
{
  using DetElemVariant = boost::variant<const InDetDD::SiDetectorElement*, const InDetDD::TRT_BaseElement*>;
public:
  enum class Subdetector { Pixel, SCT, TRT };

  ActsDetectorElement(const InDetDD::SiDetectorElement* detElem);

  /// Constructor for a straw surface.
  /// @param transform Transform to the straw system
  ActsDetectorElement(const Acts::Transform3D& trf,
                      const InDetDD::TRT_BaseElement* detElem,
                      const Identifier& id // we need explicit ID here b/c of straws
                      );

  ///  Destructor
  virtual ~ActsDetectorElement() {}

  /// Identifier
  Identifier
  identify() const;

  /// Return local to global transform associated with this identifier
  
  void
  storeTransform(ActsAlignmentStore* gas) const;
  virtual const Acts::Transform3D &
  transform(const Acts::GeometryContext &gctx) const final override;


  /// Return surface associated with this identifier, which should come from the
  virtual const Acts::Surface&
  surface() const final override;

  /// Returns the thickness of the module
  virtual double
  thickness() const final override;

  // optimize this, it doesn't ever change so can be saved on construction
  Subdetector getSubdetector() const {
    if(m_detElement.which() == 1) {
      return Subdetector::TRT;
    }

    // this should be Pixel or SCT
    const auto& de = *boost::get<const InDetDD::SiDetectorElement*>(m_detElement);
    if(de.isPixel()) {
      return Subdetector::Pixel;
    }
    else {
      return Subdetector::SCT;
    }
  }

  IdentityHelper identityHelper() const;


private:
  
  /// Returns default transform. For TRT this is static and set in constructor.
  /// For silicon detectors it is calulated from GM, and stored. Thus the method 
  /// is not const. The store is mutexed.
  const Acts::Transform3D&
  getDefaultTransformMutexed() const;

  struct IdVisitor : public boost::static_visitor<Identifier>
  {
    explicit IdVisitor(const Identifier& id) : m_explicitIdentifier(id) {}

    Identifier operator()(const InDetDD::SiDetectorElement* detElem) const
    {
      return detElem->identify(); // easy, det element has identifier
    }

    Identifier operator()(const InDetDD::TRT_BaseElement*) const
    {
      // we got the identifier in constructrion, because it identifies
      // the STRAW
      return m_explicitIdentifier;
    }

    Identifier m_explicitIdentifier;
  };

  /// Detector element as variant
  DetElemVariant m_detElement;
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
  mutable std::shared_ptr<const Acts::Transform3D> m_defTransform;

  Identifier m_explicitIdentifier;

  // this is threadsafe!
  //mutable Gaudi::Hive::ContextSpecificData<Acts::Transform3D> m_ctxSpecificTransform;


};

#endif
