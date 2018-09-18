/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// ATHENA
#include "InDetReadoutGeometry/TRT_EndcapElement.h"
#include "InDetReadoutGeometry/TRT_BarrelElement.h"
#include "GeoModelKernel/GeoVFullPhysVol.h"
#include "ActsInterop/IdentityHelper.h"
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/ThreadLocalContext.h"
#include "StoreGate/ReadCondHandle.h"
#include "GeoPrimitives/CLHEPtoEigenConverter.h"
#include "GaudiKernel/ContextSpecificPtr.h"
#include "GeoModelUtilities/GeoAlignmentStore.h"

// PACKAGE
#include "ActsGeometry/GeoModelDetectorElement.hpp"
#include "ActsGeometry/ActsTrackingGeometrySvc.h"

// ACTS
#include "Acts/Surfaces/StrawSurface.hpp"
#include "Acts/Surfaces/LineBounds.hpp"

// STL
#include <mutex>

// BOOST
#include <boost/variant.hpp>
#include <boost/variant/get.hpp>



Acts::GeoModelDetectorElement::GeoModelDetectorElement(
    const InDetDD::SiDetectorElement* detElem,
    const ActsTrackingGeometrySvc* trkSvc)
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
      = std::make_shared<const PlaneSurface>(rectangleBounds, *this);

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
      = std::make_shared<const PlaneSurface>(trapezoidBounds, *this);

  } else {
    throw std::domain_error("GeoModelDetectorElement does not support this surface type");
  }
}

Acts::GeoModelDetectorElement::GeoModelDetectorElement(
    std::shared_ptr<const Transform3D> trf,
    const InDetDD::TRT_BaseElement* detElem,
    const Identifier& id,
    const ActsTrackingGeometrySvc* trkSvc)
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

  auto straw = std::make_shared<const Acts::StrawSurface>(lineBounds, *this);
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
  
const Acts::Transform3D&
Acts::GeoModelDetectorElement::transform() const
{


  auto ctx = Gaudi::Hive::currentContext();

  if (!ctx.valid()) {
    // this is really only the case single threaded, but let's be safe and lock it down
    // also, we're not super afraid about performance here
    return getDefaultTransformMutexed();
  }

  // retrieve GAS from tracking geometry svc
  const GeoAlignmentStore* alignmentStore = m_trackingGeometrySvc->getGeoAlignmentStore(ctx);

  // no GAS, is this initialization?
  if (alignmentStore == nullptr) {
    throw std::runtime_error("GeoAlignmentStore could not be found for valid context.");
  }

  const Transform3D* cachedTrf = alignmentStore->getTransform(this);
  if (cachedTrf == nullptr) {
    throw std::runtime_error("Detector transform not found in GeoAlignmentStore.");
  }

  return *cachedTrf;
}

void
Acts::GeoModelDetectorElement::storeTransform(GeoAlignmentStore* gas) const
{
  struct get_transform : public boost::static_visitor<Transform3D>
  {
    get_transform(GeoAlignmentStore* gas, const Transform3D* trtTrf)
      : m_store(gas), m_trtTrf(trtTrf) {}

    Transform3D operator()(const InDetDD::SiDetectorElement* detElem) const
    {
      HepGeom::Transform3D g2l 
        = detElem->getMaterialGeom()->getAbsoluteTransform(m_store);

      return Amg::CLHEPTransformToEigen(g2l * detElem->recoToHitTransform());
    }
    
    Transform3D operator()(const InDetDD::TRT_BaseElement*) const
    {
      return *m_trtTrf;
    }

    GeoAlignmentStore* m_store;
    const Transform3D* m_trtTrf;
  };

  Transform3D trf 
    = boost::apply_visitor(get_transform(gas, m_defTransform.get()), m_detElement);

  gas->setTransform(this, trf);
  if (gas->getTransform(this) == nullptr) {
    throw std::runtime_error("Detector element was unable to store transform in GAS");
  }

}

const Acts::Transform3D&
Acts::GeoModelDetectorElement::getDefaultTransformMutexed() const
{
  struct get_default_transform : public boost::static_visitor<Transform3D>
  {
    get_default_transform(const Transform3D* trtTrf) : m_trtTrf(trtTrf) {}

    Transform3D operator()(const InDetDD::SiDetectorElement* detElem) const
    {
      HepGeom::Transform3D g2l 
        = detElem->getMaterialGeom()->getDefAbsoluteTransform();

      return Amg::CLHEPTransformToEigen(g2l * detElem->recoToHitTransform());
    }
    
    Transform3D operator()(const InDetDD::TRT_BaseElement*) const
    {
      return *m_trtTrf;
    }

    const Transform3D* m_trtTrf;
  };
    
  std::lock_guard<std::mutex> guard(m_cacheMutex);
  if (m_defTransform) {
    return *m_defTransform;
  }
  // transform not yet set
  m_defTransform 
    = std::make_shared<const Transform3D>(
        boost::apply_visitor(get_default_transform(m_defTransform.get()), m_detElement));

  return *m_defTransform;
}

const Acts::Surface&
Acts::GeoModelDetectorElement::surface() const
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
