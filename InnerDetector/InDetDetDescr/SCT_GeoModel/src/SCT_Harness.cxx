/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_GeoModel/SCT_Harness.h"

#include "SCT_GeoModel/SCT_MaterialManager.h"

#include "SCT_GeoModel/SCT_GeometryManager.h"
#include "SCT_GeoModel/SCT_BarrelParameters.h"

#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "CLHEP/Units/SystemOfUnits.h"

SCT_Harness::SCT_Harness(const std::string & name, double length)
  : SCT_SharedComponentFactory(name), m_length(length)
{
  getParameters();
  m_physVolume = build();
}


void
SCT_Harness::getParameters()
{
  const SCT_BarrelParameters * parameters = geometryManager()->barrelParameters();
  
  m_materialName  = parameters->harnessMaterial();
  m_thickness = parameters->harnessThickness();
  m_width     = parameters->harnessWidth();
}

GeoVPhysVol * 
SCT_Harness::build()
{

  // Build the Harness.
  SCT_MaterialManager materials;

  const GeoBox * harnessShape = new GeoBox(0.5*m_thickness, 0.5*m_width, 0.5*m_length);
  m_material = materials.getMaterialForVolume(m_materialName, harnessShape->volume());
  if(!m_material) {m_material = materials.getMaterial(m_materialName);}
  const GeoLogVol *harnessLog = 
    new GeoLogVol(getName(), harnessShape, m_material);
  GeoPhysVol * harness = new GeoPhysVol(harnessLog);

  return harness;
}
