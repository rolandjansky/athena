/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_GeoModel/SCT_FwdRingCooling.h"

#include "SCT_GeoModel/SCT_MaterialManager.h"

#include "SCT_GeoModel/SCT_GeometryManager.h"
#include "SCT_GeoModel/SCT_ForwardParameters.h"

#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Units/PhysicalConstants.h" // For pi

#include <cmath>

inline double sqr(double x) {return x*x;}

SCT_FwdRingCooling::SCT_FwdRingCooling(const std::string & name, int ringType)
  : SCT_SharedComponentFactory(name), m_ringType(ringType)
{
  getParameters();
  m_physVolume = build();
}


void 
SCT_FwdRingCooling::getParameters()
{
  const SCT_ForwardParameters * parameters = geometryManager()->forwardParameters();
    
  m_materialName= parameters->fwdRingCoolingMaterial(m_ringType);
  m_innerRadius = parameters->fwdRingCoolingInnerRadius(m_ringType);
  m_outerRadius = parameters->fwdRingCoolingOuterRadius(m_ringType);
  m_thickness   = parameters->fwdRingCoolingThickness(m_ringType);
  m_numModules  = parameters->fwdRingNumModules(m_ringType);
}

GeoVPhysVol * 
SCT_FwdRingCooling::build() 
{
  SCT_MaterialManager materials;

  const GeoTube * ringCoolingShape = new GeoTube(m_innerRadius, m_outerRadius, 0.5 * m_thickness);
  m_material = materials.getMaterialForVolume(m_materialName, ringCoolingShape->volume());
  const GeoLogVol * ringCoolingLog = new GeoLogVol(getName(), ringCoolingShape, m_material);

  GeoPhysVol * ringCooling = new GeoPhysVol(ringCoolingLog);

  return ringCooling;
}
