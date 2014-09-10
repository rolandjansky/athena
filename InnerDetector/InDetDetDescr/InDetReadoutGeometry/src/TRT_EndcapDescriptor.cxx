/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetReadoutGeometry/TRT_EndcapDescriptor.h"

#include "TrkSurfaces/CylinderBounds.h"

#include "CLHEP/Units/SystemOfUnits.h"

namespace InDetDD {



TRT_EndcapDescriptor::TRT_EndcapDescriptor():
  _nStraws(0),
  _strawPitch(0),
  _startPhi(0),
  _strawLength(0),
  _innerRadius(0),
  m_innerTubeRadius(2*CLHEP::mm),  // FIXME: Hardwired for now!!
  _f(NULL),
  _o(0),
  m_bounds(0)
{

}


TRT_EndcapDescriptor::~TRT_EndcapDescriptor()
{
  delete m_bounds;
}



void TRT_EndcapDescriptor::setStrawTransformField(const GeoXF::Function *xf, size_t offsetInto) {
  _f=xf;
  _o=offsetInto;
}



unsigned int  &TRT_EndcapDescriptor::nStraws() {
  return _nStraws;
}

double & TRT_EndcapDescriptor::strawPitch() {
  return _strawPitch;
}

double & TRT_EndcapDescriptor::startPhi() {
  return _startPhi;
}
  
double & TRT_EndcapDescriptor::strawLength() {
  return _strawLength;
}
  
double &  TRT_EndcapDescriptor::innerRadius() {
  return _innerRadius;
}

const Trk::SurfaceBounds & 
TRT_EndcapDescriptor::strawBounds() const
{
  if (!m_bounds) m_bounds = new Trk::CylinderBounds(m_innerTubeRadius, 0.5*_strawLength);
  return *m_bounds;
}

}
