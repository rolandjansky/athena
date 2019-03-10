/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_GeoModel/SCT_FwdPPConnector.h"

#include "SCT_GeoModel/SCT_MaterialManager.h"

#include "SCT_GeoModel/SCT_GeometryManager.h"
#include "SCT_GeoModel/SCT_ForwardParameters.h"

#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/Units.h"


SCT_FwdPPConnector::SCT_FwdPPConnector(const std::string & name,
                                       InDetDD::SCT_DetectorManager* detectorManager,
                                       const SCT_GeometryManager* geometryManager,
                                       SCT_MaterialManager* materials)
  : SCT_SharedComponentFactory(name, detectorManager, geometryManager, materials)
{
  getParameters();
  m_physVolume = build();
}


void 
SCT_FwdPPConnector::getParameters()
{
  const SCT_ForwardParameters * parameters = m_geometryManager->forwardParameters();
        
  m_materialName= parameters->fwdPPConnectorMaterial();
  m_thickness   = parameters->fwdPPConnectorThickness();
  m_deltaR      = parameters->fwdPPConnectorDeltaR();
  m_rphi        = parameters->fwdPPConnectorRPhi();
}

GeoVPhysVol * 
SCT_FwdPPConnector::build() 
{
  const GeoBox * pPConnectorShape = new GeoBox(0.5 * m_deltaR, 0.5 * m_rphi, 0.5 * m_thickness);
  m_material = m_materials->getMaterialForVolume(m_materialName, pPConnectorShape->volume());
  const GeoLogVol * pPConnectorLog = new GeoLogVol(getName(), pPConnectorShape, m_material);

  GeoPhysVol * pPConnector = new GeoPhysVol(pPConnectorLog);

  return pPConnector;
}


