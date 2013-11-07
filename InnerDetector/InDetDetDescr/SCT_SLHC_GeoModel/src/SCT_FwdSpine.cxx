/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_SLHC_GeoModel/SCT_FwdSpine.h"
#include "SCT_SLHC_GeoModel/SCT_MaterialManager.h"
#include "SCT_SLHC_GeoModel/SCT_GeometryManager.h"
#include "SCT_SLHC_GeoModel/SCT_ForwardModuleParameters.h"

#include "GeoModelKernel/GeoTrd.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoShape.h"
#include "GeoModelKernel/GeoShapeShift.h"
#include "GeoModelKernel/GeoShapeUnion.h"

#include "CLHEP/Units/SystemOfUnits.h"

#include "CLHEP/Geometry/Transform3D.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Vector/Rotation.h"

#include <cmath>

namespace InDetDDSLHC {


SCT_FwdSpine::SCT_FwdSpine(const std::string & name,
         int ring)
  : SCT_SharedComponentFactory(name), m_ring(ring){
    getParameters();
    m_physVolume = build();
  }



void SCT_FwdSpine::getParameters(){
  const SCT_ForwardModuleParameters* parameters = geometryManager()->forwardModuleParameters();
  
  m_materialName = parameters->fwdSpineMaterial(m_ring);
  
  //Main spine
  SCT_MaterialManager * materials = geometryManager()->materialManager();
  m_thickness     = parameters->fwdSpineThickness(m_ring);
  m_length        = parameters->fwdSpineLength(m_ring);
  m_middleRadius  = parameters->fwdSpineMiddleRadius(m_ring);
  m_deltaPhi      = parameters->fwdSpineDeltaPhi(m_ring);
  m_material      = materials->getMaterial(parameters->fwdSpineMaterial(m_ring));
}

GeoVPhysVol* SCT_FwdSpine::build(){
  // Build the spine. Just a simple box. Its orientated for building a module in a trapezoid 
  //so the spine has its length along the z axis, width along the y axis and thickness along 
  //the x axis.

  m_innerRadius = m_middleRadius - 0.5*m_length;
  m_outerRadius = m_middleRadius + 0.5*m_length;
  m_innerWidth  = 2*m_innerRadius*tan(0.5*m_deltaPhi);
  m_outerWidth  = 2*m_outerRadius*sin(0.5*m_deltaPhi);

  const GeoTrd* spineShape = new GeoTrd(0.5*m_thickness, 0.5*m_thickness,
					 0.5*m_innerWidth, 0.5*m_outerWidth,
					 0.5*m_length);
  //---SCT_MaterialManager materials;
  //---m_material = materials.getMaterial(m_materialName);
  GeoLogVol* spineLog = new GeoLogVol(getName(), spineShape, m_material);
  GeoPhysVol* spine = new GeoPhysVol(spineLog);

  return spine;
}
}
