/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_GeoModel/SCT_FwdDiscFixation.h"

#include "SCT_GeoModel/SCT_MaterialManager.h"

#include "SCT_GeoModel/SCT_GeometryManager.h"
#include "SCT_GeoModel/SCT_ForwardParameters.h"

#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "CLHEP/Units/SystemOfUnits.h"


SCT_FwdDiscFixation::SCT_FwdDiscFixation(const std::string & name)
  : SCT_SharedComponentFactory(name)
{
  getParameters();
  m_physVolume = build();
}


void 
SCT_FwdDiscFixation::getParameters()
{
  const SCT_ForwardParameters * parameters = geometryManager()->forwardParameters();
        
  m_materialName= parameters->fwdDiscFixationMaterial();
  m_thickness   = parameters->fwdDiscFixationThickness();
  m_radius      = parameters->fwdDiscFixationRadius();
}

GeoVPhysVol * 
SCT_FwdDiscFixation::build() 
{

  SCT_MaterialManager materials;

  const GeoTube * discFixationShape = new GeoTube(0., m_radius, 0.5*m_thickness);
  m_material = materials.getMaterialForVolume(m_materialName, discFixationShape->volume());
  const GeoLogVol * discFixationLog = new GeoLogVol(getName(), discFixationShape, m_material);

  GeoPhysVol * discFixation = new GeoPhysVol(discFixationLog);

  return discFixation;
}


