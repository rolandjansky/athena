/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_GeoModel/SCT_Harness.h"

#include "SCT_GeoModel/SCT_MaterialManager.h"

#include "SCT_GeoModel/SCT_GeometryManager.h"
#include "SCT_GeoModel/SCT_BarrelParameters.h"

#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/Units.h"

SCT_Harness::SCT_Harness(const std::string & name, double length,
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
SCT_Harness::getParameters()
{
  const SCT_BarrelParameters * parameters = m_geometryManager->barrelParameters();

  m_materialName  = parameters->harnessMaterial();
  m_thickness = parameters->harnessThickness();
  m_width     = parameters->harnessWidth();
}

GeoVPhysVol * 
SCT_Harness::build()
{

  // Build the Harness.
  const GeoBox * harnessShape = new GeoBox(0.5*m_thickness, 0.5*m_width, 0.5*m_length);
  m_material = m_materials->getMaterialForVolume(m_materialName, harnessShape->volume());
  if (!m_material) m_material = m_materials->getMaterial(m_materialName);
  const GeoLogVol *harnessLog = new GeoLogVol(getName(), harnessShape, m_material);
  GeoPhysVol * harness = new GeoPhysVol(harnessLog);

  return harness;
}
