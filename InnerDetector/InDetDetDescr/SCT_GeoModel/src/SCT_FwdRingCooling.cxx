/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_GeoModel/SCT_FwdRingCooling.h"

#include "SCT_GeoModel/SCT_MaterialManager.h"

#include "SCT_GeoModel/SCT_GeometryManager.h"
#include "SCT_GeoModel/SCT_ForwardParameters.h"

#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/Units.h"

#include <cmath>

inline double sqr(double x) {return x*x;}

SCT_FwdRingCooling::SCT_FwdRingCooling(const std::string & name, int ringType,
                                       InDetDD::SCT_DetectorManager* detectorManager,
                                       const SCT_GeometryManager* geometryManager,
                                       SCT_MaterialManager* materials)
  : SCT_SharedComponentFactory(name, detectorManager, geometryManager, materials),
    m_ringType(ringType)
{
  getParameters();
  m_physVolume = build();
}


void 
SCT_FwdRingCooling::getParameters()
{
  const SCT_ForwardParameters * parameters = m_geometryManager->forwardParameters();
    
  m_materialName= parameters->fwdRingCoolingMaterial(m_ringType);
  m_innerRadius = parameters->fwdRingCoolingInnerRadius(m_ringType);
  m_outerRadius = parameters->fwdRingCoolingOuterRadius(m_ringType);
  m_thickness   = parameters->fwdRingCoolingThickness(m_ringType);
  m_numModules  = parameters->fwdRingNumModules(m_ringType);
}

GeoVPhysVol * 
SCT_FwdRingCooling::build() 
{
  const GeoTube * ringCoolingShape = new GeoTube(m_innerRadius, m_outerRadius, 0.5 * m_thickness);
  m_material = m_materials->getMaterialForVolume(m_materialName, ringCoolingShape->volume());
  const GeoLogVol * ringCoolingLog = new GeoLogVol(getName(), ringCoolingShape, m_material);

  GeoPhysVol * ringCooling = new GeoPhysVol(ringCoolingLog);

  return ringCooling;
}
