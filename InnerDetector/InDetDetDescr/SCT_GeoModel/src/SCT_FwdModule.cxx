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

#include "SCT_GeoModel/SCT_FwdModule.h"
#include "SCT_GeoModel/SCT_GeometryManager.h"
#include "SCT_GeoModel/SCT_MaterialManager.h"

#include "SCT_GeoModel/SCT_ForwardModuleParameters.h"
#include "SCT_GeoModel/SCT_GeneralParameters.h"
#include "SCT_GeoModel/SCT_Identifier.h"

#include "SCT_GeoModel/SCT_FwdModuleConnector.h"
#include "SCT_GeoModel/SCT_FwdSpine.h"
#include "SCT_GeoModel/SCT_FwdSubSpine.h"
#include "SCT_GeoModel/SCT_FwdHybrid.h"

#include "InDetReadoutGeometry/SCT_DetectorManager.h"

#include "GeoModelKernel/GeoTrd.h"
#include "GeoModelKernel/GeoShapeShift.h"
#include "GeoModelKernel/GeoShape.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoIdentifierTag.h"
#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoAlignableTransform.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoDefinitions.h"
#include "GaudiKernel/SystemOfUnits.h"

#include <cmath>
#include <sstream>

inline double sqr(double x) {return x*x;}

SCT_FwdModule::SCT_FwdModule(const std::string & name, int ringType)
  : SCT_UniqueComponentFactory(name), m_ringType(ringType)
{

  getParameters();

  m_hybrid = new SCT_FwdHybrid("SCT_FwdHybrid"+intToString(ringType), m_ringType );
  m_spine  = new SCT_FwdSpine("SCT_FwdSpine"+intToString(ringType), m_ringType);
  m_subspineL  = new SCT_FwdSubSpine("SCT_FwdSubSpineL"+intToString(ringType), m_ringType, SUBSPINE_LEFT);
  m_subspineR  = new SCT_FwdSubSpine("SCT_FwdSubSpineR"+intToString(ringType), m_ringType, SUBSPINE_RIGHT);
  m_sensor = new SCT_FwdSensor("ECSensor"+intToString(ringType), m_ringType);
  if (m_connectorPresent) {
    m_connector = new SCT_FwdModuleConnector("SCT_FwdModuleConnector"+intToString(ringType), m_ringType);
  }
  else {
    m_connector = NULL;
  }

  m_logVolume = preBuild();

}

SCT_FwdModule::~SCT_FwdModule()
{
  delete m_connector;
  delete m_hybrid;
  delete m_spine;
  delete m_subspineL;
  delete m_subspineR;
  delete m_sensor;
}
  

void 
SCT_FwdModule::getParameters()
{
  const SCT_ForwardModuleParameters * parameters = geometryManager()->forwardModuleParameters();
  m_glueThickness = parameters->fwdModuleGlueThickness(m_ringType);
  m_distBtwMountPoints = parameters->fwdModuleDistBtwMountPoints(m_ringType);
  m_mountPointToCenter = parameters->fwdModuleMountPoint(m_ringType);
  m_hybridIsOnInnerEdge =  parameters->fwdHybridIsOnInnerEdge(m_ringType);
  m_stereoAngle = parameters->fwdModuleStereoAngle(m_ringType);
  m_upperSide   = parameters->fwdModuleUpperSideNumber(m_ringType);
  m_connectorPresent = parameters->fwdModuleConnectorPresent();
     
}


