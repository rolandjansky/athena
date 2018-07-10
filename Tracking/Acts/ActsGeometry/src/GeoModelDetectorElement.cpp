// This file is part of the ACTS project.
//
// Copyright (C) 2016 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <boost/variant.hpp>
#include <boost/variant/get.hpp>

#include <mutex>

#include "InDetReadoutGeometry/TRT_EndcapElement.h"
#include "InDetReadoutGeometry/TRT_BarrelElement.h"

#include "GeoModelKernel/GeoVFullPhysVol.h"

#include "ActsGeometry/GeoModelDetectorElement.hpp"
#include "ActsGeometry/TrackingGeometrySvc.h"
#include "ActsInterop/IdentityHelper.h"

#include "Acts/Surfaces/StrawSurface.hpp"
#include "Acts/Surfaces/LineBounds.hpp"

#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/ThreadLocalContext.h"
#include "StoreGate/ReadCondHandle.h"


#include "GeoPrimitives/CLHEPtoEigenConverter.h"

#include "GaudiKernel/ContextSpecificPtr.h"
#include "GeoModelUtilities/GeoAlignmentStore.h"



Acts::GeoModelDetectorElement::GeoModelDetectorElement(
    const InDetDD::SiDetectorElement* detElem,
    const Acts::TrackingGeometrySvc* trkSvc)
  : m_trackingGeometrySvc(trkSvc)
{
  m_detElement = detElem;

  //InDetDD::DetectorShape shape = detElem->design().shape();

  //auto center     = detElem->center();
  auto boundsType = detElem->bounds().type();

  // thickness
  m_thickness = detElem->thickness();

  auto id = detElem->identify();


  if (boundsType == Trk::SurfaceBounds::Rectangle) {

    const InDetDD::SiDetectorDesign &design = detElem->design();
    double hlX = design.width()/2.;
    double hlY = design.length()/2.;

    auto rectangleBounds = std::make_shared<const RectangleBounds>(
        hlX, hlY);

    m_bounds = rectangleBounds;

    m_surface
      = std::make_shared<const PlaneSurface>(rectangleBounds, *this, id);

  } else if (boundsType == Trk::SurfaceBounds::Trapezoid) {

    std::string shapeString = detElem->getMaterialGeom()->getLogVol()->getShape()->type();
    //std::cout << __FUNCTION__ << "tapezoid, GeoLogVol -> shape says: " << shapeString << std::endl;

    const InDetDD::SiDetectorDesign &design = detElem->design();

    double minHlX = design.minWidth()/2.;
    double maxHlX = design.maxWidth()/2.;
    double hlY    = design.length()/2.;

    auto trapezoidBounds = std::make_shared<const TrapezoidBounds>(
        minHlX, maxHlX, hlY);

    m_bounds = trapezoidBounds;

    m_surface
      = std::make_shared<const PlaneSurface>(trapezoidBounds, *this, id);

  } else {
    throw std::domain_error("GeoModelDetectorElement does not support this surface type");
  }
}

Acts::GeoModelDetectorElement::GeoModelDetectorElement(
    std::shared_ptr<const Transform3D> trf,
    const InDetDD::TRT_BaseElement* detElem,
    const Identifier& id,
    const Acts::TrackingGeometrySvc* trkSvc)
  : m_trackingGeometrySvc(trkSvc)
{
  m_detElement = detElem;
  m_defTransform = trf;
  m_explicitIdentifier = id;
  
  // we know this is a straw
  double length = detElem->strawLength()/2.;

  // we need to find the radius
  auto ecElem = dynamic_cast<const InDetDD::TRT_EndcapElement*>(detElem);
  auto brlElem = dynamic_cast<const InDetDD::TRT_BarrelElement*>(detElem);
  double innerTubeRadius;
  if (ecElem) {
    innerTubeRadius = ecElem->getDescriptor()->innerTubeRadius();
  }
  else {
    innerTubeRadius = brlElem->getDescriptor()->innerTubeRadius();
  }
  
  auto lineBounds = std::make_shared<const Acts::LineBounds>(innerTubeRadius, length);
  m_bounds = lineBounds;

  auto straw = std::make_shared<const Acts::StrawSurface>(lineBounds, *this, detElem->identify());
  m_surface = straw;
}

Acts::IdentityHelper 
Acts::GeoModelDetectorElement::identityHelper() const 
{
  size_t which = m_detElement.which();
  if (which == 0) {
    return IdentityHelper(boost::get<const InDetDD::SiDetectorElement*>(m_detElement));
  } else {
    throw std::domain_error("Cannot get IdentityHelper for TRT element");
  }
}
  
void
Acts::GeoModelDetectorElement::assignIdentifier(const Identifier& /*identifier*/)
{
  throw std::domain_error(
      "Unable to assign Identifier in Acts::GeoModelDetectorElement");
}

const Acts::Transform3D&
Acts::GeoModelDetectorElement::transform(const Identifier&) const
{

  auto get_transform = [this](GeoAlignmentStore* store, bool def) -> Transform3D {
    return boost::apply_visitor(GetTransformVisitor(store, def, m_defTransform.get()), m_detElement);
  };

  auto ctx = Gaudi::Hive::currentContext();

  if (!ctx.valid()) {
    // this is really only the case single threaded, but let's be safe and lock it down
    std::lock_guard<std::mutex> guard(m_cacheMutex);
    if (m_defTransform) return *m_defTransform;
    m_defTransform = std::make_shared<const Transform3D>(get_transform(nullptr, true));
    return *m_defTransform;
  }

  // retrieve GAS from tracking geometry svc
  const GeoAlignmentStore* alignmentStore = m_trackingGeometrySvc->getGeoAlignmentStore(ctx);

  if (!rch.isValid()) {
    return *m_defTransform;
  }

  // since we "cache" in GAS for this IOV, we need this to be mutable.
  // @TODO: find out if this breaks something
  GeoAlignmentStore* alignmentStore = const_cast<GeoAlignmentStore*>(*rch);

  // early return if cache is valid for IOV
  const Transform3D* cachedTrf = alignmentStore->getTransform(this);
  if (cachedTrf != nullptr) return *cachedTrf;

  // no trf cached
  Transform3D trf = get_transform(alignmentStore, false);
  alignmentStore->setTransform(this, trf);
  return *(alignmentStore->getTransform(this));
  
}

const Acts::Surface&
Acts::GeoModelDetectorElement::surface(const Identifier&) const
{
  return (*m_surface);
}

double
Acts::GeoModelDetectorElement::thickness() const
{
  return m_thickness;
}

Identifier
Acts::GeoModelDetectorElement::identify() const
{
  

  return boost::apply_visitor(IdVisitor(m_explicitIdentifier), m_detElement);
}
