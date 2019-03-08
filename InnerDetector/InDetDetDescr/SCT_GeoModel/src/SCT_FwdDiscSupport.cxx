/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_GeoModel/SCT_FwdDiscSupport.h"

#include "SCT_GeoModel/SCT_MaterialManager.h"

#include "SCT_GeoModel/SCT_GeometryManager.h"
#include "SCT_GeoModel/SCT_ForwardParameters.h"

#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/Units.h"

SCT_FwdDiscSupport::SCT_FwdDiscSupport(const std::string & name, int iWheel,
                                       InDetDD::SCT_DetectorManager* detectorManager,
                                       const SCT_GeometryManager* geometryManager,
                                       SCT_MaterialManager* materials)
  : SCT_SharedComponentFactory(name, detectorManager, geometryManager, materials),
    m_iWheel(iWheel)
{
  getParameters();
  m_physVolume = build();
}


void 
SCT_FwdDiscSupport::getParameters()
{
  const SCT_ForwardParameters * parameters = m_geometryManager->forwardParameters();

  m_materialName = parameters->fwdDiscSupportMaterial();
  m_thickness   = parameters->fwdDiscSupportThickness();
  m_innerRadius = parameters->fwdDiscSupportInnerRadius();
  m_outerRadius = parameters->fwdDiscSupportOuterRadius();
}

GeoVPhysVol * 
SCT_FwdDiscSupport::build() 
{
  // Make the support disk. A simple tube.
  const GeoTube * discSupportShape = new GeoTube(m_innerRadius, m_outerRadius, 0.5 * m_thickness);
  m_material = m_materials->getMaterialForVolume(m_materialName+intToString(m_iWheel), discSupportShape->volume());
  if(!m_material) m_material = m_materials->getMaterial(m_materialName);
  const GeoLogVol * discSupportLog = new GeoLogVol(getName(), discSupportShape, m_material);

  GeoPhysVol * discSupport = new GeoPhysVol(discSupportLog);

  return discSupport;
}
