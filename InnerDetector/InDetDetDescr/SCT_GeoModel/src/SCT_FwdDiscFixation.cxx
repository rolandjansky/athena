/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_GeoModel/SCT_FwdDiscFixation.h"

#include "SCT_GeoModel/SCT_MaterialManager.h"

#include "SCT_GeoModel/SCT_GeometryManager.h"
#include "SCT_GeoModel/SCT_ForwardParameters.h"

#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/Units.h"


SCT_FwdDiscFixation::SCT_FwdDiscFixation(const std::string & name,
                                         InDetDD::SCT_DetectorManager* detectorManager,
                                         const SCT_GeometryManager* geometryManager,
                                         SCT_MaterialManager* materials)
  : SCT_SharedComponentFactory(name, detectorManager, geometryManager, materials)
{
  getParameters();
  m_physVolume = build();
}


void 
SCT_FwdDiscFixation::getParameters()
{
  const SCT_ForwardParameters * parameters = m_geometryManager->forwardParameters();
        
  m_materialName= parameters->fwdDiscFixationMaterial();
  m_thickness   = parameters->fwdDiscFixationThickness();
  m_radius      = parameters->fwdDiscFixationRadius();
}

GeoVPhysVol * 
SCT_FwdDiscFixation::build() 
{
  const GeoTube * discFixationShape = new GeoTube(0., m_radius, 0.5*m_thickness);
  m_material = m_materials->getMaterialForVolume(m_materialName, discFixationShape->volume());
  const GeoLogVol * discFixationLog = new GeoLogVol(getName(), discFixationShape, m_material);

  GeoPhysVol * discFixation = new GeoPhysVol(discFixationLog);

  return discFixation;
}


