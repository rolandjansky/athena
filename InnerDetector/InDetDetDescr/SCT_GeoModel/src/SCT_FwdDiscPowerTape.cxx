/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_GeoModel/SCT_FwdDiscPowerTape.h"

#include "SCT_GeoModel/SCT_MaterialManager.h"

#include "SCT_GeoModel/SCT_GeometryManager.h"
#include "SCT_GeoModel/SCT_ForwardParameters.h"

#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/Units.h"


SCT_FwdDiscPowerTape::SCT_FwdDiscPowerTape(const std::string & name, int iRing,
                                           InDetDD::SCT_DetectorManager* detectorManager,
                                           const SCT_GeometryManager* geometryManager,
                                           SCT_MaterialManager* materials)
  : SCT_SharedComponentFactory(name, detectorManager, geometryManager, materials),
    m_ringNumber(iRing) 
{
  getParameters();
  m_physVolume = build();
}


void 
SCT_FwdDiscPowerTape::getParameters()
{    
  const SCT_ForwardParameters * parameters = m_geometryManager->forwardParameters();
      
  m_materialName = parameters->fwdDiscPowerTapeMaterial(m_ringNumber);
  m_innerRadius  = parameters->fwdDiscPowerTapeInnerRadius(m_ringNumber);
  m_outerRadius  = parameters->fwdDiscPowerTapeOuterRadius(m_ringNumber);
  m_thickness    = parameters->fwdDiscPowerTapeThickness(m_ringNumber);

}

GeoVPhysVol * 
SCT_FwdDiscPowerTape::build() 
{
  // Make the support disk. A simple tube.
  const GeoTube * powerTapeShape = new GeoTube(m_innerRadius, m_outerRadius, 0.5 * m_thickness);
  m_material = m_materials->getMaterialForVolume(m_materialName, powerTapeShape->volume());
  const GeoLogVol * powerTapeLog = new GeoLogVol(getName(), powerTapeShape, m_material);

  GeoPhysVol * powerTape = new GeoPhysVol(powerTapeLog);

  return powerTape;
}
