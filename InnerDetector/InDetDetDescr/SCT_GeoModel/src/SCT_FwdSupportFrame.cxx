/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_GeoModel/SCT_FwdSupportFrame.h"

#include "SCT_GeoModel/SCT_MaterialManager.h"

#include "SCT_GeoModel/SCT_GeometryManager.h"
#include "SCT_GeoModel/SCT_ForwardParameters.h"

#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "CLHEP/Units/SystemOfUnits.h"


SCT_FwdSupportFrame::SCT_FwdSupportFrame(const std::string & name)
  : SCT_SharedComponentFactory(name)
{
  getParameters();
  m_physVolume = build();
}


void 
SCT_FwdSupportFrame::getParameters()
{
  const SCT_ForwardParameters * parameters = geometryManager()->forwardParameters();
     
  m_materialName = parameters->fwdSupportFrameMaterial();
  m_thickness    = parameters->fwdSupportFrameRadialThickness();
  m_innerRadius  = parameters->fwdSupportFrameInnerRadius();
  m_zMin         = parameters->fwdSupportFrameZMin();
  m_zMax         = parameters->fwdSupportFrameZMax();

  m_outerRadius = m_innerRadius + m_thickness;
  m_length = m_zMax - m_zMin;

}

GeoVPhysVol * 
SCT_FwdSupportFrame::build() 
{
  SCT_MaterialManager materials;

  // Make the support disk. A simple tube.
  const GeoTube * supportFrameShape = new GeoTube(m_innerRadius, m_outerRadius, 0.5 * m_length);
  m_material = materials.getMaterialForVolume(m_materialName, supportFrameShape->volume());
  if(!m_material) {m_material = materials.getMaterial(m_materialName);}
  const GeoLogVol * supportFrameLog = new GeoLogVol(getName(), supportFrameShape, m_material);

  GeoPhysVol * supportFrame = new GeoPhysVol(supportFrameLog);

  return supportFrame;
}
