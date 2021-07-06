/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoModelInterfaces/StoredMaterialManager.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoElement.h"
#include "GeoModelKernel/GeoPcon.h"
#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoCons.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoAlignableTransform.h"
#include "GeoModelKernel/GeoDefinitions.h"
#include "GeoModelKernel/Units.h"
#include "GeoModelUtilities/GeoExtendedMaterial.h"
#include "GeoModelUtilities/GeoMaterialPropertiesTable.h"
#include "GeoModelUtilities/GeoOpticalSurface.h"
#include "GeoModelUtilities/GeoBorderSurfaceContainer.h"
#include "GeoModelUtilities/GeoOpticalPhysVol.h"

#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "AthenaKernel/getMessageSvc.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"

#include "LUCID_DetectorFactory.h"
#include "GetRefIndex.h"

#include "LUCID_RDBAaccess.h"

#include <iomanip>

LUCID_DetectorFactory::LUCID_DetectorFactory(StoreGateSvc* detStore,
					     IRDBAccessSvc* pAccess)
  :m_detectorManager(0),
   m_materialManager(0),
   m_detectorStore  (detStore),
   m_access         (pAccess),
   m_lp             (new LUCID_RDBAccess()) {

  m_quartz = 0;
  m_peek = 0;
}

LUCID_DetectorFactory::~LUCID_DetectorFactory() {
  delete m_lp;
}

void LUCID_DetectorFactory::create(GeoPhysVol* world) {

  MsgStream log(Athena::getMessageSvc(), "LUCID_DetectorFactory::create");
  
  log << MSG::INFO << " LUCID_DetectorFactory::create " << endmsg;

  m_detectorManager = new LUCID_DetectorManager();
  
  if (StatusCode::SUCCESS != m_detectorStore->retrieve(m_materialManager, std::string("MATERIALS"))) return; 
  
  buildMaterials(); 
  
  log << MSG::DEBUG << " Build LUCID side A " << endmsg;

  GeoPcon* aShape = new GeoPcon(0, 360*Gaudi::Units::deg); 
  
  double VJConOuterRadFront     =  110;
  double VJConOuterRadBack      =  187;
  double VJConFrontLength       =  225;
  double VJConMidLength         = 4485;
  double VJConBackLength        =  425; 
  double VJConFrontRingInnerRad =  100;
  double VJConBackRingInerRad   =  152;

  aShape->addPlane(   0,                                               VJConFrontRingInnerRad-10, VJConOuterRadFront+10);
  aShape->addPlane(VJConFrontLength,                                   VJConFrontRingInnerRad-10, VJConOuterRadFront+10);
  aShape->addPlane(VJConFrontLength+VJConMidLength,                    VJConFrontRingInnerRad-10, VJConOuterRadBack +10);
  aShape->addPlane(5036,                                               VJConBackRingInerRad  -10, VJConOuterRadBack +10);
  aShape->addPlane(VJConFrontLength+VJConMidLength+VJConBackLength+30, VJConBackRingInerRad  -10, VJConOuterRadBack +10); 

  GeoLogVol*      logVol  = new GeoLogVol("Lucid", aShape, m_materialManager->getMaterial("std::Air"));
  GeoFullPhysVol* phyVolA = new GeoFullPhysVol(logVol);
  
  addVJcone              (phyVolA);
  addVJconeFrontRing     (phyVolA);
  addVJconeBackRing      (phyVolA);
  addLucidSupportCylinder(phyVolA);
  addPmtSupportCylinder  (phyVolA);
  addPmtTubes            (phyVolA);
  addPmtSupport          (phyVolA);
  
  double VJConeDistanceToIP = 13442;

  world->add(new GeoAlignableTransform(GeoTrf::Translate3D(0, 0,  VJConeDistanceToIP)));
  world->add(new GeoNameTag("LucidSideA"));
  world->add(phyVolA);
  
  m_detectorManager->addTreeTop(phyVolA);
  
  log << MSG::DEBUG << " Build LUCID side C " << endmsg;
  
  GeoFullPhysVol* phyVolC = phyVolA->clone();
    
  world->add(new GeoAlignableTransform(GeoTrf::Translate3D(0, 0, -VJConeDistanceToIP )*GeoTrf::RotateY3D(180*Gaudi::Units::degree)*GeoTrf::RotateZ3D(180*Gaudi::Units::degree)));
  world->add(new GeoNameTag("LucidSideC"));
  world->add(phyVolC);
  
  m_detectorManager->addTreeTop(phyVolC);
}

