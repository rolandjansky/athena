/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////
// Simulation for the COMBINED TESTBEAM 2004 //
//                                           //
//       Zdenka.Broklova@matfyz.cz           //
//       Carlos.Escobar@ific.uv.es           //
//       Peter.Kodys@ific.uv.es              //
//        January 23, 2004                   //
///////////////////////////////////////////////

#include "SCT_GeoModel/SCT_FwdSensor.h"

#include "SCT_GeoModel/SCT_GeometryManager.h"
#include "SCT_GeoModel/SCT_MaterialManager.h"

#include "SCT_GeoModel/SCT_ForwardModuleParameters.h"

#include "SCT_GeoModel/SCT_Identifier.h"

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
#include "GaudiKernel/SystemOfUnits.h"

#include "GeoModelKernel/GeoDefinitions.h"

#include "InDetReadoutGeometry/SCT_DetectorManager.h"
#include "InDetReadoutGeometry/SCT_ForwardModuleSideDesign.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetReadoutGeometry/SiCommonItems.h"
#include "InDetReadoutGeometry/InDetDD_Defs.h"

#include <cmath>

using namespace InDetDD;


SCT_FwdSensor::SCT_FwdSensor(const std::string & name,
                             int ringType)
  : SCT_UniqueComponentFactory(name), m_ringType(ringType)
{
  getParameters();
  m_logVolume = preBuild();
}

SCT_FwdSensor::~SCT_FwdSensor() {
  if (m_inactive) m_inactive->unref();
}

void
SCT_FwdSensor::getParameters()
{

  const SCT_ForwardModuleParameters * parameters = geometryManager()->forwardModuleParameters();
  SCT_MaterialManager materials;

  m_materialSensor  = materials.getMaterial(parameters->fwdSensorMaterialFar(m_ringType));
  
 
  m_materialGlass  = 0;
  if (m_ringType == 2) { // Only need to define glass if its a Truncated middle module.
    m_materialGlass =  materials.getMaterial(parameters->fwdSensorMaterialNear(m_ringType));
  }
                  
  m_thicknessN = m_thicknessF = parameters->fwdSensorThickness(m_ringType);


  m_innerWidthF = parameters->fwdSensorInnerWidthFar(m_ringType);
  m_outerWidthF = parameters->fwdSensorOuterWidthFar(m_ringType);
  m_lengthF     = parameters->fwdSensorLengthFar(m_ringType);
  m_radiusF     = parameters->fwdSensorRadiusFar(m_ringType);

  m_innerWidthN = parameters->fwdSensorInnerWidthNear(m_ringType);
  m_outerWidthN = parameters->fwdSensorOuterWidthNear(m_ringType);
  m_lengthN     = parameters->fwdSensorLengthNear(m_ringType);
  m_radiusN     = parameters->fwdSensorRadiusNear(m_ringType);

  if (m_ringType == 3) {
    // For Inner Module only use number for far sensor.
    m_innerRadius = m_radiusF - 0.5 * m_lengthF;
    m_innerWidth = m_innerWidthF;
  } else {
    m_innerRadius = m_radiusN - 0.5 * m_lengthN;
    m_innerWidth = m_innerWidthN;
  }
  m_outerWidth = m_outerWidthF;
  m_outerRadius = m_radiusF + 0.5 * m_lengthF;

  m_activeHalfLengthFar  = parameters->fwdSensorActiveHalfLengthFar(m_ringType);
  m_activeHalfLengthNear = parameters->fwdSensorActiveHalfLengthNear(m_ringType);

  if (m_ringType == 3) { // Inner 
    m_sensorRadius = m_radiusF;
  } else {
    m_sensorRadius = 0.5 * (m_radiusF + m_activeHalfLengthFar  
                            + m_radiusN - m_activeHalfLengthNear);
  }

  // For truncated middle the sensor is offset from what it would be if it was a full middle.
  m_sensorOffset = 0;
  if (m_ringType == 2) { // truncated middle
    m_sensorOffset = m_radiusF - m_sensorRadius;
  }
  
  // The thickness of the two are the same, but to be pedantic.
  m_thickness = std::max(m_thicknessF, m_thicknessN);
}

