/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_SLHC_GeoModel/SCT_StaveSupport.h"

#include "SCT_SLHC_GeoModel/SCT_MaterialManager.h"
#include "SCT_SLHC_GeoModel/SCT_GeometryManager.h"
#include "SCT_SLHC_GeoModel/SCT_BarrelParameters.h"

#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "CLHEP/Units/SystemOfUnits.h"

namespace InDetDDSLHC {

SCT_StaveSupport::SCT_StaveSupport(const std::string & name, int iLayer, double length)
  : SCT_SharedComponentFactory(name), 
    m_length(length),
    m_iLayer(iLayer)
{
  getParameters();
  m_physVolume = build();
}


void SCT_StaveSupport::getParameters(){
  const SCT_BarrelParameters * parameters = geometryManager()->barrelParameters();
  SCT_MaterialManager * materials = geometryManager()->materialManager();
    
  m_thickness = parameters->staveSupportThickness(m_iLayer);
  m_width     = parameters->staveSupportWidth(m_iLayer);
  m_material  = materials->getMaterial(parameters->staveSupportMaterial(m_iLayer));
}

GeoVPhysVol* SCT_StaveSupport::build(){
  //Build the StaveSupport. Just a simple box.
  // If thickness is 0 then do not build
  GeoPhysVol * support = 0;
  if (m_thickness > 0) {
    const GeoBox * supportShape = new GeoBox(0.5*m_thickness, 0.5*m_width, 0.5*m_length);
    const GeoLogVol *supportLog = new GeoLogVol(getName(), supportShape, m_material);
    support = new GeoPhysVol(supportLog);
  }
  return support;
}
}
