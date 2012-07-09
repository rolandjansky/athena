/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_GeoModel/SCT_Flange.h"

#include "SCT_GeoModel/SCT_MaterialManager.h"

#include "SCT_GeoModel/SCT_GeometryManager.h"
#include "SCT_GeoModel/SCT_BarrelParameters.h"

#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "CLHEP/Units/SystemOfUnits.h"


SCT_Flange::SCT_Flange(const std::string & name, int iLayer)
  : SCT_SharedComponentFactory(name), m_iLayer(iLayer)
{
  getParameters();
  m_physVolume = build();
}


void 
SCT_Flange::getParameters()
{
  const SCT_BarrelParameters * parameters = geometryManager()->barrelParameters();
    
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
  SCT_MaterialManager materials;

  const GeoTube * flangeShape = new GeoTube(m_innerRadius, m_outerRadius, 0.5 * m_length);
  m_material = materials.getMaterialForVolume(m_materialName, flangeShape->volume());
  if(!m_material) {m_material = materials.getMaterial(m_materialName);}
  const GeoLogVol * flangeLog = new GeoLogVol(getName(), flangeShape, m_material);

  GeoPhysVol * flange = new GeoPhysVol(flangeLog);

  return flange;
}
