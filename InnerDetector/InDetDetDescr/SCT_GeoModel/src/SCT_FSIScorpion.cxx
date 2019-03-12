/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_GeoModel/SCT_FSIScorpion.h"

#include "SCT_GeoModel/SCT_MaterialManager.h"

#include "SCT_GeoModel/SCT_GeometryManager.h"
#include "SCT_GeoModel/SCT_BarrelParameters.h"

#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/Units.h"

SCT_FSIScorpion::SCT_FSIScorpion(const std::string & name,
                                 InDetDD::SCT_DetectorManager* detectorManager,
                                 const SCT_GeometryManager* geometryManager,
                                 SCT_MaterialManager* materials)
  : SCT_SharedComponentFactory(name, detectorManager, geometryManager, materials)
{
  getParameters();
  m_physVolume = build();
}


void
SCT_FSIScorpion::getParameters()
{
  const SCT_BarrelParameters * parameters = m_geometryManager->barrelParameters();
  m_materialName  = parameters->fsiScorpionMaterial();
  m_radialWidth = parameters->fsiScorpionRadialWidth();
  m_rPhiWidth = parameters->fsiScorpionRPhiWidth();
  m_length    = parameters->fsiScorpionLength();
}

GeoVPhysVol * 
SCT_FSIScorpion::build()
{

  // Build the barrel FSI scorpion. Just a simple box.
  // Length is in z-direction.
  const GeoBox * scorpionShape = new GeoBox(0.5*m_radialWidth, 0.5*m_rPhiWidth, 0.5*m_length);
  m_material = m_materials->getMaterialForVolume(m_materialName, scorpionShape->volume());
  const GeoLogVol *scorpionLog = new GeoLogVol(getName(), scorpionShape, m_material);
  GeoPhysVol * scorpion = new GeoPhysVol(scorpionLog);

  return scorpion;
}
