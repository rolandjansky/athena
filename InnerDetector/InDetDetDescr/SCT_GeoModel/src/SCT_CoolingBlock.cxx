/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_GeoModel/SCT_CoolingBlock.h"

#include "SCT_GeoModel/SCT_MaterialManager.h"

#include "SCT_GeoModel/SCT_GeometryManager.h"
#include "SCT_GeoModel/SCT_BarrelParameters.h"

#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/Units.h"

SCT_CoolingBlock::SCT_CoolingBlock(const std::string & name,
                                   InDetDD::SCT_DetectorManager* detectorManager,
                                   const SCT_GeometryManager* geometryManager,
                                   SCT_MaterialManager* materials)
  : SCT_SharedComponentFactory(name, detectorManager, geometryManager, materials)
{
  getParameters();
  m_physVolume = build();
}


void
SCT_CoolingBlock::getParameters()
{
  const SCT_BarrelParameters * parameters = m_geometryManager->barrelParameters();

  m_materialName  = parameters->coolingBlockMaterial();
  m_thickness = parameters->coolingBlockThickness();
  m_length    = parameters->coolingBlockLength();
  m_width     = parameters->coolingBlockWidth();
}

GeoVPhysVol * 
SCT_CoolingBlock::build()
{

  // Build the CoolingBlock. Just a simple box.
  // Width is in phi-direction.
  // Length is in z-direction. 

  const GeoBox * coolingBlockShape = new GeoBox(0.5*m_thickness, 0.5*m_width, 0.5*m_length);
  m_material = m_materials->getMaterialForVolume(m_materialName, coolingBlockShape->volume());
  const GeoLogVol *coolingBlockLog = 
    new GeoLogVol(getName(), coolingBlockShape, m_material);
  GeoPhysVol * coolingBlock = new GeoPhysVol(coolingBlockLog);

  return coolingBlock;
}
