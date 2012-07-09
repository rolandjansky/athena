/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
// 14th Aug 2005 S.Mima modified.
//
#include "SCT_GeoModel/SCT_Bracket.h"

#include "SCT_GeoModel/SCT_MaterialManager.h"

#include "SCT_GeoModel/SCT_GeometryManager.h"
#include "SCT_GeoModel/SCT_BarrelParameters.h"

#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "CLHEP/Units/SystemOfUnits.h"

SCT_Bracket::SCT_Bracket(const std::string & name)
  : SCT_SharedComponentFactory(name)
{
  getParameters();
  m_physVolume = build();
}


void
SCT_Bracket::getParameters()
{
  const SCT_BarrelParameters * parameters = geometryManager()->barrelParameters();
  SCT_MaterialManager materials;   
 
  m_material  = materials.getMaterial(parameters->bracketMaterial());
  m_thickness = parameters->bracketThickness();
  m_length    = parameters->bracketLength();
  m_width     = parameters->bracketWidth();
}

GeoVPhysVol * 
SCT_Bracket::build()
{

  // Build the Bracket. Just a simple box.
  // Width is in phi-direction.
  // Length is in z-direction. 

  const GeoBox * bracketShape = new GeoBox(0.5*m_thickness, 0.5*m_width, 0.5*m_length);
  const GeoLogVol *bracketLog = 
    new GeoLogVol(getName(), bracketShape, m_material);
  GeoPhysVol * bracket = new GeoPhysVol(bracketLog);

  return bracket;
}
