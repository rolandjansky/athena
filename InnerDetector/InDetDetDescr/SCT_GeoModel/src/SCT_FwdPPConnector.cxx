/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_GeoModel/SCT_FwdPPConnector.h"

#include "SCT_GeoModel/SCT_MaterialManager.h"

#include "SCT_GeoModel/SCT_GeometryManager.h"
#include "SCT_GeoModel/SCT_ForwardParameters.h"

#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "CLHEP/Units/SystemOfUnits.h"


SCT_FwdPPConnector::SCT_FwdPPConnector(const std::string & name)
  : SCT_SharedComponentFactory(name)
{
  getParameters();
  m_physVolume = build();
}


void 
SCT_FwdPPConnector::getParameters()
{
  const SCT_ForwardParameters * parameters = geometryManager()->forwardParameters();
        
  m_materialName= parameters->fwdPPConnectorMaterial();
  m_thickness   = parameters->fwdPPConnectorThickness();
  m_deltaR      = parameters->fwdPPConnectorDeltaR();
  m_rphi        = parameters->fwdPPConnectorRPhi();
}

GeoVPhysVol * 
SCT_FwdPPConnector::build() 
{

  SCT_MaterialManager materials;

  const GeoBox * pPConnectorShape = new GeoBox(0.5 * m_deltaR, 0.5 * m_rphi, 0.5 * m_thickness);
  m_material = materials.getMaterialForVolume(m_materialName, pPConnectorShape->volume());
  const GeoLogVol * pPConnectorLog = new GeoLogVol(getName(), pPConnectorShape, m_material);

  GeoPhysVol * pPConnector = new GeoPhysVol(pPConnectorLog);

  return pPConnector;
}


