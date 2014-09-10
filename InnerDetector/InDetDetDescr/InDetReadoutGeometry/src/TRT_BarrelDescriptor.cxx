/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetReadoutGeometry/TRT_BarrelDescriptor.h"

#include "TrkSurfaces/CylinderBounds.h"

#include "CLHEP/Units/SystemOfUnits.h"

namespace InDetDD {

TRT_BarrelDescriptor::TRT_BarrelDescriptor()
  : _length(0),
    _zDead(0),
    _zPos(0),
    m_innerTubeRadius(2*CLHEP::mm), // FIXME: Hardwired for now!!
    _f(NULL),
    _o(0),
    m_bounds(0)

{
}


TRT_BarrelDescriptor::~TRT_BarrelDescriptor()
{
  delete m_bounds;
}


void TRT_BarrelDescriptor::addStraw(double xPos, double yPos) {

  _x.push_back(xPos);
  _y.push_back(yPos);

}


void TRT_BarrelDescriptor::setStrawTransformField(const GeoXF::Function *xf, size_t offsetInto) {
  _f=xf;
  _o=offsetInto;
}

const Trk::SurfaceBounds & 
TRT_BarrelDescriptor::strawBounds() const
{
  if (!m_bounds) m_bounds = new Trk::CylinderBounds(m_innerTubeRadius, 0.5*_length);
  return *m_bounds;
}


}
