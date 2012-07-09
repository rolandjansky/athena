/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_GeoModel/SCT_Spider.h"

#include "SCT_GeoModel/SCT_MaterialManager.h"

#include "SCT_GeoModel/SCT_GeometryManager.h"
#include "SCT_GeoModel/SCT_BarrelParameters.h"

#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "CLHEP/Units/SystemOfUnits.h"


SCT_Spider::SCT_Spider(const std::string & name)
  : SCT_SharedComponentFactory(name)
{
  getParameters();
  m_physVolume = build();
}


void 
SCT_Spider::getParameters()
{
  const SCT_BarrelParameters * parameters = geometryManager()->barrelParameters();
   
  m_materialName = parameters->spiderMaterial();
  m_length       = parameters->spiderDeltaZ();
  m_innerRadius  = parameters->spiderInnerRadius();
  m_outerRadius  = parameters->spiderOuterRadius();
}

GeoVPhysVol * 
SCT_Spider::build() 
{
  // Make the spider. A simple tube.
  SCT_MaterialManager materials;

  const GeoTube * spiderShape = new GeoTube(m_innerRadius, m_outerRadius, 0.5 * m_length);
  m_material = materials.getMaterialForVolume(m_materialName, spiderShape->volume());
  if(!m_material) {m_material = materials.getMaterial(m_materialName);}
  const GeoLogVol * spiderLog = new GeoLogVol(getName(), spiderShape, m_material);

  GeoPhysVol * spider = new GeoPhysVol(spiderLog);

  return spider;
}

