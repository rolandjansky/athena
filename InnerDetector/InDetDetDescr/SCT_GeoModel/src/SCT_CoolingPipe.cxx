/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_GeoModel/SCT_CoolingPipe.h"

#include "SCT_GeoModel/SCT_MaterialManager.h"

#include "SCT_GeoModel/SCT_GeometryManager.h"
#include "SCT_GeoModel/SCT_BarrelParameters.h"

#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "CLHEP/Units/SystemOfUnits.h"

SCT_CoolingPipe::SCT_CoolingPipe(const std::string & name, double length)
  : SCT_SharedComponentFactory(name), m_length(length)
{
  getParameters();
  m_physVolume = build();
}


void
SCT_CoolingPipe::getParameters()
{
  const SCT_BarrelParameters * parameters = geometryManager()->barrelParameters();
    
  m_materialName  = parameters->coolingPipeMaterial();
  m_pipeRadius = parameters->coolingPipeRadius();
}

GeoVPhysVol * 
SCT_CoolingPipe::build()
{

  // Build the CoolingPipe.
  SCT_MaterialManager materials;

  const GeoTube * coolingPipeShape = new GeoTube(0., m_pipeRadius, 0.5*m_length);
  m_material = materials.getMaterialForVolume(m_materialName, coolingPipeShape->volume());
  if(!m_material) {m_material = materials.getMaterial(m_materialName);}
  const GeoLogVol *coolingPipeLog = 
    new GeoLogVol(getName(), coolingPipeShape, m_material);
  GeoPhysVol * coolingPipe = new GeoPhysVol(coolingPipeLog);

  return coolingPipe;
}
