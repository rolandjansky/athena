/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "ActsGeometry/ActsDetectorElement.h"

// ATHENA
#include "ActsInterop/IdentityHelper.h"
#include "GeoPrimitives/CLHEPtoEigenConverter.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "SCT_ReadoutGeometry/StripBoxDesign.h"
#include "SCT_ReadoutGeometry/StripStereoAnnulusDesign.h"
#include "TRT_ReadoutGeometry/TRT_BarrelElement.h"
#include "TRT_ReadoutGeometry/TRT_EndcapElement.h"
#include "TrkSurfaces/AnnulusBounds.h"
#include "TrkSurfaces/RectangleBounds.h"
#include "TrkSurfaces/Surface.h"
#include "TrkSurfaces/SurfaceBounds.h"
#include "TrkSurfaces/TrapezoidBounds.h"

// PACKAGE
#include "ActsGeometry/ActsAlignmentStore.h"
#include "ActsGeometry/ActsGeometryContext.h"
#include "ActsGeometry/ActsTrackingGeometrySvc.h"

// ACTS
#include "Acts/Definitions/Units.hpp"
#include "Acts/Geometry/GeometryContext.hpp"
#include "Acts/Surfaces/AnnulusBounds.hpp"
#include "Acts/Surfaces/DiscSurface.hpp"
#include "Acts/Surfaces/LineBounds.hpp"
#include "Acts/Surfaces/PlaneSurface.hpp"
#include "Acts/Surfaces/RectangleBounds.hpp"
#include "Acts/Surfaces/StrawSurface.hpp"
#include "Acts/Surfaces/TrapezoidBounds.hpp"
#include "Acts/Visualization/ObjVisualization3D.hpp"
#include "Acts/Visualization/PlyVisualization3D.hpp"

// STL
#include <mutex>

// BOOST
#include <boost/variant.hpp>
#include <boost/variant/get.hpp>

using Acts::Surface;
using Acts::Transform3;

using namespace Acts::UnitLiterals;

constexpr double length_unit = 1_mm;

ActsDetectorElement::ActsDetectorElement(
    const InDetDD::SiDetectorElement &detElem) {
  m_detElement = &detElem;

  auto boundsType = detElem.bounds().type();

  m_thickness = detElem.thickness();

  if (boundsType == Trk::SurfaceBounds::Rectangle) {

    const InDetDD::SiDetectorDesign &design = detElem.design();
    double hlX = design.width() / 2. * length_unit;
    double hlY = design.length() / 2. * length_unit;

    auto rectangleBounds =
        std::make_shared<const Acts::RectangleBounds>(hlX, hlY);

    m_bounds = rectangleBounds;

    if (const auto *bd = dynamic_cast<const InDetDD::StripBoxDesign *>(&design);
        bd != nullptr) {
      // extra shift for split row modules
      m_extraTransform = Acts::Transform3::Identity() *
                         Acts::AngleAxis3{-M_PI / 2., Acts::Vector3::UnitZ()} *
                         bd->moduleShift() *
                         Acts::AngleAxis3{M_PI / 2., Acts::Vector3::UnitZ()};
    }

    m_surface =
        Acts::Surface::makeShared<Acts::PlaneSurface>(rectangleBounds, *this);

  } else if (boundsType == Trk::SurfaceBounds::Trapezoid) {

    const InDetDD::SiDetectorDesign &design = detElem.design();

    double minHlX = design.minWidth() / 2. * length_unit;
    double maxHlX = design.maxWidth() / 2. * length_unit;
    double hlY = design.length() / 2. * length_unit;

    auto trapezoidBounds =
        std::make_shared<const Acts::TrapezoidBounds>(minHlX, maxHlX, hlY);

    m_bounds = trapezoidBounds;

    m_surface =
        Acts::Surface::makeShared<Acts::PlaneSurface>(trapezoidBounds, *this);

  } else if (boundsType == Trk::SurfaceBounds::Annulus) {

    const InDetDD::SiDetectorDesign &design = detElem.design();
    const auto *annulus =
        dynamic_cast<const InDetDD::StripStereoAnnulusDesign *>(&design);
    if (annulus == nullptr) {
      throw std::domain_error("ActsDetectorElement got inconsistent surface");
    }

    double phi = annulus->phiWidth();
    double phiS = annulus->stereo();
    double R = annulus->waferCentreR();
    double maxR = annulus->maxR();
    double minR = annulus->minR();

    double phiAvg =
        0; // phiAvg is the bounds-internal local rotation. We don't want one

    // phi is the total opening angle, set up symmetric phi bounds
    double phiMax = phi / 2.;
    double phiMin = -phiMax;

    // need to rotate pi/2 to reproduce ABXY orientation, phiS so that phi=0
    // is center and symmetric
    double phiShift = M_PI / 2. - phiS;

    Amg::Vector2D originStripXYRotated(R * (1 - std::cos(phiS)),
                                       R * std::sin(-phiS));

    auto annulusBounds = std::make_shared<Acts::AnnulusBounds>(
        minR, maxR, phiMin, phiMax, originStripXYRotated, phiAvg);
    m_bounds = annulusBounds;

    m_surface =
        Acts::Surface::makeShared<Acts::DiscSurface>(annulusBounds, *this);

    Amg::Vector2D origin2D = annulusBounds->moduleOrigin();
    Amg::Translation3D transl(Amg::Vector3D(origin2D.x(), origin2D.y(), 0));
    Amg::Rotation3D rot(Amg::AngleAxis3D(-phiShift, Amg::Vector3D::UnitZ()));
    Amg::Transform3D originTrf;
    originTrf = transl * rot;
    m_extraTransform = originTrf.inverse();

  } else {
    std::cout << boundsType << std::endl;
    throw std::domain_error(
        "ActsDetectorElement does not support this surface type");
  }
}

