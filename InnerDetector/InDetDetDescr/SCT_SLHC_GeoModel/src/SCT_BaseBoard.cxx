/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_SLHC_GeoModel/SCT_BaseBoard.h"

#include "SCT_SLHC_GeoModel/SCT_MaterialManager.h"
#include "SCT_SLHC_GeoModel/SCT_GeometryManager.h"
#include "SCT_SLHC_GeoModel/SCT_BarrelModuleParameters.h"

#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "CLHEP/Units/SystemOfUnits.h"

namespace InDetDDSLHC {

SCT_BaseBoard::SCT_BaseBoard(const std::string & name, int moduleType)
  : SCT_SharedComponentFactory(name), m_moduleType(moduleType)
{
  getParameters();
  m_physVolume = build();
}


void SCT_BaseBoard::getParameters(){
  const SCT_BarrelModuleParameters * parameters = geometryManager()->barrelModuleParameters();
  SCT_MaterialManager * materials = geometryManager()->materialManager();
    
  m_thickness = parameters->baseBoardThickness(m_moduleType);
  m_length    = parameters->baseBoardLength(m_moduleType);
  m_width     = parameters->baseBoardWidth(m_moduleType);
  m_material  = materials->getMaterial(parameters->baseBoardMaterial(m_moduleType));
}

GeoVPhysVol* SCT_BaseBoard::build(){
  //Build the BaseBoard. Just a simple box.
  const GeoBox * baseBoardShape = new GeoBox(0.5*m_thickness, 0.5*m_width, 0.5*m_length);
  const GeoLogVol *baseBoardLog = new GeoLogVol(getName(), baseBoardShape, m_material);
  GeoPhysVol * baseBoard = new GeoPhysVol(baseBoardLog);
  
  return baseBoard;
}
}
