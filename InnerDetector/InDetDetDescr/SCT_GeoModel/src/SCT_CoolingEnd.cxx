/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_GeoModel/SCT_CoolingEnd.h"

#include "SCT_GeoModel/SCT_MaterialManager.h"

#include "SCT_GeoModel/SCT_GeometryManager.h"
#include "SCT_GeoModel/SCT_BarrelParameters.h"

#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/Units.h"


SCT_CoolingEnd::SCT_CoolingEnd(const std::string & name, int iLayer,
                               double innerRadius,
                               double length,
                               InDetDD::SCT_DetectorManager* detectorManager,
                               const SCT_GeometryManager* geometryManager,
                               SCT_MaterialManager* materials)
  : SCT_SharedComponentFactory(name, detectorManager, geometryManager, materials),
    m_iLayer(iLayer),
    m_innerRadius(innerRadius),
    m_length(length)
{
  getParameters();
  m_physVolume = build();
}


void 
SCT_CoolingEnd::getParameters()
{
  const SCT_BarrelParameters * parameters = m_geometryManager->barrelParameters();
    
  m_materialName    = parameters->coolingEndMaterial(m_iLayer);
  m_radialWidth = parameters->clampDeltaR(m_iLayer);

  m_outerRadius = m_innerRadius + m_radialWidth;
}

GeoVPhysVol * 
SCT_CoolingEnd::build() 
{
  // Make volume representing cooling inlets, outlets and U-bends. A simple tube.
  const GeoTube * coolShape = new GeoTube(m_innerRadius, m_outerRadius, 0.5 * m_length);
  m_material = m_materials->getMaterialForVolume(m_materialName, coolShape->volume());
  if(!m_material) m_material = m_materials->getMaterial(m_materialName);
  const GeoLogVol * coolLog = new GeoLogVol(getName(), coolShape, m_material);

  GeoPhysVol * cool = new GeoPhysVol(coolLog);

  return cool;
}
