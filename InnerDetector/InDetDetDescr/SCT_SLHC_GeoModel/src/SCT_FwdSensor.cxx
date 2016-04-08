/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_SLHC_GeoModel/SCT_FwdSensor.h"
#include "SCT_SLHC_GeoModel/SCT_GeometryManager.h"
#include "SCT_SLHC_GeoModel/SCT_MaterialManager.h"
#include "SCT_SLHC_GeoModel/SCT_ForwardModuleParameters.h"
#include "SCT_SLHC_GeoModel/SCT_Identifier.h"

#include "GeoModelKernel/GeoTrd.h"
#include "GeoModelKernel/GeoShape.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoShapeShift.h"
#include "GeoModelKernel/GeoShapeUnion.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoAlignableTransform.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "CLHEP/Units/SystemOfUnits.h"

#include "CLHEP/Geometry/Transform3D.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Vector/Rotation.h"

#include "InDetReadoutGeometry/SCT_DetectorManager.h"
#include "InDetReadoutGeometry/SCT_ForwardModuleSideDesign.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetReadoutGeometry/SiCommonItems.h"
#include "InDetReadoutGeometry/InDetDD_Defs.h"

#include <cmath>

using namespace InDetDD;

namespace InDetDDSLHC {

SCT_FwdSensor::SCT_FwdSensor(const std::string & name, int ring)
  : SCT_UniqueComponentFactory(name), m_ring(ring){
  getParameters();
  m_logVolume = preBuild();
}



void SCT_FwdSensor::getParameters(){
  
  const SCT_ForwardModuleParameters* parameters = geometryManager()->forwardModuleParameters();
  SCT_MaterialManager * materials = geometryManager()->materialManager();
  
  m_thickness     = parameters->fwdSensorThickness(m_ring);
  m_length        = parameters->fwdSensorLength(m_ring);
  m_innerWidth    = parameters->fwdSensorInnerWidth(m_ring);
  m_outerWidth    = parameters->fwdSensorOuterWidth(m_ring);
  m_middleRadius  = parameters->fwdSensorMiddleRadius(m_ring);
  m_deltaPhi      = parameters->fwdSensorDeltaPhi(m_ring);
  m_activeHalfLength  = parameters->fwdSensorActiveHalfLength(m_ring);
  // For truncated middle the sensor is offset from what it would be if it was a full middle.
  m_sensorOffset = 0;
  m_materialSensor  = materials->getMaterial(parameters->fwdSensorMaterial(m_ring));
  m_chargeCarrier      = parameters->fwdSensorChargeCarrier(m_ring);
}



const GeoLogVol* SCT_FwdSensor::preBuild(){

  m_innerRadius = m_middleRadius - 0.5*m_length;
  m_outerRadius = m_middleRadius + 0.5*m_length;
  //--m_innerWidth  = 2*m_innerRadius*tan(0.5*m_deltaPhi);
  //--m_outerWidth  = 2*m_outerRadius*sin(0.5*m_deltaPhi);

  const GeoTrd* sensorShape = new GeoTrd(0.5*m_thickness, 0.5*m_thickness,
					 0.5*m_innerWidth, 0.5*m_outerWidth,
					 0.5*m_length);
  const GeoLogVol* sensorLog = new GeoLogVol(getName(), sensorShape, m_materialSensor);
  // Make the moduleside design for this sensor
  makeDesign();

  detectorManager()->addDesign(m_design);

  return sensorLog;
}

void SCT_FwdSensor::makeDesign()
{
  // The designs require a name when put in the collection
  // but usually the design is accessed through SiDetectorElement
  // and so is not generally acessesed through the DesignCollection.

  // SCT_ForwardModuleSideDesign Constructor with parameters:
  //   local axis corresponding to eta direction
  //   local axis corresponding to phi direction
  //   local axis corresponding to depth direction
  //   thickness of silicon sensor
  //   number of crystals within module side
  //   number of diodes within crystal
  //   number of cells within module side
  //   index of diode connected to cell with index 0
  //   radius from inner crystal center to beam
  //   inner crystal half height
  //   radius from outer crystal (if present) center to beam
  //   outer crystal (if present) half height
  //   strip step in angle (same for both crystals)
  //   eta coordinate of crystal center
  //   phi coordinate of crystal center 

  const SCT_ForwardModuleParameters* parameters = geometryManager()->forwardModuleParameters();
 
  double radius1=0;
  double radius2=0;
  double halfHeight1=0;
  double halfHeight2=0;
  int crystals=0;
  InDetDD::CarrierType carrierType;
  if (m_chargeCarrier < 0.0) carrierType = InDetDD::electrons;
  else carrierType = InDetDD::holes;
  
  crystals = 1;
  radius1 = m_middleRadius;
  radius2 = 0.;
  halfHeight1 = m_activeHalfLength;
  halfHeight2 = 0.;

  
  double etaCenter = 0;
  double phiCenter = 0;
  //For consistency with older version we keep shift = 0.
  int cells  = parameters->fwdSensorNumReadoutStrips(m_ring);
  int diodes = cells;
  int shift  = 0;
     
  double step = parameters->fwdSensorAngularPitch(m_ring);

  // Readout direction is same direction as local phi direction for outer module
  // and the opposite direction for inner and middle module.
  bool swapStripReadout = false; // ie false for outer module only.

  // The readout side is at the +ve depth direction
  int readoutSide = +1;

  m_design = new SCT_ForwardModuleSideDesign(m_thickness,    
					     crystals, 
					     diodes, 
					     cells, 
					     shift,
					     swapStripReadout, 
					     carrierType,	     
					     radius1, 
					     halfHeight1, 
					     radius2, 
					     halfHeight2, 
					     step,
					     etaCenter, 
					     phiCenter,
					     readoutSide);

}



GeoVPhysVol*SCT_FwdSensor::build(SCT_Identifier id) const
{			
  GeoFullPhysVol* sensor = new GeoFullPhysVol(m_logVolume);
  
  // Make detector element and add to collection
  // Only do so if we have a valid id helper.
  //id.print(); // for debugging only

   SiCommonItems* commonItems =  geometryManager()->commonItems();

  if (commonItems->getIdHelper()) {


    SiDetectorElement* detElement = new SiDetectorElement(id.getWaferId(),
                                                           m_design,
                                                           sensor,
                                                           commonItems);

    // Add the detector element.
    detectorManager()->addDetectorElement(detElement);

  } else {
    static bool noElementWarning = true; // So we don't get the message thousands of times.
    if (noElementWarning) {
      std::cout << "WARNING!!!!: No SCT id helper and so no elements being produced." << std::endl;
      noElementWarning = false;
    }
  }
    
  return sensor;
}
}
