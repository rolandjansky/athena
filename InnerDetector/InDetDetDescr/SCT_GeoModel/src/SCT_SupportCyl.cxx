/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_GeoModel/SCT_SupportCyl.h"

#include "SCT_GeoModel/SCT_MaterialManager.h"

#include "SCT_GeoModel/SCT_GeometryManager.h"
#include "SCT_GeoModel/SCT_BarrelParameters.h"

#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "CLHEP/Units/SystemOfUnits.h"


SCT_SupportCyl::SCT_SupportCyl(const std::string & name, int iLayer, double length)
  : SCT_SharedComponentFactory(name), m_iLayer(iLayer), m_length(length)
{
  getParameters();
  m_physVolume = build();
}


void 
SCT_SupportCyl::getParameters()
{
  const SCT_BarrelParameters * parameters = geometryManager()->barrelParameters();
       
  m_materialName = parameters->supportCylMaterial(m_iLayer);
  m_innerRadius = parameters->supportCylInnerRadius(m_iLayer);
  m_outerRadius = parameters->supportCylOuterRadius(m_iLayer);

}

GeoVPhysVol * 
SCT_SupportCyl::build() 
{
  // Make the support cyliner. A simple tube.
  SCT_MaterialManager materials;

  const GeoTube * supportCylShape = new GeoTube(m_innerRadius, m_outerRadius, 0.5 * m_length);
  m_material = materials.getMaterialForVolume(m_materialName, supportCylShape->volume());
  if(!m_material) {m_material = materials.getMaterial(m_materialName);}
  const GeoLogVol * supportCylLog = new GeoLogVol(getName(), supportCylShape, m_material);

  GeoPhysVol * supportCyl = new GeoPhysVol(supportCylLog);

  return supportCyl;
}