const GeoLogVol * SCT_FwdSensor::preBuild()
{

  const GeoTrd * sensorShapeF = new GeoTrd(0.5 * m_thicknessF, 0.5 * m_thicknessF,
                                           0.5 * m_innerWidthF, 0.5 * m_outerWidthF,
                                           0.5 * m_lengthF);
  
  
  const GeoTrd * sensorShapeN= 0;
  if (m_ringType != 3) {
    sensorShapeN= new GeoTrd(0.5 * m_thicknessN, 0.5 * m_thicknessN,
                             0.5 * m_innerWidthN, 0.5 * m_outerWidthN,
                             0.5 * m_lengthN);
  }


  const GeoShape * sensorShape = 0;
  if ((m_ringType == 2) || (m_ringType == 3)) {
    // For truncated middle and inner there is only one wafer.
    sensorShape = sensorShapeF;
  } else {
    // For outer and middle there are two wafers. We 
    // define the sensor as a boolean volume of the two wafers.
    // relative position of near sensor 
    double positionNearZ = m_radiusN - m_sensorRadius;
    const GeoShape & sensorPosN = (*sensorShapeN<< GeoTrf::TranslateZ3D(positionNearZ)) ;
    // relative position of near sensor 
    double positionFarZ = m_radiusF - m_sensorRadius; 
    const GeoShape & sensorPosF = (*sensorShapeF<< GeoTrf::TranslateZ3D(positionFarZ) );
    sensorShape = &(sensorPosF.add(sensorPosN));
  }

  const GeoLogVol * sensorLog;
  sensorLog = new GeoLogVol(getName(), sensorShape, m_materialSensor);
  


  if (m_ringType == 2) {
    // Make inactive glass sensor. 
    double positionZ = m_radiusN - m_sensorRadius;
    const GeoShape & sensorPosN = (*sensorShapeN<< GeoTrf::TranslateZ3D(positionZ) );
    GeoLogVol * inactiveLog = new GeoLogVol(getName()+"Glass", &sensorPosN, m_materialGlass);
    m_inactive = new GeoPhysVol(inactiveLog);
    m_inactive->ref();
  } else {
    m_inactive = NULL;
  }

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

  // Design names are no longer used/needed, but might be used in the future for 
  // information purposes.
  /*
    std::string designName;
    switch (m_ringType) {
    case 0: // Outer
    designName = "SCT:ForwardRing1G3";
    break;
    case 1: // Middle
    designName = "SCT:ForwardRing2G3";
    break;
    case 2: // Truncated Middle
    designName = "SCT:ForwardRing3G3";
    break;
    case 3: // Inner
    designName = "SCT:ForwardRing4G3";
    break;
    default:
    break;
    }
  */

  // These can no longer be user defined and are ignored.
  //SiDetectorDesign::Axis etaAxis   = SiDetectorDesign::zAxis;
  //SiDetectorDesign::Axis phiAxis   = SiDetectorDesign::yAxis;
  //SiDetectorDesign::Axis depthAxis = SiDetectorDesign::xAxis;


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

  const SCT_ForwardModuleParameters * parameters = geometryManager()->forwardModuleParameters();
 
  double radius1=0;
  double radius2=0;
  double halfHeight1=0;
  double halfHeight2=0;
  int crystals=0;
  
  switch (m_ringType) {
  case 0: // Outer Module
  case 1: // Full Middle module
    crystals = 2;
    radius1 = m_radiusN;
    radius2 = m_radiusF;
    halfHeight1 = m_activeHalfLengthNear;
    halfHeight2 = m_activeHalfLengthFar;
    break;
  case 2: // Truncated Middle Module
  case 3: // Inner Module
    crystals = 1;
    radius1 = m_radiusF;
    radius2 = 0.;
    halfHeight1 = m_activeHalfLengthFar;
    halfHeight2 = 0.;
    break;
  default:
    std::cout << "ERROR!!!! SCT_FwdSensor: Invalid ring type" << std::endl;
  }
  
  double etaCenter = 0;
  double phiCenter = 0;

  //int cells  = parameters->fwdSensorNumReadoutStrips();
  //int diodes = parameters->fwdSensorNumStrips();
  //int shift  = parameters->fwdSensorStripShift();
  // For consistency with older version we keep shift = 0.
  int cells  = parameters->fwdSensorNumReadoutStrips(m_ringType);
  int diodes = cells;
  int shift  = 0;
     
  double step = parameters->fwdSensorAngularPitch(m_ringType);

  // Readout direction is same direction as local phi direction for outer module
  // and the opposite direction for inner and middle module.
  bool swapStripReadout = (m_ringType != 0); // ie false for outer module only.

  // The readout side is at the +ve depth direction
  int readoutSide = +1;

  m_design = new SCT_ForwardModuleSideDesign(m_thicknessN,    
                                             crystals, 
                                             diodes, 
                                             cells, 
                                             shift,
                                             swapStripReadout, 
                                             InDetDD::holes,
                                             radius1, 
                                             halfHeight1, 
                                             radius2, 
                                             halfHeight2, 
                                             step,
                                             etaCenter, 
                                             phiCenter,
                                             readoutSide);

  //
  // Flags to signal if axis can be swapped.
  // For trapezoid z axis (xEta) cannot be swapped. 
  // This is the default and no action is required. 
  // Can force axes not to be swapped by setting to false.
  // 
  // bool phiSyGaudi::Units::mmetric = true;
  // bool etaSyGaudi::Units::mmetric = false; 
  // bool depthSyGaudi::Units::mmetric = true;
  // m_design->setSyGaudi::Units::mmetry(phiSyGaudi::Units::mmetric, etaSyGaudi::Units::mmetric, depthSyGaudi::Units::mmetric,
  //

}



GeoVPhysVol *SCT_FwdSensor::build(SCT_Identifier id) const
{
    
  GeoFullPhysVol * sensor = new GeoFullPhysVol(m_logVolume);
  
  // Make detector element and add to collection
  // Only do so if we have a valid id helper.
  //id.print(); // for debugging only

  SiCommonItems* commonItems =  geometryManager()->commonItems();

  if (commonItems->getIdHelper()) {

    SiDetectorElement * detElement = new SiDetectorElement(id.getWaferId(),
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


