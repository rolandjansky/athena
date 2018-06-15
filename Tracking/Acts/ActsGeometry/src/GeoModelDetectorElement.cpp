// This file is part of the ACTS project.
//
// Copyright (C) 2016 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <boost/variant.hpp>
#include <boost/variant/get.hpp>


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
    const Acts::TrackingGeometrySvc* trkSvc)
  : m_trackingGeometrySvc(trkSvc)
{
  m_detElement = detElem;
  m_transform = trf;
  
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

  struct get_transform_visitor : public boost::static_visitor<Transform3D> {

    explicit get_transform_visitor(GeoAlignmentStore* store, bool default, const Transform3D* fallback)
      : m_store(store), m_default(default), m_fallback(fallback)
    {}

    Transform3D operator()(const InDetDD::SiDetectorElement* detElem) const
    {
      HepGeom::Transform3D g2l;
      if (m_default) g2l = detElem->getMaterialGeom()->getDefAbsoluteTransform(m_store);
      else g2l = detElem->getMaterialGeom()->getAbsoluteTransform(m_store);

      return clhep2eigen(g2l * detElem->recoToHitTransform());
    }

    Transform3D operator()(const InDetDD::TRT_BaseElement* detElem) const
    {
      return *m_fallback;
    }

    GeoAlignmentStore* m_store;
    bool m_default;
    const Transform3D* m_fallback;
  };

  auto get_transform = [this](GeoAlignmentStore* store, bool default) {
    return boost::apply_visitor(get_transform_visitor(store, default), m_detElement);
  };

  auto ctx = Gaudi::Hive::currentContext();

  if (!ctx.valid()) {
    return get_transform(nullptr, true); // no store, default transform
  }

  // ctx is valid

  SG::ReadCondHandle<GeoAlignmentStore> rch(m_trackingGeometrySvc->alignmentCondHandleKey());
  GeoAlignmentStore* alignmentStore = const_cast<GeoAlignmentStore*>(*rch);

  // early return if cache is valid for IOV
  Transform3D* trf = alignmentStore->getTransform(this);
  if (trf != nullptr) return *trf;




  // unpack context specific storage
  //Transform3D& trf = m_ctxSpecificTransform;


  // OLD default way to get transform
  size_t which = m_detElement.which();
  const GeoVFullPhysVol* physVol;
  
  Transform3D recoToHit = Transform3D::Identity();

  auto clhep2eigen = [](auto const &trf) { return Amg::CLHEPTransformToEigen(trf); };

  if (which == 0) {
    auto de = boost::get<const InDetDD::SiDetectorElement*>(m_detElement);
    physVol = de->getMaterialGeom();
    trf = clhep2eigen(physVol->getDefAbsoluteTransform());
    trf = trf * clhep2eigen(de->recoToHitTransform());
    recoToHit = clhep2eigen(de->recoToHitTransform());
  }
  else {
    auto de = boost::get<const InDetDD::TRT_BaseElement*>(m_detElement);
    physVol = de->getMaterialGeom();
    //nominal = &de->transform();
    trf = *m_transform;
  }
  



  if (!ctx.valid()) {
    // don't have valid context yet, this is probably geometry construction
    //trans = physVol->getDefAbsoluteTransform();
    //return *(new Transform3D(Amg::CLHEPTransformToEigen(trans)));
    return trf; // def transform
  }

  // have valid context, get aligned transform
  //const ShiftCondObj *shift = *rch;
  // this is obviously not great
  
  HepGeom::Transform3D trans = physVol->getAbsoluteTransform(alignmentStore);

  //std::cout << __FUNCTION__ << " alignmentStore: " << alignmentStore << std::endl;

  //if (shift != 0) {
    //std::cout << "  read CH: " << rch.key() << " = " << *shift << std::endl;
  //} else {
    //std::cout << "  CDO ptr for " << rch.key() << " == zero" << std::endl;
  //}

  // build fake shift along z
  //Transform3D shiftTrf;
  //shiftTrf = Translation3D(Vector3D::UnitZ() * shift->val());
  //trf = shiftTrf * trf;
  
  // @TODO: This is SUPER EVIL! Only temporary
  //trans = physVol->getAbsoluteTransform();
  //Transform3D* aligned = new Transform3D(shiftTrf * Amg::CLHEPTransformToEigen(trans));

  trf = clhep2eigen(trans) * recoToHit;

  return trf;

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
  size_t which = m_detElement.which();
  if (which == 0) {
    return boost::get<const InDetDD::SiDetectorElement*>(m_detElement)->identify();
  }
  else {
    return boost::get<const InDetDD::TRT_BaseElement*>(m_detElement)->identify();
  }
}