void LUCID_DetectorFactory::buildMaterials()  {  

  MsgStream log(Athena::getMessageSvc(), "LUCID_DetectorFactory::buildMaterials");

  log << MSG::INFO << " LUCID_DetectorFactory::buildMaterials " << endmsg;
  
  log << MSG::DEBUG << " Wavelength dependent properties " << endmsg;
  
  const int waveLengthNum = (int)((m_lp->waveLengthMax - m_lp->waveLengthMin)/m_lp->waveLengthStep);
  
  double* photonWaveLength = new double[waveLengthNum];
  double* photonEnergy     = new double[waveLengthNum];
  double* quartzRefIndex   = new double[waveLengthNum];

  log << MSG::INFO << " Lambda[nm] Energy[eV] QuartzRefIndex " << endmsg;
  
  for(int i=0; i<waveLengthNum; i++) {
    
    photonWaveLength[i] = (m_lp->waveLengthMin + i*m_lp->waveLengthStep)*Gaudi::Units::nm;
    photonEnergy    [i] = 2.*M_PI*(Gaudi::Units::hbarc/(Gaudi::Units::eV*Gaudi::Units::nm))/(photonWaveLength[i]/Gaudi::Units::nm)*Gaudi::Units::eV;
    quartzRefIndex  [i] = GetRefIndexQuartz (photonWaveLength[i]/Gaudi::Units::nm);
  }
  
  log << MSG::DEBUG << " **************************************************************************************************** " << endmsg;
  log << MSG::DEBUG << " Lambda[nm] Energy[eV] QuartzRefIndex GasRefIndex GasAbsLength[m] TubeReflectivity " << endmsg;
  
  for(int i=0; i<waveLengthNum; i++) {
    
    log << MSG::DEBUG 
	<< std::setw(11) << photonWaveLength[i]/Gaudi::Units::nm
	<< std::setw(11) << photonEnergy    [i]/Gaudi::Units::eV
	<< std::setw(13) << quartzRefIndex  [i]
	<< endmsg;
  }
  
  log << MSG::DEBUG << " ********************************************************************************************************** " << endmsg;
  
  log << MSG::DEBUG << " Build Quartz for PMT windows" << endmsg; 
  
  m_quartz = new GeoExtendedMaterial("SiO2", m_lp->quartzDensity, stateSolid, Gaudi::Units::STP_Temperature);
  
  const GeoElement* oxygen  = m_materialManager->getElement("Oxygen");
  const GeoElement* silicon = m_materialManager->getElement("Silicon");

  m_quartz->add(const_cast<GeoElement*>(silicon), 1*silicon->getA()/(2*oxygen->getA() + 1*silicon->getA()));
  m_quartz->add(const_cast<GeoElement*>(oxygen) , 2*oxygen->getA() /(2*oxygen->getA() + 1*silicon->getA()));
  
  GeoMaterialPropertiesTable* mpt = new GeoMaterialPropertiesTable();
  
  mpt->AddProperty("RINDEX", photonEnergy, quartzRefIndex , waveLengthNum);
  
  log << MSG::DEBUG << "LUCID Quartz Material Properties Table " << mpt << endmsg;

  m_quartz->SetMaterialPropertiesTable(mpt); 
  m_quartz->lock();

  delete [] photonWaveLength;
  delete [] photonEnergy;
  delete [] quartzRefIndex;

  log << MSG::DEBUG << " Build Peek: Carbon with density 1.320*g/cm3 " << endmsg;

  m_peek = new GeoMaterial("peek", 1.320*GeoModelKernelUnits::g/Gaudi::Units::cm3);
  const GeoElement* carbon = m_materialManager->getElement("Carbon");
  m_peek->add(const_cast<GeoElement*>(carbon),1.0);
  m_peek->lock();
}

