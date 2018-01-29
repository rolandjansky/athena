// This file is part of the ACTS project.
//
// Copyright (C) 2016 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "GeomACTS/GeoModelDetectorElement.hpp"


Acts::GeoModelDetectorElement::GeoModelDetectorElement(const InDetDD::SiDetectorElement* detElem)
{
  m_detElement = detElem;

  //InDetDD::DetectorShape shape = m_detElement->design().shape();

  //auto center     = m_detElement->center();
  auto boundsType = m_detElement->bounds().type();

  auto id = m_detElement->identify();

  // collect information
  if (m_detElement->isPixel()) {
    auto hlp     = dynamic_cast<const PixelID*>(m_detElement->getIdHelper());
    m_bec        = hlp->barrel_ec(id);
    m_det        = GeoModelDetectorElement::Subdetector::Pixel;
    m_layer_disk = hlp->layer_disk(id);
    m_phi_module = hlp->phi_module(id);
    m_eta_module = hlp->eta_module(id);
    m_side       = 0;

    m_layer_disk_max = hlp->layer_disk_max(id);
    m_phi_module_max = hlp->phi_module_max(id);
    m_eta_module_max = hlp->eta_module_max(id);
    m_eta_module_min = hlp->eta_module_min(id);
  } else if (m_detElement->isSCT()) {
    auto hlp     = dynamic_cast<const SCT_ID*>(m_detElement->getIdHelper());
    m_bec        = hlp->barrel_ec(id);
    m_det        = GeoModelDetectorElement::Subdetector::SCT;
    m_layer_disk = hlp->layer_disk(id);
    m_phi_module = hlp->phi_module(id);
    m_eta_module = hlp->eta_module(id);
    m_side       = hlp->side(id);

    m_layer_disk_max = hlp->layer_disk_max(id);
    m_phi_module_max = hlp->phi_module_max(id);
    m_eta_module_max = hlp->eta_module_max(id);
    m_eta_module_min = hlp->eta_module_min(id);
  } else {
    // ACTS_ERROR("Only Pixel and SCT are currently supported");
    // not implemented
  }

  // thickness
  m_thickness = m_detElement->thickness();


  if (boundsType == Trk::SurfaceBounds::Rectangle) {
    // we ignore Trk::*
    //auto rect
    //= dynamic_cast<const Trk::RectangleBounds*>(&m_detElement->bounds());

    //auto rectangleBounds = std::make_shared<const RectangleBounds>(
    //rect->halflengthX(), rect->halflengthY());

    const InDetDD::SiDetectorDesign &design = m_detElement->design();
    double hlX = design.width()/2.;
    double hlY = design.length()/2.;

    auto rectangleBounds = std::make_shared<const RectangleBounds>(
        hlX, hlY);

    m_bounds = rectangleBounds;

    m_surface
      = std::make_shared<const PlaneSurface>(rectangleBounds, *this, id);
  } else if (boundsType == Trk::SurfaceBounds::Trapezoid) {

    // we ignore Trk::*
    //auto trap
    //= dynamic_cast<const Trk::TrapezoidBounds*>(&m_detElement->bounds());

    //auto trapezoidBounds = std::make_shared<const TrapezoidBounds>(
    //trap->minHalflengthX(), trap->maxHalflengthX(), trap->halflengthY());

    const InDetDD::SiDetectorDesign &design = m_detElement->design();

    double minHlX = design.minWidth()/2.;
    double maxHlX = design.maxWidth()/2.;
    double hlY    = design.length()/2.;

    auto trapezoidBounds = std::make_shared<const TrapezoidBounds>(
        minHlX, maxHlX, hlY);

    m_bounds = trapezoidBounds;

    m_surface
      = std::make_shared<const PlaneSurface>(trapezoidBounds, *this, id);

  } else {
    // ACTS_WARNING("GeoModel bounds type " << boundsType << " is not
    // supported");
    // std::cout << "not implemented" << std::endl;
    // not imlemented
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
  return m_detElement->transform();
}

const Acts::Surface&
Acts::GeoModelDetectorElement::surface(const Identifier&) const
{
  return (*m_surface);
}

const std::vector<std::shared_ptr<const Acts::Surface>>&
Acts::GeoModelDetectorElement::surfaces() const
{
  return (m_surfaces);
}

double
Acts::GeoModelDetectorElement::thickness() const
{
  return m_thickness;
}

Identifier
Acts::GeoModelDetectorElement::identify() const
{
  return m_detElement->identify();
}
