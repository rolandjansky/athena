/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_GeoModel/SCT_Hybrid.h"

#include "SCT_GeoModel/SCT_MaterialManager.h"

#include "SCT_GeoModel/SCT_GeometryManager.h"
#include "SCT_GeoModel/SCT_BarrelModuleParameters.h"

#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/Units.h"

SCT_Hybrid::SCT_Hybrid(const std::string & name,
                       InDetDD::SCT_DetectorManager* detectorManager,
                       const SCT_GeometryManager* geometryManage,
                       SCT_MaterialManager* materials)
  : SCT_SharedComponentFactory(name, detectorManager, geometryManage, materials)
{
  getParameters();
  m_physVolume = build();
}

void
SCT_Hybrid::getParameters() 
{
  const SCT_BarrelModuleParameters * parameters = m_geometryManager->barrelModuleParameters();
  m_material  = m_materials->getMaterial(parameters->hybridMaterial());
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
