// This file is part of the ACTS project.
//
// Copyright (C) 2016 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.


#ifndef ACTS_GEOMODELPLUGIN_GEOMODELDETECTORELEMENT_H
#define ACTS_GEOMODELPLUGIN_GEOMODELDETECTORELEMENT_H

#include <iostream>

#include <boost/variant.hpp>

// ATHENA INCLUDES
#include "InDetIdentifier/PixelID.h"
#include "InDetIdentifier/SCT_ID.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetReadoutGeometry/TRT_BaseElement.h"
#include "TrkSurfaces/RectangleBounds.h"
#include "TrkSurfaces/SurfaceBounds.h"
#include "TrkSurfaces/TrapezoidBounds.h"
#include "InDetReadoutGeometry/SiDetectorDesign.h"

#define ACTS_CORE_IDENTIFIER_PLUGIN "Identifier/Identifier.h"
// ACTS
#include "Acts/Detector/DetectorElementBase.hpp"
#include "Acts/Surfaces/PlaneSurface.hpp"
#include "Acts/Surfaces/RectangleBounds.hpp"
#include "Acts/Surfaces/TrapezoidBounds.hpp"

#include "GaudiKernel/ContextSpecificPtr.h"
#include "GeoModelUtilities/GeoAlignmentStore.h"

#include <mutex>



namespace Acts {

class IdentityHelper;

class DigitizationModule;

class TrackingGeometrySvc;

/// @class GeoModelDetectorElement
///
class GeoModelDetectorElement : public DetectorElementBase
{
  using DetElemVariant = boost::variant<const InDetDD::SiDetectorElement*, const InDetDD::TRT_BaseElement*>;
public:
  enum class Subdetector { Pixel, SCT, TRT };

  GeoModelDetectorElement(const InDetDD::SiDetectorElement* detElem,
                          const TrackingGeometrySvc* trkSvc);

  /// Constructor for a straw surface.
  /// @param transform Transform to the straw system
  GeoModelDetectorElement(std::shared_ptr<const Transform3D> trf, 
                          const InDetDD::TRT_BaseElement* detElem,
                          const Identifier& id, // we need explicit ID here b/c of straws
                          const TrackingGeometrySvc* trkSvc);

  ///  Destructor
  virtual ~GeoModelDetectorElement() {}

  /// Identifier
  virtual Identifier
  identify() const final override;

  /// Return local to global transform associated with this identifier
  virtual const Transform3D&
  transform(const Identifier& identifier = Identifier()) const final override;

  /// Set the identifier after construction (sometimes needed)
  virtual void
  assignIdentifier(const Identifier& identifier) final override;

  /// Return surface associated with this identifier, which should come from the
  virtual const Surface&
  surface(const Identifier& identifier = Identifier()) const final override;

  /// Returns the full list of all detection surfaces associated
  /// to this detector element
  virtual const std::vector<std::shared_ptr<const Surface>>&
  surfaces() const final override;

  /// Return the DigitizationModule
  /// @return optionally the DigitizationModule
  virtual std::shared_ptr<const DigitizationModule>
  digitizationModule() const override
  {
    return DetectorElementBase::digitizationModule();
  }

  /// Returns the thickness of the module
  virtual double
  thickness() const final override;

  IdentityHelper identityHelper() const;


private:

  struct GetTransformVisitor : public boost::static_visitor<Transform3D> {
    explicit GetTransformVisitor(GeoAlignmentStore* store, bool def, const Transform3D* fallback)
      : m_store(store), m_default(def), m_fallback(fallback)
    {}

    Transform3D operator()(const InDetDD::SiDetectorElement* detElem) const
    {
      HepGeom::Transform3D g2l;
      if (m_default) g2l = detElem->getMaterialGeom()->getDefAbsoluteTransform(m_store);
      else g2l = detElem->getMaterialGeom()->getAbsoluteTransform(m_store);

      return Amg::CLHEPTransformToEigen(g2l * detElem->recoToHitTransform());
    }

    Transform3D operator()(const InDetDD::TRT_BaseElement*) const
    {
      return *m_fallback;
    }

    GeoAlignmentStore* m_store;
    bool m_default;
    const Transform3D* m_fallback;
  };
  
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
  std::shared_ptr<const SurfaceBounds> m_bounds;
  ///  Thickness of this detector element
  double m_thickness;
  /// Corresponding Surface
  std::shared_ptr<const Surface> m_surface;
  std::vector<std::shared_ptr<const Surface>> m_surfaces;

  // this is pretty much only used single threaded, so
  // the mutex does not hurt
  mutable std::mutex m_cacheMutex;
  mutable std::shared_ptr<const Transform3D> m_defTransform;

  const TrackingGeometrySvc* m_trackingGeometrySvc;
  
  Identifier m_explicitIdentifier;

  // this is threadsafe!
  //mutable Gaudi::Hive::ContextSpecificData<Transform3D> m_ctxSpecificTransform;


};

// this is entirely useless
inline const std::vector<std::shared_ptr<const Surface>>&
GeoModelDetectorElement::surfaces() const
{
  return (m_surfaces);
}

}

#endif
