/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_SLHC_GeoModel/SCT_InnerSide.h"
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

SCT_InnerSide::SCT_InnerSide(const std::string & name, int moduleType, bool staveLayout) 
  : SCT_UniqueComponentFactory(name),
    m_moduleType(moduleType),
    m_staveLayout(staveLayout),
    m_sensor(0)
{
  getParameters();
  m_logVolume = preBuild();
}


SCT_InnerSide::~SCT_InnerSide(){
  // Clean up
  delete m_sensor;
}


void SCT_InnerSide::getParameters(){
  //const SCT_BarrelModuleParameters* parameters = geometryManager()->barrelModuleParameters();
  const SCT_GeneralParameters* generalParameters = geometryManager()->generalParameters();

  m_safety           = generalParameters->safety();
}

const GeoLogVol* SCT_InnerSide::preBuild(){
  SCT_MaterialManager * materials = geometryManager()->materialManager();

  //Create child components
  m_sensor = new SCT_Sensor("BRLSensor", m_moduleType);

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
  //Inner side (ie nearest beam pipe)
  const GeoBox* innerSideShape = new GeoBox(0.5*m_thickness, 0.5*m_width, 0.5*m_length);
  const GeoLogVol* InnerSideLogV = new GeoLogVol(getName(), innerSideShape, 
						 materials->gasMaterial());
  //positions with respect to the center of the inner side box
  double innerSensorXPos = 0;
  //Position sensor
  m_sensorPos = new GeoTransform(HepGeom::TranslateX3D(innerSensorXPos));
  return InnerSideLogV;
}

GeoVPhysVol* SCT_InnerSide::build(SCT_Identifier id) const{

  //  std::cout<<"SCT InnerSide : "<<id.getPhiModule()<<" "<<id.getEtaModule()<<"  "<<std::endl;

  //  std::cout<<"InnerSide id: "<<id.getBarrelEC()<<" "<<id.getLayerDisk()<<" "<<id.getEtaModule()<<" "<<id.getPhiModule()<<" "<<id.getSide()<<" "<<id.getWaferId()<<std::endl;

  GeoFullPhysVol* innerSide = new GeoFullPhysVol(m_logVolume);
  // Add Sensor
  //1000 to tell the Sensitive detector that it's a GeoModel geometry
  //1100 to tell the Sensitive detector that the hierachy is stave orientated.
  int idTag = 1000;
  if (m_staveLayout) idTag = 1100;
  innerSide->add(new GeoNameTag("Sensor#0")); 
  innerSide->add(new GeoIdentifierTag(idTag));
  innerSide->add(m_sensorPos);
  innerSide->add(m_sensor->build(id));

  return innerSide;
}
}