void LUCID_DetectorFactory::addVJcone(GeoFullPhysVol* parent) {

  MsgStream log(Athena::getMessageSvc(), "LUCID_DetectorFactory::addVJcone");

  log << MSG::INFO << " LUCID_DetectorFactory::addVJcone " << endmsg;

  double VJConInerRadFront  =  105;
  double VJConOuterRadFront =  110;
  double VJConInerRadBack   =  182;
  double VJConOuterRadBack  =  187;
  double VJConFrontLength   =  225;
  double VJConMidLength     = 4485;
  double VJConBackLength    =  425;
  
  GeoPcon* aShape = new GeoPcon(0, 360*Gaudi::Units::degree); 
  
  aShape->addPlane(              0,                                 VJConInerRadFront, VJConOuterRadFront);
  aShape->addPlane(VJConFrontLength,                                VJConInerRadFront, VJConOuterRadFront);
  aShape->addPlane(VJConFrontLength+VJConMidLength,                 VJConInerRadBack,  VJConOuterRadBack);
  aShape->addPlane(VJConFrontLength+VJConMidLength+VJConBackLength, VJConInerRadBack,  VJConOuterRadBack);

  GeoLogVol*  logVol = new GeoLogVol("lvVJcone", aShape, m_materialManager->getMaterial("std::Carbon"));
  GeoPhysVol* phyVol = new GeoPhysVol(logVol); 

  parent->add(new GeoTransform(GeoTrf::Translate3D(0, 0, 0)));
  parent->add(new GeoNameTag("VJcone"));
  parent->add(phyVol);
}

void LUCID_DetectorFactory::addVJconeFrontRing(GeoFullPhysVol* parent) {

  MsgStream log(Athena::getMessageSvc(), "LUCID_DetectorFactory");

  log << MSG::INFO << " LUCID_DetectorFactory::addVJconeFrontRing " << endmsg;

  double VJConFrontRingLength   = 215;
  double VJConFrontRingInnerRad = 100;
  double VJConInerRadFront      = 105;
  
  GeoShape*   aShape = new GeoTube(VJConFrontRingInnerRad, VJConInerRadFront, VJConFrontRingLength/2); 
  GeoLogVol*  logVol = new GeoLogVol("lvVJconeFrontRing", aShape, m_materialManager->getMaterial("std::Aluminium"));
  GeoPhysVol* phyVol = new GeoPhysVol(logVol); 
  
  parent->add(new GeoTransform(GeoTrf::Translate3D(0, 0, VJConFrontRingLength/2)));
  parent->add(new GeoNameTag("VJconeFrontRing"));
  parent->add(phyVol);
}

void LUCID_DetectorFactory::addVJconeBackRing(GeoFullPhysVol* parent) {

  MsgStream log(Athena::getMessageSvc(), "LUCID_DetectorFactory");

  log << MSG::INFO << " LUCID_DetectorFactory::addVJconeBackRing " << endmsg;

  double VJConBackRingLength         = 35;
  double VJConBackRingCentrePosition = (18567-13442)+(VJConBackRingLength/2);
  double VJConBackRingInerRad        = 152;
  double LucidCylinderRadius         = 177;
 
  GeoShape*   aShape = new GeoTube(VJConBackRingInerRad, LucidCylinderRadius, VJConBackRingLength/2);
  GeoLogVol*  logVol = new GeoLogVol("lvVJconeBackRing", aShape, m_materialManager->getMaterial("std::Aluminium"));
  GeoPhysVol* phyVol = new GeoPhysVol(logVol);
 
  parent->add(new GeoTransform(GeoTrf::Translate3D(0, 0, VJConBackRingCentrePosition)));
  parent->add(new GeoNameTag("VJconeBackRing"));
  parent->add(phyVol);
}

void LUCID_DetectorFactory::addLucidSupportCylinder(GeoFullPhysVol* parent) {

  MsgStream log(Athena::getMessageSvc(), "LUCID_DetectorFactory");

  log << MSG::INFO << " LUCID_DetectorFactory::addLucidSupportCylinder " << endmsg;
  
  double LucidCylinderRadius         = 110;
  double LucidCylinderThickness      = 1.5;
  double LucidCylinderLength         = 18400-16850;
  double LucidCylinderCenterPosition = (16850-13442)+(LucidCylinderLength/2);
  
  GeoShape*   aShape = new GeoTube  (LucidCylinderRadius, LucidCylinderRadius + LucidCylinderThickness, LucidCylinderLength/2);
  GeoLogVol*  logVol = new GeoLogVol("lvLucidSupportCylinder", aShape, m_materialManager->getMaterial("std::Carbon"));
  GeoPhysVol* phyVol = new GeoPhysVol(logVol); 
  
  parent->add(new GeoTransform(GeoTrf::Translate3D(0, 0,LucidCylinderCenterPosition)));
  parent->add(new GeoNameTag("LucidSupportCylinder"));
  parent->add(phyVol);
}
  
