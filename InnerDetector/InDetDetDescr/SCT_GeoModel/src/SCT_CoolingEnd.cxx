/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_GeoModel/SCT_CoolingEnd.h"

#include "SCT_GeoModel/SCT_MaterialManager.h"

#include "SCT_GeoModel/SCT_GeometryManager.h"
#include "SCT_GeoModel/SCT_BarrelParameters.h"

#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "CLHEP/Units/SystemOfUnits.h"


SCT_CoolingEnd::SCT_CoolingEnd(const std::string & name, int iLayer,
				   double innerRadius,
				   double length)
  : SCT_SharedComponentFactory(name), m_iLayer(iLayer),
    m_innerRadius(innerRadius),
    m_length(length)
{
  getParameters();
  m_physVolume = build();
}


void 
SCT_CoolingEnd::getParameters()
{
  const SCT_BarrelParameters * parameters = geometryManager()->barrelParameters();
    
  m_materialName    = parameters->coolingEndMaterial(m_iLayer);
  m_radialWidth = parameters->clampDeltaR(m_iLayer);

  m_outerRadius = m_innerRadius + m_radialWidth;
}

GeoVPhysVol * 
SCT_CoolingEnd::build() 
{
  // Make volume representing cooling inlets, outlets and U-bends. A simple tube.
  SCT_MaterialManager materials;

  const GeoTube * coolShape = new GeoTube(m_innerRadius, m_outerRadius, 0.5 * m_length);
  m_material = materials.getMaterialForVolume(m_materialName, coolShape->volume());
  if(!m_material) {m_material = materials.getMaterial(m_materialName);}
  const GeoLogVol * coolLog = new GeoLogVol(getName(), coolShape, m_material);

  GeoPhysVol * cool = new GeoPhysVol(coolLog);

  return cool;
}
