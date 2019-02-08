/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_SLHC_GeoModel/SCT_FwdDiscSupport.h"
#include "SCT_SLHC_GeoModel/SCT_MaterialManager.h"
#include "SCT_SLHC_GeoModel/SCT_GeometryManager.h"
#include "SCT_SLHC_GeoModel/SCT_ForwardParameters.h"

#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GaudiKernel/SystemOfUnits.h"

namespace InDetDDSLHC {

SCT_FwdDiscSupport::SCT_FwdDiscSupport(const std::string & name, int iWheel)
  : SCT_SharedComponentFactory(name), m_iWheel(iWheel){
    getParameters();
    m_physVolume = build();
}


void SCT_FwdDiscSupport::getParameters(){
  const SCT_ForwardParameters * parameters = geometryManager()->forwardParameters();
  
  SCT_MaterialManager * materials = geometryManager()->materialManager();
  m_thickness    = parameters->fwdDiscSupportThickness(m_iWheel);
  m_innerRadius  = parameters->fwdDiscSupportInnerRadius(m_iWheel);
  m_outerRadius  = parameters->fwdDiscSupportOuterRadius(m_iWheel);
  //m_material     = materials.getMaterial(parameters->fwdDiscSupportMaterial(m_iWheel));
  //0.1265 is taken from oracle database (DiskSupport)
  double materialIncreaseFactor = parameters->materialIncreaseFactor(m_iWheel);
  //double cf_density = 0.1265*materialIncreaseFactor*GeoModelKernelUnits::g/Gaudi::Units::cm3;
  //m_material = materials->getMaterial(parameters->fwdDiscSupportMaterial(m_iWheel), cf_density);
  m_material = materials->getMaterialScaled(parameters->fwdDiscSupportMaterial(m_iWheel), materialIncreaseFactor);
}

GeoVPhysVol* SCT_FwdDiscSupport::build(){
  // Make the support disk. A simple tube.
  
  const GeoTube* discSupportShape = new GeoTube(m_innerRadius, m_outerRadius, 0.5*m_thickness);
  const GeoLogVol* discSupportLog = new GeoLogVol(getName(), discSupportShape, m_material);
  
  GeoPhysVol* discSupport = new GeoPhysVol(discSupportLog);

  return discSupport;
}
}
