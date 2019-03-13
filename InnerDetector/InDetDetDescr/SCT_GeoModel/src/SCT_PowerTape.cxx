/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_GeoModel/SCT_PowerTape.h"

#include "SCT_GeoModel/SCT_MaterialManager.h"

#include "SCT_GeoModel/SCT_GeometryManager.h"
#include "SCT_GeoModel/SCT_BarrelParameters.h"

#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/Units.h"

SCT_PowerTape::SCT_PowerTape(const std::string & name, double length,
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
SCT_PowerTape::getParameters()
{
  const SCT_BarrelParameters * parameters = m_geometryManager->barrelParameters();
  m_material  = m_materials->getMaterial(parameters->powerTapeMaterial());
  m_thickness = parameters->powerTapeThickness();
  m_width     = parameters->powerTapeWidth();
}

GeoVPhysVol * 
SCT_PowerTape::build()
{

  // Build the PowerTape.
  const GeoBox * powerTapeShape = new GeoBox(0.5*m_thickness-epsilon(), 0.5*m_width-epsilon(), 0.5*m_length-epsilon());
  const GeoLogVol *powerTapeLog = 
    new GeoLogVol(getName(), powerTapeShape, m_material);
  GeoPhysVol * powerTape = new GeoPhysVol(powerTapeLog);

  return powerTape;
}
