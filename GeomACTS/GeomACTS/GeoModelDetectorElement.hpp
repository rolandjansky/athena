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
// ATHENA INCLUDES
#include "InDetIdentifier/PixelID.h"
#include "InDetIdentifier/SCT_ID.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "TrkSurfaces/RectangleBounds.h"
#include "TrkSurfaces/SurfaceBounds.h"
#include "TrkSurfaces/TrapezoidBounds.h"
#include "InDetReadoutGeometry/SiDetectorDesign.h"

#define ACTS_CORE_IDENTIFIER_PLUGIN "Identifier/Identifier.h"
// ACTS
#include "ACTS/Detector/DetectorElementBase.hpp"
#include "ACTS/Surfaces/PlaneSurface.hpp"
#include "ACTS/Surfaces/RectangleBounds.hpp"
#include "ACTS/Surfaces/TrapezoidBounds.hpp"


#define DUMP(v) #v << "=" << v
#define PRV(v)                                                                 \
  #v << "(x="                                                                  \
     << v.x() << ", y="                                                        \
              << v.y() << ", z="                                               \
                       << v.z() << " r="                                       \
                                << v.perp() << ", phi=" << v.phi() << ")"

namespace Acts {

class DigitizationModule;

/// @class GeoModelDetectorElement
///
/// DetectorElement plugin for ROOT TGeo shapes. Added possibility to hand over
/// transformation matrix.
///
/// @todo what if shape conversion failes? add implementation of more than one
/// surface per module, implementing also for other shapes->Cone,ConeSeg,Tube?
/// what
/// if not used with DD4hep?
///
class GeoModelDetectorElement : public DetectorElementBase
{
public:
  enum class Subdetector { Pixel, SCT };

  GeoModelDetectorElement(const InDetDD::SiDetectorElement* detElem);

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

  int
  bec() const
  {
    return m_bec;
  }
  GeoModelDetectorElement::Subdetector
  det() const
  {
    return m_det;
  }
  int
  layer_disk() const
  {
    return m_layer_disk;
  }
  int
  phi_module() const
  {
    return m_phi_module;
  }
  int
  eta_module() const
  {
    return m_eta_module;
  }
  int
  side() const
  {
    return m_side;
  }

  int
  layer_disk_max() const
  {
    return m_layer_disk_max;
  }
  int
  phi_module_max() const
  {
    return m_phi_module_max;
  }
  int
  eta_module_max() const
  {
    return m_eta_module_max;
  }
  int
  eta_module_min() const
  {
    return m_eta_module_min;
  }

private:
  /// DD4hep detector element, just linked - not owned
  const InDetDD::SiDetectorElement* m_detElement;
  /// Transformation of the detector element
  // std::shared_ptr<const Amg::Transform3D> m_transform;
  // not smart pointer, because SiDetElement owns the transform
  // const Amg::Transform3D *m_transform;
  /// Scale matrix for transform
  Eigen::UniformScaling<double> m_transformScale;
  /// Center position of the detector element
  std::shared_ptr<const Vector3D> m_center;
  /// Normal vector to the detector element
  std::shared_ptr<const Vector3D> m_normal;
  /// Identifier of the detector element
  /// Boundaries of the detector element
  std::shared_ptr<const SurfaceBounds> m_bounds;
  ///  Thickness of this detector element
  double m_thickness;
  /// Corresponding Surface
  std::shared_ptr<const Surface> m_surface;
  /// possible contained surfaces
  std::vector<std::shared_ptr<const Surface>> m_surfaces;

  int                                  m_bec;
  GeoModelDetectorElement::Subdetector m_det;
  int                                  m_layer_disk;
  int                                  m_phi_module;
  int                                  m_eta_module;
  int                                  m_side;

  int m_layer_disk_max;
  int m_phi_module_max;
  int m_eta_module_max;
  int m_eta_module_min;
};


}

#endif
