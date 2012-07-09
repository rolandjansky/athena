/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
// 14th Aug 2005 S.Mima modified.
//
#include "SCT_GeoModel/SCT_Dogleg.h"

#include "SCT_GeoModel/SCT_GeometryManager.h"
#include "SCT_GeoModel/SCT_MaterialManager.h"

#include "SCT_GeoModel/SCT_BarrelParameters.h"

#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "CLHEP/Units/SystemOfUnits.h"

SCT_Dogleg::SCT_Dogleg(const std::string & name)
  : SCT_SharedComponentFactory(name)
{
  getParameters();
  m_physVolume = build();
}


void
SCT_Dogleg::getParameters()
{
  const SCT_BarrelParameters * parameters = geometryManager()->barrelParameters();
  SCT_MaterialManager materials;
    
  m_material  = materials.getMaterial(parameters->doglegMaterial());
  m_thickness = parameters->doglegThickness();
  m_length    = parameters->doglegLength();
  m_width     = parameters->doglegWidth();
}

GeoVPhysVol * 
SCT_Dogleg::build()
{

  // Build the Dogleg. Just a simple box.
  // Width is in z-direction.
  // Length is in phi-direction. 
  // NB. This is opposite to other elements I use.

  const GeoBox * doglegShape = new GeoBox(0.5*m_thickness, 0.5*m_length, 0.5*m_width);
  const GeoLogVol *doglegLog = 
    new GeoLogVol(getName(), doglegShape, m_material);
  GeoPhysVol * dogleg = new GeoPhysVol(doglegLog);

  return dogleg;
}
