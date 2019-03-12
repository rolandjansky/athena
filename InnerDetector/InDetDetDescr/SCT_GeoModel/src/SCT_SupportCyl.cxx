/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_GeoModel/SCT_SupportCyl.h"

#include "SCT_GeoModel/SCT_MaterialManager.h"

#include "SCT_GeoModel/SCT_GeometryManager.h"
#include "SCT_GeoModel/SCT_BarrelParameters.h"

#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/Units.h"


SCT_SupportCyl::SCT_SupportCyl(const std::string & name, int iLayer, double length,
                               InDetDD::SCT_DetectorManager* detectorManager,
                               const SCT_GeometryManager* geometryManager,
                               SCT_MaterialManager* materials)
  : SCT_SharedComponentFactory(name, detectorManager, geometryManager, materials),
    m_iLayer(iLayer), m_length(length)
{
  getParameters();
  m_physVolume = build();
}


void 
SCT_SupportCyl::getParameters()
{
  const SCT_BarrelParameters * parameters = m_geometryManager->barrelParameters();
       
  m_materialName = parameters->supportCylMaterial(m_iLayer);
  m_innerRadius = parameters->supportCylInnerRadius(m_iLayer);
  m_outerRadius = parameters->supportCylOuterRadius(m_iLayer);

}

GeoVPhysVol * 
SCT_SupportCyl::build() 
{
  // Make the support cyliner. A simple tube.
  const GeoTube * supportCylShape = new GeoTube(m_innerRadius, m_outerRadius, 0.5 * m_length);
  m_material = m_materials->getMaterialForVolume(m_materialName, supportCylShape->volume());
  if (!m_material) m_material = m_materials->getMaterial(m_materialName);
  const GeoLogVol * supportCylLog = new GeoLogVol(getName(), supportCylShape, m_material);

  GeoPhysVol * supportCyl = new GeoPhysVol(supportCylLog);

  return supportCyl;
}