const GeoLogVol * SCT_FwdModule::preBuild()
{  
  // module volume preparing 

  const SCT_GeneralParameters * generalParameters = geometryManager()->generalParameters();
  
  double safety = generalParameters->safety();
  double safetyTmp = safety * Gaudi::Units::cm; // For compatibility with minor bug in older version - safety already in CLHEP units;

  // module_length = (zhyb->hyby - zhyb->hybysh + zsmi[m_ringType].mountd2 + 0.33 ) * Gaudi::Units::cm + safety;
  // Distance from outer bybrid edge to outer spine edge.
  // FIXME: The 1.05Gaudi::Units::mm is not needed
  double moduleLength = m_hybrid->mountPointToOuterEdge() + m_mountPointToCenter + m_spine->moduleCenterToEnd() + 1.05 * Gaudi::Units::mm;
  m_length = moduleLength + safety; // We add a bit of safety for the envelope

  //  module_thickness = (zhyb->hybz0 * 2 + safety) * Gaudi::Units::cm;
  double sensorEnvelopeThickness = 2 * m_sensor->thickness() + m_spine->thickness() + 2 * m_glueThickness;
  m_thickness = std::max(sensorEnvelopeThickness,  m_hybrid->thickness());

  // module_widthInner = ((zsmo->subdq + zssp[m_ringType].ssp0l + 0.325) * 2.+ 0.7 + safety)*Gaudi::Units::cm;   // upto to NOVA_760
  // module_widthOuter = ((zsmo->subdq + zssp[m_ringType].ssp2l + 0.325) * 2.+ 0.7 + safety)*Gaudi::Units::cm;   // upto to NOVA_760
  
  //module_widthInner = ((zsmo->subdq + zssp[m_ringType].ssp0l) * 2.+ 0.7 + safety)*Gaudi::Units::cm;
  //module_widthOuter = ((zsmo->subdq + zssp[m_ringType].ssp2l) * 2.+ 0.7 + safety)*Gaudi::Units::cm;
  
  m_widthInner = (m_spine->width() + 2 * m_subspineL->innerWidth() + 0.7*Gaudi::Units::cm) + safetyTmp; 
  m_widthOuter = (m_spine->width() + 2 * m_subspineL->outerWidth() + 0.7*Gaudi::Units::cm) + safetyTmp; 
  


  if (m_ringType == 3 ) {
    //  module_widthOuter = (( zsmo->subdq + zssp[m_ringType].ssp2l + 0.325) * 2.+ 1.6 + safety)*Gaudi::Units::cm;  // upto to NOVA_760
    //  module_widthOuter = (( zsmo->subdq + zssp[m_ringType].ssp2l) * 2.+ 1.6 + safety)*Gaudi::Units::cm; 
    m_widthOuter = m_spine->width() + 2 * m_subspineL->outerWidth() + 1.6*Gaudi::Units::cm + safetyTmp; 
  }  
    
  // Calculate module shift. Distance between module physics center and center of envelope.  
  int hybridSign =  m_hybridIsOnInnerEdge ? +1: -1;
  //module_shift = (zhyb->hyby - zhyb->hybysh + zsmi[m_ringType].mountd + 0.05)*Gaudi::Units::cm;
  //module_shift = hybrid * (module_length / 2. - module_shift);

  double moduleCenterToHybridOuterEdge = m_hybrid->mountPointToOuterEdge() + m_mountPointToCenter + 0.5*Gaudi::Units::mm;
  //FIXME: Should be: (ie don't need the 0.5Gaudi::Units::mm)
  // double moduleCenterToHybridOuterEdge = m_hybrid->mountPointToOuterEdge() + m_mountPointToCenter ;
  m_moduleShift = hybridSign * (0.5 * m_length - moduleCenterToHybridOuterEdge);

  // Envelope inner/outer radius
  m_innerRadius = m_sensor->centerRadius() + m_moduleShift - 0.5*m_length;
  m_outerRadius = m_sensor->centerRadius() + m_moduleShift + 0.5*m_length;
  
  // Radial location of mount points (ignoring streo rotation)
  m_mainMountPoint = m_sensor->centerRadius() - hybridSign * m_mountPointToCenter;
  m_secMountPoint =  m_mainMountPoint +  hybridSign * m_distBtwMountPoints;
  m_endLocator =  m_sensor->centerRadius() + hybridSign * m_spine->moduleCenterToEnd();

  // Outer module the hybrid is on inner edge.
  // For the rest its in the outer edge.
  // TODO Check this.
  m_powerTapeStart =  m_sensor->centerRadius() - hybridSign * moduleCenterToHybridOuterEdge;
 

  const GeoTrd * moduleEnvelopeShape = new GeoTrd(0.5 * m_thickness, 0.5 * m_thickness,
                                                  0.5 * m_widthInner, 0.5 * m_widthOuter,  
                                                  0.5 * m_length);
  const GeoShapeShift & moduleEnvelope = (*moduleEnvelopeShape << GeoTrf::TranslateZ3D(m_moduleShift) );
  
  SCT_MaterialManager materials;
  GeoLogVol * moduleLog =  new GeoLogVol(getName(), &moduleEnvelope, materials.gasMaterial());
     
  return moduleLog;

}

