/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_GeoModel/SCT_PowerTape.h"

#include "SCT_GeoModel/SCT_MaterialManager.h"

#include "SCT_GeoModel/SCT_GeometryManager.h"
#include "SCT_GeoModel/SCT_BarrelParameters.h"

#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "CLHEP/Units/SystemOfUnits.h"

SCT_PowerTape::SCT_PowerTape(const std::string & name, double length)
  : SCT_SharedComponentFactory(name), m_length(length)
{
  getParameters();
  m_physVolume = build();
}


void
SCT_PowerTape::getParameters()
{
  const SCT_BarrelParameters * parameters = geometryManager()->barrelParameters();
  SCT_MaterialManager materials;
  
  // 30th Aug 2005 S.Mima modified.
  //  m_material  = materials.powerTapeMaterial();
  m_material  = materials.getMaterial(parameters->powerTapeMaterial());
  m_thickness = parameters->powerTapeThickness();
  m_width     = parameters->powerTapeWidth();
}

GeoVPhysVol * 
SCT_PowerTape::build()
{

  // Build the PowerTape.
  const GeoBox * powerTapeShape = new GeoBox(0.5*m_thickness-epsilon(), 0.5*m_width-epsilon(), 0.5*m_length-epsilon());
  const GeoLogVol *powerTapeLog = 
    new GeoLogVol(getName(), powerTapeShape, m_material);
  GeoPhysVol * powerTape = new GeoPhysVol(powerTapeLog);

  return powerTape;
}
