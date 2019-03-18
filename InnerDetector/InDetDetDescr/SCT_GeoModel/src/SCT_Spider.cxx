/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_GeoModel/SCT_Spider.h"

#include "SCT_GeoModel/SCT_MaterialManager.h"

#include "SCT_GeoModel/SCT_GeometryManager.h"
#include "SCT_GeoModel/SCT_BarrelParameters.h"

#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/Units.h"


SCT_Spider::SCT_Spider(const std::string & name,
                       InDetDD::SCT_DetectorManager* detectorManager,
                       const SCT_GeometryManager* geometryManager,
                       SCT_MaterialManager* materials)
  : SCT_SharedComponentFactory(name, detectorManager, geometryManager, materials)
{
  getParameters();
  m_physVolume = build();
}


void 
SCT_Spider::getParameters()
{
  const SCT_BarrelParameters * parameters = m_geometryManager->barrelParameters();
   
  m_materialName = parameters->spiderMaterial();
  m_length       = parameters->spiderDeltaZ();
  m_innerRadius  = parameters->spiderInnerRadius();
  m_outerRadius  = parameters->spiderOuterRadius();
}

GeoVPhysVol * 
SCT_Spider::build() 
{
  // Make the spider. A simple tube.
  const GeoTube * spiderShape = new GeoTube(m_innerRadius, m_outerRadius, 0.5 * m_length);
  m_material = m_materials->getMaterialForVolume(m_materialName, spiderShape->volume());
  if (!m_material) m_material = m_materials->getMaterial(m_materialName);
  const GeoLogVol * spiderLog = new GeoLogVol(getName(), spiderShape, m_material);

  GeoPhysVol * spider = new GeoPhysVol(spiderLog);

  return spider;
}