void LUCID_DetectorFactory::addPmtSupportCylinder(GeoFullPhysVol* parent) {

  MsgStream log(Athena::getMessageSvc(), "LUCID_DetectorFactory");

  log << MSG::INFO << " LUCID_DetectorFactory::addPmtSupportCylinder " << endmsg;
    
  double PmtSupportCylinderRadius         = 111.75;
  double PmtSupportCylinderThickness      = 1.5;
  double PmtSupportCylinderLength         = 17120-16920;
  double PmtSupportCylinderCenterPosition = (16920-13442)+(PmtSupportCylinderLength/2);
 
  GeoShape*   aShape = new GeoTube  (PmtSupportCylinderRadius, PmtSupportCylinderRadius + PmtSupportCylinderThickness, PmtSupportCylinderLength/2);
  GeoLogVol*  logVol = new GeoLogVol("lvPmtSupportCylinder", aShape, m_materialManager->getMaterial("std::Aluminium"));
  GeoPhysVol* phyVol = new GeoPhysVol(logVol);  
  
  parent->add(new GeoTransform(GeoTrf::Translate3D(0, 0, PmtSupportCylinderCenterPosition)));
  parent->add(new GeoNameTag("PmtSupportCylinder"));
  parent->add(phyVol);
}

void LUCID_DetectorFactory::addPmtSupport(GeoFullPhysVol* parent) {

  MsgStream log(Athena::getMessageSvc(), "LUCID_DetectorFactory");
 
  log << MSG::INFO << " LUCID_DetectorFactory::addPmtSupport" << endmsg;

  double PmtSupportRadius        = 113.25;
  double PmtSupportThickness     = 3.4;
  double PmtSupportLength        = 17082-16962;
  double PmtSupportFrontPosition = (16962-13442);

  int nSections = 4;
 
  for (int sec=0; sec<nSections; sec++) {
 
    GeoPcon* aShape = new GeoPcon(0.134 * M_PI +(sec-1)*(M_PI/2),0.232 * M_PI);
 
    aShape->addPlane(               0,   PmtSupportRadius,  PmtSupportRadius+PmtSupportThickness);
    aShape->addPlane(PmtSupportLength,   PmtSupportRadius,  PmtSupportRadius+PmtSupportThickness);

    GeoLogVol*  logVol = new GeoLogVol("lvPmtSupport", aShape, m_materialManager->getMaterial("std::Aluminium"));
    GeoPhysVol* phyVol = new GeoPhysVol(logVol);

    parent->add(new GeoTransform(GeoTrf::Translate3D(0, 0, PmtSupportFrontPosition)));
    parent->add(new GeoNameTag("PmtSupport"));
    parent->add(phyVol);
  }
}

