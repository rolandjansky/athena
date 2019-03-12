/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_GeoModel/SCT_Flange.h"

#include "SCT_GeoModel/SCT_MaterialManager.h"

#include "SCT_GeoModel/SCT_GeometryManager.h"
#include "SCT_GeoModel/SCT_BarrelParameters.h"

#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/Units.h"


SCT_Flange::SCT_Flange(const std::string & name, int iLayer,
                       InDetDD::SCT_DetectorManager* detectorManager,
                       const SCT_GeometryManager* geometryManager,
                       SCT_MaterialManager* materials)
  : SCT_SharedComponentFactory(name, detectorManager, geometryManager, materials),
    m_iLayer(iLayer)
{
  getParameters();
  m_physVolume = build();
}


void 
SCT_Flange::getParameters()
{
  const SCT_BarrelParameters * parameters = m_geometryManager->barrelParameters();
    
  m_materialName    = parameters->flangeMaterial(m_iLayer);
  m_length      = parameters->flangeDeltaZ(m_iLayer);
  m_radialWidth = parameters->flangeDeltaR(m_iLayer);

  if(parameters->isOldGeometry()) {
    // Same outer radius as support cylinder
    m_outerRadius = parameters->supportCylOuterRadius(m_iLayer);
  }
  else {
    // Outer radius is inner radius of support cylinder
    m_outerRadius = parameters->supportCylInnerRadius(m_iLayer);
  }
  m_innerRadius = m_outerRadius - m_radialWidth;
}

GeoVPhysVol * 
SCT_Flange::build() 
{
  // Make the flange. A simple tube.
  const GeoTube * flangeShape = new GeoTube(m_innerRadius, m_outerRadius, 0.5 * m_length);
  m_material = m_materials->getMaterialForVolume(m_materialName, flangeShape->volume());
  if(!m_material) {m_material = m_materials->getMaterial(m_materialName);}
  const GeoLogVol * flangeLog = new GeoLogVol(getName(), flangeShape, m_material);

  GeoPhysVol * flange = new GeoPhysVol(flangeLog);

  return flange;
}
