/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_GeoModel/SCT_Hybrid.h"

#include "SCT_GeoModel/SCT_MaterialManager.h"

#include "SCT_GeoModel/SCT_GeometryManager.h"
#include "SCT_GeoModel/SCT_BarrelModuleParameters.h"

#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "CLHEP/Units/SystemOfUnits.h"

SCT_Hybrid::SCT_Hybrid(const std::string & name)
  : SCT_SharedComponentFactory(name)
{
  getParameters();
  m_physVolume = build();
}

void
SCT_Hybrid::getParameters() 
{
  const SCT_BarrelModuleParameters * parameters = geometryManager()->barrelModuleParameters();
  SCT_MaterialManager materials;
 
  m_material  = materials.getMaterial(parameters->hybridMaterial());
  m_thickness = parameters->hybridThickness();
  m_length    = parameters->hybridLength();
  m_width     = parameters->hybridWidth();
}

GeoVPhysVol * 
SCT_Hybrid::build()
{
  // Build the . Just a simple box.
  const GeoBox * simpleHybridShape = new GeoBox(0.5*m_thickness,
						0.5*m_width,
						0.5*m_length);
  const GeoLogVol * simpleHybridLog = 
    new GeoLogVol(getName(), simpleHybridShape, m_material);
  GeoPhysVol * simpleHybrid = new GeoPhysVol(simpleHybridLog);

  return simpleHybrid;
}
