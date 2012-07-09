/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_GeoModel/SCT_FwdDiscSupport.h"

#include "SCT_GeoModel/SCT_MaterialManager.h"

#include "SCT_GeoModel/SCT_GeometryManager.h"
#include "SCT_GeoModel/SCT_ForwardParameters.h"

#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "CLHEP/Units/SystemOfUnits.h"

SCT_FwdDiscSupport::SCT_FwdDiscSupport(const std::string & name, int iWheel)
  : SCT_SharedComponentFactory(name), m_iWheel(iWheel)
{
  getParameters();
  m_physVolume = build();
}


void 
SCT_FwdDiscSupport::getParameters()
{
  const SCT_ForwardParameters * parameters = geometryManager()->forwardParameters();

  m_materialName = parameters->fwdDiscSupportMaterial();
  m_thickness   = parameters->fwdDiscSupportThickness();
  m_innerRadius = parameters->fwdDiscSupportInnerRadius();
  m_outerRadius = parameters->fwdDiscSupportOuterRadius();
}

GeoVPhysVol * 
SCT_FwdDiscSupport::build() 
{
  // Make the support disk. A simple tube.
  SCT_MaterialManager materials;

  const GeoTube * discSupportShape = new GeoTube(m_innerRadius, m_outerRadius, 0.5 * m_thickness);
  m_material = materials.getMaterialForVolume(m_materialName+intToString(m_iWheel), discSupportShape->volume());
  if(!m_material) {m_material = materials.getMaterial(m_materialName);}
  const GeoLogVol * discSupportLog = new GeoLogVol(getName(), discSupportShape, m_material);

  GeoPhysVol * discSupport = new GeoPhysVol(discSupportLog);

  return discSupport;
}