GeoVPhysVol * SCT_FwdModule::build(SCT_Identifier id) const
{

  // build method for creating module parent physical volume 
  // and puting all components into it
  // - relative position of component is part of its shape 
  GeoFullPhysVol * module = new GeoFullPhysVol(m_logVolume);

  if(m_connector != NULL) module->add(m_connector->getVolume());
  module->add(m_hybrid->getVolume());
  module->add(m_spine->getVolume());
  module->add(m_subspineL->getVolume());
  module->add(m_subspineR->getVolume());


  // name tags are not final 


  // Position bottom (x<0)sensor
  double positionX;
  double positionZ = m_sensor->sensorOffset(); // For truncated middle the sensor is offset.
  double rotation;
  positionX =-(0.5*m_spine->thickness() + m_glueThickness + 0.5*m_sensor->thickness());
  rotation = 0.5 * m_stereoAngle;
  GeoTrf::Translation3D vecB(positionX,0,0);
  // Rotate so that X axis goes from backside to implant side 
  GeoTrf::Transform3D rotB = GeoTrf::RotateX3D(rotation)*GeoTrf::RotateZ3D(180*Gaudi::Units::degree);
  // First translate in z (only non-zero for truncated middle)
  // Then rotate and then translate in x.
  GeoAlignableTransform *bottomTransform
    = new GeoAlignableTransform(GeoTrf::Transform3D(vecB*rotB)*GeoTrf::TranslateZ3D(positionZ));

  int bottomSideNumber = (m_upperSide) ? 0 : 1; 
  id.setSide(bottomSideNumber);
  module->add(new GeoNameTag("Sensor_Side#"+intToString(bottomSideNumber)));
  module->add(new GeoIdentifierTag(600+bottomSideNumber));
  module->add(bottomTransform);
  GeoVPhysVol * bottomSensorPV = m_sensor->build(id);
  module->add(bottomSensorPV);
  
  // Store transform
  detectorManager()->addAlignableTransform(0, id.getWaferId(), bottomTransform, bottomSensorPV); 

  
  if (m_ringType == 2) { // Place glass pieces in place of sensor
    module->add(new GeoTransform(GeoTrf::Transform3D(vecB*rotB)));
    module->add(m_sensor->getInactive());
  }

  // Position top (x>0) sensor
  positionX=-positionX;
  rotation=-rotation;
  GeoTrf::RotateX3D rotT(rotation);
  //rotT.rotateZ(180*Gaudi::Units::degree); // Rotate so that X axis goes from implant side to backside 
  GeoTrf::Translation3D vecT(positionX,0,0);
  // First translate in z (only non-zero for truncated middle)
  // Then rotate and then translate in x.
  GeoAlignableTransform *topTransform  
    = new GeoAlignableTransform(GeoTrf::Transform3D(vecT*rotT)*GeoTrf::TranslateZ3D(positionZ));
       
  int topSideNumber = m_upperSide;
  id.setSide(topSideNumber);
  module->add(new GeoNameTag("Sensor_Side#"+intToString(topSideNumber)));
  module->add(new GeoIdentifierTag(600+topSideNumber));
  module->add(topTransform);
  GeoVPhysVol * topSensorPV = m_sensor->build(id);
  module->add(topSensorPV);

  // Store transform
  detectorManager()->addAlignableTransform(0, id.getWaferId(), topTransform, topSensorPV); 

  if (m_ringType == 2) { // Place glass pieces in place of sensor
    module->add(new GeoTransform(GeoTrf::Transform3D(vecT*rotT)));
    module->add(m_sensor->getInactive());
  };  

  return module;

}
