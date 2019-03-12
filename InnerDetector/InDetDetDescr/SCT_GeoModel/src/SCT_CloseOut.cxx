/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_GeoModel/SCT_CloseOut.h"

#include "SCT_GeoModel/SCT_GeometryManager.h"
#include "SCT_GeoModel/SCT_MaterialManager.h"

#include "SCT_GeoModel/SCT_BarrelParameters.h"

#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/Units.h"


SCT_CloseOut::SCT_CloseOut(const std::string & name, int iLayer,
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
SCT_CloseOut::getParameters()
{
  const SCT_BarrelParameters * parameters = m_geometryManager->barrelParameters();
    
  m_material    = m_materials->getMaterial(parameters->closeOutMaterial(m_iLayer));
  m_length      = parameters->closeOutDeltaZ(m_iLayer);

  // Same inner and outer radius of support cylinder.
  m_outerRadius = parameters->supportCylOuterRadius(m_iLayer);
  m_innerRadius = parameters->supportCylInnerRadius(m_iLayer);

}

GeoVPhysVol * 
SCT_CloseOut::build() 
{
  // Make the closeOut. A simple tube.
  const GeoTube * closeOutShape = new GeoTube(m_innerRadius, m_outerRadius, 0.5 * m_length);
  const GeoLogVol * closeOutLog = new GeoLogVol(getName(), closeOutShape, m_material);

  GeoPhysVol * closeOut = new GeoPhysVol(closeOutLog);

  return closeOut;
}
