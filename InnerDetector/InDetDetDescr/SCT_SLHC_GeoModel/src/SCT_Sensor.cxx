/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_SLHC_GeoModel/SCT_Sensor.h"

#include "SCT_SLHC_GeoModel/SCT_GeometryManager.h"
#include "SCT_SLHC_GeoModel/SCT_MaterialManager.h"

#include "SCT_SLHC_GeoModel/SCT_BarrelModuleParameters.h"

#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoIdentifierTag.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "InDetReadoutGeometry/SCT_DetectorManager.h"
#include "InDetReadoutGeometry/SCT_BarrelModuleSideDesign.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetReadoutGeometry/InDetDD_Defs.h"
#include "InDetReadoutGeometry/SiCommonItems.h"
#include "GaudiKernel/SystemOfUnits.h"

using namespace InDetDD;

namespace InDetDDSLHC {

SCT_Sensor::SCT_Sensor(const std::string & name, int moduleType)
  : SCT_UniqueComponentFactory(name), m_moduleType(moduleType)
{
  getParameters();
  m_logVolume = preBuild();
}

SCT_Sensor::~SCT_Sensor() {
  if (m_subSensorLog) m_subSensorLog->unref();
}

void
SCT_Sensor::getParameters()
{

  const SCT_BarrelModuleParameters * parameters = geometryManager()->barrelModuleParameters();
  SCT_MaterialManager * materials = geometryManager()->materialManager();    

  m_thickness   = parameters->sensorThickness(m_moduleType);
  m_length      = parameters->sensorLength(m_moduleType);
  m_width       = parameters->sensorWidth(m_moduleType);
  m_numSegments = parameters->barrelModelSideSegments(m_moduleType);
  m_segmentGap  = parameters->barrelModelSideSegmentGap(m_moduleType);
  m_stripLength = parameters->barrelModelSideStripLength(m_moduleType);
  m_material    = materials->getMaterial(parameters->sensorMaterial(m_moduleType));
  m_chargeCarrier = parameters->chargeCarrier(m_moduleType);
}

const GeoLogVol * 
SCT_Sensor::preBuild()
{

  std::string logVolName = getName();
  m_subSensorLength = 0;
  m_subSensorLog = 0;
  // If more than one segment then the sensor becomes the mother volume of 
  // subsensors. The subsensor will be the sensitive detectors, so we need to
  // rename the parent volume
  if (m_numSegments > 1) {
    logVolName = "BrlMultiSensor";
    m_subSensorLength = m_stripLength + m_segmentGap;
    if (m_subSensorLength * m_numSegments > m_length) {
      geometryManager()->msg(MSG::ERROR) << "Total length of sub sensor segments is larger than sensor."
					 << " Sensor length = " << m_length
					 << ", Total sub sensor length = " << m_subSensorLength * m_numSegments << endmsg;
    }  
    
    // Build the subsensor logical volume (same for all segments).
    // We reduce the size by a small amount to avoid touching volumes. 
    double epsilon = 1e-7*Gaudi::Units::mm;
    const GeoBox * subSensorShape = new GeoBox(0.5*m_thickness-epsilon, 0.5*m_width-epsilon, 0.5*m_subSensorLength-epsilon);
    m_subSensorLog = new GeoLogVol(getName(), subSensorShape, m_material);  
    m_subSensorLog->ref();
  }

  // Build the sensor. Just a simple box.
  const GeoBox * sensorShape = new GeoBox(0.5*m_thickness, 0.5*m_width, 0.5*m_length);
  GeoLogVol * sensorLog = new GeoLogVol(logVolName, sensorShape, m_material);

  // Make the moduleside design for this sensor
  makeDesign();

  detectorManager()->addDesign(m_design);

  return sensorLog;
}


void
SCT_Sensor::makeDesign()
{
  const SCT_BarrelModuleParameters * parameters = geometryManager()->barrelModuleParameters();
  
  double stripPitch      = parameters->barrelModelSideStripPitch(m_moduleType);
  double stripLength     = m_stripLength;
  double totalDeadLength = parameters->barrelModelSideTotalDeadLength(m_moduleType);
  int crystals           = parameters->barrelModelSideCrystals(m_moduleType);
  int diodes             = parameters->barrelModelSideDiodes(m_moduleType);
  int cells              = parameters->barrelModelSideCells(m_moduleType);
  int shift              = parameters->barrelModelSideShift(m_moduleType); 
  InDetDD::CarrierType carrierType;
  if (m_chargeCarrier < 0.0) carrierType = InDetDD::electrons;
  else carrierType = InDetDD::holes;

  double xEtaStripPatternCenter = 0;
  double xPhiStripPatternCenter = 0;

  // Readout direction is opposite to local phi direction
  // (This is only useful for the new modules, for G3 Modules the two sides are the 
  // same direction so one will always be wrong unless we make separate designs for each side,
  // We leave it as false though for the G3 module)
  // 
  bool swapStripReadout = false;
// The readout side is at the +ve depth direction
  int readoutSide = +1;
  m_design = new SCT_BarrelModuleSideDesign(m_thickness,
					    crystals,
					    diodes,
					    cells,
					    shift,
					    swapStripReadout,
					    carrierType,
					    stripPitch,
					    stripLength,
					    xEtaStripPatternCenter,
					    xPhiStripPatternCenter,
					    totalDeadLength,
					    readoutSide);
}


GeoVPhysVol* SCT_Sensor::build(SCT_Identifier id) const{
  GeoFullPhysVol * sensor = new GeoFullPhysVol(m_logVolume); 

  // If more than one segment we add daughter volumes corresponding to the segments and 
  // create a detector element for each segment.
  GeoFullPhysVol * subSensor = sensor;

  int etaModuleFirst = id.getEtaModule();
  
  for (int iSegment = 0; iSegment < m_numSegments; iSegment++) {
    if (m_numSegments > 1) {
      double zpos = (iSegment - 0.5*(m_numSegments-1)) * m_subSensorLength;
      subSensor = new GeoFullPhysVol(m_subSensorLog);
      GeoTransform * trans = new GeoTransform(GeoTrf::TranslateZ3D(zpos));
      GeoIdentifierTag * idTag = new GeoIdentifierTag(1200+iSegment);
      GeoNameTag * nameTag = new GeoNameTag("SubSensor#"+intToString(iSegment));
      sensor->add(trans);
      sensor->add(nameTag);
      sensor->add(idTag);
      sensor->add(subSensor);
    }

    // Make detector element and add to collection
    //Only do so if we have a valid id helper.
    SiCommonItems* commonItems =  geometryManager()->commonItems();
    if (commonItems->getIdHelper()) {
  
      id.setEtaModule(etaModuleFirst + iSegment);

      SiDetectorElement * detElement;
      
      //{ // DEBUG printout
      //  int side = id.getSide();
      //  int sign = (etaModuleFirst >= 0) ? 1: -1;
      //  int moduleTag = etaModuleFirst*2+sign*side;
      //  std::cout << "ModuleTag: " << moduleTag << std::endl;
      //  std::cout << "Sensor Copy num: " << 1200+iSegment << std::endl;
      //  // decode 
      //  int sideDecode = std::abs(moduleTag)%2;
      //  int etaModDecode = sign * std::abs(moduleTag)/2 + iSegment;
      //  std::cout << "Eta/side decode " << etaModDecode << " " << sideDecode << std::endl;
      //  id.print();
      //}
      //if(id.getBarrelEC()==0&&id.getLayerDisk()==0&&id.getEtaModule()<3&&id.getEtaModule()>-1&&id.getPhiModule()<2)
      //std::cerr<<"id: "<<id.getBarrelEC()<<" "<<id.getLayerDisk()<<" "<<id.getEtaModule()<<" "<<id.getPhiModule()<<" "<<id.getSide()<<" "<<id.getWaferId()<<std::endl;

      detElement =  new SiDetectorElement(id.getWaferId(), 
					  m_design, 
					  subSensor,  
					  commonItems);

      // Add the detector element.
      detectorManager()->addDetectorElement(detElement);

    }else{
      static bool noElementWarning = true; // So we don't get the message thousands of times.
      if (noElementWarning) {
	std::cout<<"SCT_Sensor, WARNING: No SCT id helper and so no elements being produced" 
		 << std::endl;
	noElementWarning = false;
      }
    }
  }
  return sensor;
}
}
