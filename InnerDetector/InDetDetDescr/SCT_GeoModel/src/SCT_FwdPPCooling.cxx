/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_GeoModel/SCT_FwdPPCooling.h"

#include "SCT_GeoModel/SCT_MaterialManager.h"

#include "SCT_GeoModel/SCT_GeometryManager.h"
#include "SCT_GeoModel/SCT_ForwardParameters.h"

#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "CLHEP/Units/SystemOfUnits.h"


SCT_FwdPPCooling::SCT_FwdPPCooling(const std::string & name)
  : SCT_SharedComponentFactory(name)
{
  getParameters();
  m_physVolume = build();
}


void 
SCT_FwdPPCooling::getParameters()
{
  const SCT_ForwardParameters * parameters = geometryManager()->forwardParameters();
        
  m_materialName= parameters->fwdPPCoolingMaterial();
  m_thickness   = parameters->fwdPPCoolingThickness();
  m_deltaR      = parameters->fwdPPCoolingDeltaR();
  m_rphi        = parameters->fwdPPCoolingRPhi();
}

GeoVPhysVol * 
SCT_FwdPPCooling::build() 
{

  SCT_MaterialManager materials;

  const GeoBox * pPCoolingShape = new GeoBox(0.5 * m_deltaR, 0.5 * m_rphi, 0.5 * m_thickness);
  m_material = materials.getMaterialForVolume(m_materialName, pPCoolingShape->volume());
  const GeoLogVol * pPCoolingLog = new GeoLogVol(getName(), pPCoolingShape, m_material);

  GeoPhysVol * pPCooling = new GeoPhysVol(pPCoolingLog);

  return pPCooling;
}


