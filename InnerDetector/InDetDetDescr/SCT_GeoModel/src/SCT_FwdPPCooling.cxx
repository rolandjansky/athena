/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_GeoModel/SCT_FwdPPCooling.h"

#include "SCT_GeoModel/SCT_MaterialManager.h"

#include "SCT_GeoModel/SCT_GeometryManager.h"
#include "SCT_GeoModel/SCT_ForwardParameters.h"

#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/Units.h"


SCT_FwdPPCooling::SCT_FwdPPCooling(const std::string & name,
                                   InDetDD::SCT_DetectorManager* detectorManager,
                                   const SCT_GeometryManager* geometryManager,
                                   SCT_MaterialManager* materials)
  : SCT_SharedComponentFactory(name, detectorManager, geometryManager, materials)
{
  getParameters();
  m_physVolume = build();
}


void 
SCT_FwdPPCooling::getParameters()
{
  const SCT_ForwardParameters * parameters = m_geometryManager->forwardParameters();
        
  m_materialName= parameters->fwdPPCoolingMaterial();
  m_thickness   = parameters->fwdPPCoolingThickness();
  m_deltaR      = parameters->fwdPPCoolingDeltaR();
  m_rphi        = parameters->fwdPPCoolingRPhi();
}

GeoVPhysVol * 
SCT_FwdPPCooling::build() 
{
  const GeoBox * pPCoolingShape = new GeoBox(0.5 * m_deltaR, 0.5 * m_rphi, 0.5 * m_thickness);
  m_material = m_materials->getMaterialForVolume(m_materialName, pPCoolingShape->volume());
  const GeoLogVol * pPCoolingLog = new GeoLogVol(getName(), pPCoolingShape, m_material);

  GeoPhysVol * pPCooling = new GeoPhysVol(pPCoolingLog);

  return pPCooling;
}


