/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_GeoModel/SCT_CoolingPipe.h"

#include "SCT_GeoModel/SCT_MaterialManager.h"

#include "SCT_GeoModel/SCT_GeometryManager.h"
#include "SCT_GeoModel/SCT_BarrelParameters.h"

#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/Units.h"

SCT_CoolingPipe::SCT_CoolingPipe(const std::string & name, double length,
                                 InDetDD::SCT_DetectorManager* detectorManager,
                                 const SCT_GeometryManager* geometryManager,
                                 SCT_MaterialManager* materials)
  : SCT_SharedComponentFactory(name, detectorManager, geometryManager, materials),
    m_length(length)
{
  getParameters();
  m_physVolume = build();
}


void
SCT_CoolingPipe::getParameters()
{
  const SCT_BarrelParameters * parameters = m_geometryManager->barrelParameters();
    
  m_materialName  = parameters->coolingPipeMaterial();
  m_pipeRadius = parameters->coolingPipeRadius();
}

GeoVPhysVol * 
SCT_CoolingPipe::build()
{

  // Build the CoolingPipe.
  const GeoTube * coolingPipeShape = new GeoTube(0., m_pipeRadius, 0.5*m_length);
  m_material = m_materials->getMaterialForVolume(m_materialName, coolingPipeShape->volume());
  if(!m_material) m_material = m_materials->getMaterial(m_materialName);
  const GeoLogVol *coolingPipeLog = 
    new GeoLogVol(getName(), coolingPipeShape, m_material);
  GeoPhysVol * coolingPipe = new GeoPhysVol(coolingPipeLog);

  return coolingPipe;
}
