/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
#include "GeoModelKernel/Units.h"

SCT_Bracket::SCT_Bracket(const std::string & name,
                         InDetDD::SCT_DetectorManager* detectorManager,
                         const SCT_GeometryManager* geometryManager,
                         SCT_MaterialManager* materials)
  : SCT_SharedComponentFactory(name, detectorManager, geometryManager, materials)
{
  getParameters();
  m_physVolume = build();
}


void
SCT_Bracket::getParameters()
{
  const SCT_BarrelParameters * parameters = m_geometryManager->barrelParameters();
 
  m_material  = m_materials->getMaterial(parameters->bracketMaterial());
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
  const GeoLogVol *bracketLog = new GeoLogVol(getName(), bracketShape, m_material);
  GeoPhysVol * bracket = new GeoPhysVol(bracketLog);

  return bracket;
}
