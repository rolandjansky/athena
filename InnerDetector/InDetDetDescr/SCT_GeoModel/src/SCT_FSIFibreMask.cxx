/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_GeoModel/SCT_FSIFibreMask.h"

#include "SCT_GeoModel/SCT_MaterialManager.h"

#include "SCT_GeoModel/SCT_GeometryManager.h"
#include "SCT_GeoModel/SCT_BarrelParameters.h"

#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/Units.h"


SCT_FSIFibreMask::SCT_FSIFibreMask(const std::string & name, int iLayer, double length,
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
SCT_FSIFibreMask::getParameters()
{
  const SCT_BarrelParameters * parameters = m_geometryManager->barrelParameters();

  m_materialName = parameters->fsiFibreMaskMaterial();
  m_outerRadius = parameters->supportCylInnerRadius(m_iLayer);
  m_innerRadius = m_outerRadius - parameters->fsiFibreMaskDeltaR();

}

GeoVPhysVol * 
SCT_FSIFibreMask::build() 
{
  // Make the support cyliner. A simple tube.
  const GeoTube * fibreMaskShape = new GeoTube(m_innerRadius, m_outerRadius, 0.5 * m_length);
  m_material = m_materials->getMaterialForVolume(m_materialName+intToString(m_iLayer), fibreMaskShape->volume());
  const GeoLogVol * fibreMaskLog = new GeoLogVol(getName(), fibreMaskShape, m_material);

  GeoPhysVol * fibreMask = new GeoPhysVol(fibreMaskLog);

  return fibreMask;
}