void LUCID_DetectorFactory::addPmtTubes(GeoFullPhysVol* parent) {
  
  MsgStream log(Athena::getMessageSvc(), "LUCID_DetectorFactory");

  log << MSG::INFO << " LUCID_DetectorFactory::addPmtTubes " << endmsg;
 
  double MUMetalCylinderInnerRad = 7.99;
  double MuMetalCylinderOuterRad = 8.835;
  double MuMetalCylinderLength   = 75;

  double PmtRadius               = 5;
  double PmtCenterPosition       = 125.5;
  double PmtWindowThickness      = 1.2;

  double PeekCapInnerRad         = 1.6;
  double PeekCapOuterRad         = 8.835;
  double PeekCapLength           = 2.8;

  double MetalConnectorOuterRad  = 1.6;
  double MetalConnectorLength    = 9.8;

  GeoShape*   aShape = new GeoTube(0, PmtRadius, PmtWindowThickness/2);  
  GeoLogVol*  logVol = new GeoLogVol("lvPmt", aShape, m_quartz);
  GeoPhysVol* phyVol = new GeoPhysVol(logVol); 
  
  GeoShape*   aShapeMu = new GeoTube(MUMetalCylinderInnerRad, MuMetalCylinderOuterRad, MuMetalCylinderLength/2);  
  GeoLogVol*  logVolMu = new GeoLogVol("lvMuMetalCylinder", aShapeMu, m_materialManager->getMaterial("std::Iron"));
  GeoPhysVol* phyVolMu = new GeoPhysVol(logVolMu);

  GeoShape*   aShapePeekCap = new GeoTube(PeekCapInnerRad, PeekCapOuterRad, PeekCapLength/2);  
  GeoLogVol*  logVolPeekCap = new GeoLogVol("lvPmtPeekCap", aShapePeekCap, m_peek);
  GeoPhysVol* phyVolPeekCap = new GeoPhysVol(logVolPeekCap);

  GeoShape*   aShapeMetalconnector = new GeoTube(0, MetalConnectorOuterRad, MetalConnectorLength/2);  
  GeoLogVol*  logVolMetalConnector = new GeoLogVol("lvPmtMetalConnector", aShapeMetalconnector, m_materialManager->getMaterial("std::Iron"));
  GeoPhysVol* phyVolMetalConnector = new GeoPhysVol(logVolMetalConnector);

  double phi              = M_PI/6*Gaudi::Units::rad;
  int nSections           = 4;
  int nPmtTubesPerSection = 4;

  for (int sec=0; sec<nSections; sec++) {
    for (int tub=0; tub<nPmtTubesPerSection; tub++) {
      
      double x      = PmtCenterPosition*cos(phi/Gaudi::Units::rad);
      double y      = PmtCenterPosition*sin(phi/Gaudi::Units::rad);
      double z      = (16970-13442)+(PmtWindowThickness/2);
      double zMu    = (16963-13442)+(MuMetalCylinderLength/2);
      double zCap   = (16963-13442)-(PeekCapLength/2);
      double zMetal = (16963-13442)-(MetalConnectorLength/2);

      char sname[256]; sprintf(sname, "LucidPmt%d", 4*sec+tub);
      
      log << MSG::INFO 
    << format("Adding %11s phi:%10.3f x:%10.3f y:%10.3f z:%10.3f", sname, phi, x, y, z) 
    << endmsg;
      
      parent->add(new GeoNameTag  (sname));
      parent->add(new GeoTransform(GeoTrf::Translate3D(x, y, z)));
      parent->add(phyVol);

      char snameMu[256]; sprintf(snameMu, "MuMetal cylinder%d", 4*sec+tub);
      
      log << MSG::INFO 
    << format("Adding %11s phi:%10.3f x:%10.3f y:%10.3f zMu:%10.3f", snameMu, phi, x, y, zMu) 
    << endmsg;
      
      parent->add(new GeoNameTag  (snameMu));
      parent->add(new GeoTransform(GeoTrf::Translate3D(x, y, zMu)));
      parent->add(phyVolMu);

      char snamePeekCap[256]; sprintf(snamePeekCap, "PmtPeekCap%d", 4*sec+tub);
      
      log << MSG::INFO 
    << format("Adding %11s phi:%10.3f x:%10.3f y:%10.3f zCap:%10.3f", snamePeekCap, phi, x, y, zCap) 
    << endmsg;
      
      parent->add(new GeoNameTag  (snamePeekCap));
      parent->add(new GeoTransform(GeoTrf::Translate3D(x, y, zCap)));
      parent->add(phyVolPeekCap);

      char snameMetalConnector[256]; sprintf(snameMetalConnector, "MetalConnector%d", 4*sec+tub);
      
      log << MSG::INFO 
    << format("Adding %11s phi:%10.3f x:%10.3f y:%10.3f zMetal:%10.3f", snameMetalConnector, phi, x, y, zMetal) 
    << endmsg;
      
      parent->add(new GeoNameTag  (snameMetalConnector));
      parent->add(new GeoTransform(GeoTrf::Translate3D(x, y, zMetal)));
      parent->add(phyVolMetalConnector);

      phi = phi+(M_PI/18)*Gaudi::Units::rad;
    }
    phi = phi+(5* M_PI/18)*Gaudi::Units::rad;
  }
}

const LUCID_DetectorManager* LUCID_DetectorFactory::getDetectorManager() const { return m_detectorManager; }