ActsDetectorElement::ActsDetectorElement(
    const Acts::Transform3 &trf, const InDetDD::TRT_BaseElement &detElem,
    const Identifier &id) {
  m_detElement = &detElem;
  m_defTransform = trf;
  m_explicitIdentifier = id;

  // we know this is a straw
  double length = detElem.strawLength() * 0.5 * length_unit;

  // we need to find the radius
  auto ecElem = dynamic_cast<const InDetDD::TRT_EndcapElement *>(&detElem);
  auto brlElem = dynamic_cast<const InDetDD::TRT_BarrelElement *>(&detElem);
  double innerTubeRadius{0.};
  if (ecElem) {
    innerTubeRadius = ecElem->getDescriptor()->innerTubeRadius() * length_unit;
  } else {
    if (brlElem) {
      innerTubeRadius =
          brlElem->getDescriptor()->innerTubeRadius() * length_unit;
    } else {
      throw std::runtime_error(
          "Cannot get tube radius for element in ActsDetectorElement c'tor");
    }
  }

  auto lineBounds =
      std::make_shared<const Acts::LineBounds>(innerTubeRadius, length);
  m_bounds = lineBounds;

  m_surface = Acts::Surface::makeShared<Acts::StrawSurface>(lineBounds, *this);
}

IdentityHelper ActsDetectorElement::identityHelper() const {
  if (const auto *detElem =
          dynamic_cast<const InDetDD::SiDetectorElement *>(m_detElement);
      detElem != nullptr) {
    return IdentityHelper(detElem);
  } else {
    throw std::domain_error("Cannot get IdentityHelper for TRT element");
  }
}

