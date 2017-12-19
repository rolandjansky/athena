/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetReadoutGeometry/TRT_EndcapDescriptor.h"

#include "TrkSurfaces/CylinderBounds.h"

#include "CLHEP/Units/SystemOfUnits.h"

namespace InDetDD {



TRT_EndcapDescriptor::TRT_EndcapDescriptor():
  m_nStraws(0),
  m_strawPitch(0),
  m_startPhi(0),
  m_strawLength(0),
  m_innerRadius(0),
  m_innerTubeRadius(2*CLHEP::mm),  // FIXME: Hardwired for now!!
  m_f(NULL),
  m_o(0),
  m_bounds(0)
{

}


TRT_EndcapDescriptor::~TRT_EndcapDescriptor()
{
  delete m_bounds;
}



void TRT_EndcapDescriptor::setStrawTransformField(const GeoXF::Function *xf, size_t offsetInto) {
  m_f=xf;
  m_o=offsetInto;
}



unsigned int  &TRT_EndcapDescriptor::nStraws() {
  return m_nStraws;
}

double & TRT_EndcapDescriptor::strawPitch() {
  return m_strawPitch;
}

double & TRT_EndcapDescriptor::startPhi() {
  return m_startPhi;
}
  
double & TRT_EndcapDescriptor::strawLength() {
  return m_strawLength;
}
  
double &  TRT_EndcapDescriptor::innerRadius() {
  return m_innerRadius;
}

const Trk::SurfaceBounds & 
TRT_EndcapDescriptor::strawBounds() const
{
  if (!m_bounds) m_bounds = new Trk::CylinderBounds(m_innerTubeRadius, 0.5*m_strawLength);
  return *m_bounds;
}

}
