/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_SLHC_GeoModel/SCT_OuterSide.h"
#include "SCT_SLHC_GeoModel/SCT_MaterialManager.h"
#include "SCT_SLHC_GeoModel/SCT_GeometryManager.h"
#include "SCT_SLHC_GeoModel/SCT_BarrelModuleParameters.h"
#include "SCT_SLHC_GeoModel/SCT_GeneralParameters.h"
#include "SCT_SLHC_GeoModel/SCT_Sensor.h"

#include "InDetReadoutGeometry/SCT_DetectorManager.h"

#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoIdentifierTag.h"
#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoShape.h"
#include "GeoModelKernel/GeoShapeUnion.h"
#include "GeoModelKernel/GeoShapeShift.h"

#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Geometry/Transform3D.h"
#include "CLHEP/Vector/ThreeVector.h" 

#include <cmath>

namespace InDetDDSLHC {

SCT_OuterSide::SCT_OuterSide(const std::string & name, int moduleType) 
  : SCT_UniqueComponentFactory(name),m_moduleType(moduleType),
    m_sensor(0){
    getParameters();
    m_logVolume = preBuild();
  }


SCT_OuterSide::~SCT_OuterSide(){
  // Clean up
  delete m_sensor;
}


void SCT_OuterSide::getParameters(){
  //const SCT_BarrelModuleParameters* parameters = geometryManager()->barrelModuleParameters();
  const SCT_GeneralParameters* generalParameters = geometryManager()->generalParameters();

  m_safety           = generalParameters->safety();
}
const GeoLogVol* SCT_OuterSide::preBuild(){
  SCT_MaterialManager * materials = geometryManager()->materialManager();

  //Create child components
  m_sensor             = new SCT_Sensor("BRLSensor", m_moduleType);

  //side dimensions (module dimensions before stereo rotation)
  m_width     = m_sensor->width();
  m_length    = m_sensor->length();						     
  m_thickness = m_sensor->thickness(); 
  // the depth axis goes from the implant side to the backside 
  // and so point to the module center.
  // The two sensor+hybrid combinations are built the same.
  //
  //                      ^ 
  //      ------- sensor  | x-axis
  //        ---   hybrid  | 
  //
  // Shown is the inner side. The outer side is the same but with a rotation 
  // of 180 deg around the z-axis.       
  //Outer side (ie nearest beam pipe)
  const GeoBox* outerSideShape = new GeoBox(0.5*m_thickness, 0.5*m_width, 0.5*m_length);
  const GeoLogVol* OuterSideLogV = new GeoLogVol(getName(), outerSideShape, 
						 materials->gasMaterial());
  //positions with respect to the center of the outer side box
  double outerSensorXPos = 0;
  //Position sensor
  m_sensorPos = new GeoTransform(HepGeom::TranslateX3D(outerSensorXPos));

  return OuterSideLogV;
}

GeoVPhysVol* SCT_OuterSide::build(SCT_Identifier id) const{

  std::cout<<"SCT OuterSide : "<<id.getPhiModule()<<" "<<id.getEtaModule()<<std::endl;

  GeoFullPhysVol* outerSide = new GeoFullPhysVol(m_logVolume);
  // Add Sensor
  //1000 to tell the Sensitive detector that it's a GeoModel geometry
  outerSide->add(new GeoNameTag("Sensor#1")); 
  outerSide->add(new GeoIdentifierTag(1000));
  outerSide->add(m_sensorPos);
  outerSide->add(m_sensor->build(id));

  return outerSide;
}
}