const Acts::Transform3 &
ActsDetectorElement::transform(const Acts::GeometryContext &anygctx) const {
  // any cast to known context type
  const ActsGeometryContext *gctx = anygctx.get<const ActsGeometryContext *>();

  // This is needed for initial geometry construction. At that point, we don't
  // have a consistent view of the geometry yet, and thus we can't populate an
  // alignment store at that time.
  if (gctx->construction) {
    // this should only happen at initialize (1 thread, but mutex anyway)
    return getDefaultTransformMutexed();
  }

  // unpack the alignment store from the context
  const ActsAlignmentStore *alignmentStore = gctx->alignmentStore;
  // no GAS, is this initialization?
  assert(alignmentStore != nullptr);

  // get the correct cached transform
  // units should be fine here since we converted at construction
  const Acts::Transform3 *cachedTrf = alignmentStore->getTransform(this);

  assert(cachedTrf != nullptr);

  return *cachedTrf;
}

void ActsDetectorElement::storeTransform(ActsAlignmentStore *gas) const {
  Amg::Transform3D trf;
  if (const auto *detElem =
          dynamic_cast<const InDetDD::SiDetectorElement *>(m_detElement);
      detElem != nullptr) {
    Amg::Transform3D l2g =
        detElem->getMaterialGeom()->getAbsoluteTransform(gas) *
        Amg::CLHEPTransformToEigen(detElem->recoToHitTransform());

    // need to make sure translation has correct units
    l2g.translation() *= 1.0 / CLHEP::mm * length_unit;

    l2g = l2g * m_extraTransform;

    trf = l2g;
  } else if (const auto *detElem =
                 dynamic_cast<const InDetDD::TRT_BaseElement *>(m_detElement);
             detElem != nullptr) {
    // So far: NO ALIGNMENT for the ACTS TRT version. Default transform set in
    // constructor, should be safe to access without mutex.
    trf = *m_defTransform;
  } else {
    throw std::runtime_error{"Unknown detector element type"};
  }

  gas->setTransform(this, trf);
  if (gas->getTransform(this) == nullptr) {
    throw std::runtime_error(
        "Detector element was unable to store transform in GAS");
  }
}

const Acts::Transform3 &
ActsDetectorElement::getDefaultTransformMutexed() const {
  std::lock_guard<std::mutex> guard(m_cacheMutex);
  if (m_defTransform) {
    return *m_defTransform;
  }
  // transform not yet set
  if (const auto *detElem =
          dynamic_cast<const InDetDD::SiDetectorElement *>(m_detElement);
      detElem != nullptr) {
    Amg::Transform3D l2g =
        detElem->getMaterialGeom()->getDefAbsoluteTransform() *
        Amg::CLHEPTransformToEigen(detElem->recoToHitTransform());

    l2g.translation() *= 1.0 / CLHEP::mm * length_unit;

    l2g = l2g * m_extraTransform;

    m_defTransform = l2g;
  } else if (const auto *detElem =
                 dynamic_cast<const InDetDD::TRT_BaseElement *>(m_detElement);
             detElem != nullptr) {
    throw std::logic_error{
        "TRT transform should have been set in the constructor"};
  } else {
    throw std::runtime_error{"Unknown detector element type"};
  }

  return *m_defTransform;
}

const Acts::Surface &ActsDetectorElement::surface() const {
  return (*m_surface);
}

const Trk::Surface &ActsDetectorElement::atlasSurface() const {
  if (const auto *detElem =
          dynamic_cast<const InDetDD::SiDetectorElement *>(m_detElement);
      detElem != nullptr) {
    return detElem->surface();
  } else {
    throw std::domain_error("Cannot get surface for TRT element");
  }
}

double ActsDetectorElement::thickness() const { return m_thickness; }

Identifier ActsDetectorElement::identify() const {
  if (const auto *detElem =
          dynamic_cast<const InDetDD::SiDetectorElement *>(m_detElement);
      detElem != nullptr) {
    return detElem->identify();
  } else if (dynamic_cast<const InDetDD::TRT_BaseElement *>(m_detElement) !=
             nullptr) {
    return m_explicitIdentifier;
  } else {
    throw std::runtime_error{"Unknown detector element type"};
  }
}

const GeoVDetectorElement *
ActsDetectorElement::upstreamDetectorElement() const {
  return m_detElement;
}