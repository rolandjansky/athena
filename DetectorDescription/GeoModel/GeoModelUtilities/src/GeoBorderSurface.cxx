/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoModelUtilities/GeoBorderSurface.h"
#include "GeoModelUtilities/GeoOpticalPhysVol.h"
#include "GeoModelUtilities/GeoOpticalSurface.h"

#include <exception>

GeoBorderSurface::GeoBorderSurface(const std::string& name,
				   GeoOpticalPhysVol* pv1,
				   GeoOpticalPhysVol* pv2,
				   GeoOpticalSurface* opticalSurface):
  m_name(name),
  m_pv1(pv1),
  m_pv2(pv2),
  m_opticalSurface(opticalSurface)
{
  if(pv1->isShared() || pv2->isShared())
    throw std::runtime_error("Attempt to create a surface with shared physical volumes");

  pv1->ref();
  pv2->ref();
  opticalSurface->ref();
}

GeoBorderSurface::GeoBorderSurface(const GeoBorderSurface &right):
  m_name(right.m_name),
  m_pv1(right.m_pv1),
  m_pv2(right.m_pv2),
  m_opticalSurface(right.m_opticalSurface)
{
  m_pv1->ref();
  m_pv2->ref();
  m_opticalSurface->ref();
}

GeoBorderSurface::~GeoBorderSurface()
{
  m_pv1->unref();
  m_pv2->unref();
  m_opticalSurface->unref();
}
