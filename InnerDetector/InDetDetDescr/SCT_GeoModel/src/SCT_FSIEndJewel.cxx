/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_GeoModel/SCT_FSIEndJewel.h"

#include "SCT_GeoModel/SCT_MaterialManager.h"

#include "SCT_GeoModel/SCT_GeometryManager.h"
#include "SCT_GeoModel/SCT_BarrelParameters.h"

#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/Units.h"

SCT_FSIEndJewel::SCT_FSIEndJewel(const std::string & name,
                                 InDetDD::SCT_DetectorManager* detectorManager,
                                 const SCT_GeometryManager* geometryManager,
                                 SCT_MaterialManager* materials)
  : SCT_SharedComponentFactory(name, detectorManager, geometryManager, materials)
{
  getParameters();
  m_physVolume = build();
}


void
SCT_FSIEndJewel::getParameters()
{
  const SCT_BarrelParameters * parameters = m_geometryManager->barrelParameters();
 
  m_materialName  = parameters->fsiEndJewelMaterial();
  m_radialWidth = parameters->fsiEndJewelRadialWidth();
  m_rPhiWidth = parameters->fsiEndJewelRPhiWidth();
  m_length    = parameters->fsiEndJewelLength();
}

GeoVPhysVol * 
SCT_FSIEndJewel::build()
{

  // Build the barrel FSI end jewel. Just a simple box.
  // Length is in z-direction. 
  const GeoBox * jewelShape = new GeoBox(0.5*m_radialWidth, 0.5*m_rPhiWidth, 0.5*m_length);
  m_material = m_materials->getMaterialForVolume(m_materialName, jewelShape->volume());
  const GeoLogVol *jewelLog = new GeoLogVol(getName(), jewelShape, m_material);
  GeoPhysVol * jewel = new GeoPhysVol(jewelLog);

  return jewel;
}
