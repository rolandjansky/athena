/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoModelInterfaces/AbsMaterialManager.h"
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
#include "GetReflectivity.h"
#include "GetRefIndex.h"
#include "GetAbsLength.h"

#include "LUCID_RDBAaccess.h"

#include <iomanip>

LUCID_DetectorFactory::LUCID_DetectorFactory(StoreGateSvc* detStore,
					     IRDBAccessSvc* pAccess)
  :m_detectorManager(0),
   m_materialManager(0),
   m_detectorStore  (detStore),
   m_access         (pAccess),
   m_lp             (new LUCID_RDBAccess()) {

  m_air = 0;
  m_alu = 0;
  m_cop = 0;
  m_gas = 0;
  m_quartz = 0;
  m_optSur = 0;
  
  for (int i=0; i<nLayers; i++) { 
    m_tubeTheta[i] = 0;
    for (int j=0; j<nPmtTubesPerLayer; j++)
      m_tubePosition[i][j][0] = m_tubePosition[i][j][1] = 0;
  }
}

LUCID_DetectorFactory::~LUCID_DetectorFactory() {
  delete m_lp;
}

void LUCID_DetectorFactory::create(GeoPhysVol* world) {

  MsgStream log(Athena::getMessageSvc(), "LUCID_DetectorFactory::create");
  
  log << MSG::INFO << " LUCID_DetectorFactory::create " << endmsg;

  m_detectorManager = new LUCID_DetectorManager();
  
  const StoredMaterialManager* materialManagerTmp = 0;
  if (StatusCode::SUCCESS != m_detectorStore->retrieve(materialManagerTmp, std::string("MATERIALS"))) return; 
  m_materialManager = materialManagerTmp;
  
  buildMaterials(); 
  calcTubeParams();
  
  log << MSG::DEBUG << " Build LUCID side A " << endmsg;

  GeoPcon* aShape = new GeoPcon(0, 360*Gaudi::Units::deg); 
  
  aShape->addPlane(                 0, m_lp->coolingRadius, m_lp->VJconeRadiusFront);
  aShape->addPlane(m_lp->VJconelength, m_lp->coolingRadius, m_lp->VJconeRadiusBack);
  
  GeoLogVol*      logVol  = new GeoLogVol("Lucid", aShape, m_air);
  GeoFullPhysVol* phyVolA = new GeoFullPhysVol(logVol);
  
  addVJcone (phyVolA);
  addCooling(phyVolA);
  addVessel (phyVolA);
  
  world->add(new GeoAlignableTransform(GeoTrf::Translate3D(0, 0,  m_lp->VJdistanceToIP)));
  world->add(new GeoNameTag("LucidSideA"));
  world->add(phyVolA);
  
  m_detectorManager->addTreeTop(phyVolA);
  
  log << MSG::DEBUG << " Build LUCID side C " << endmsg;
  
  GeoFullPhysVol* phyVolC = phyVolA->clone();
    
  world->add(new GeoAlignableTransform(GeoTrf::Translate3D(0, 0, -m_lp->VJdistanceToIP )*GeoTrf::RotateY3D(180*Gaudi::Units::degree)*GeoTrf::RotateZ3D(180*Gaudi::Units::degree)));
  world->add(new GeoNameTag("LucidSideC"));
  world->add(phyVolC);
  
  m_detectorManager->addTreeTop(phyVolC);
}

void LUCID_DetectorFactory::buildMaterials()  {  

  MsgStream log(Athena::getMessageSvc(), "LUCID_DetectorFactory::buildMaterials");

  log << MSG::INFO << " LUCID_DetectorFactory::buildMaterials " << endmsg;
  
  log << MSG::DEBUG << " Build Air for LUCID volume " << endmsg; 
  
  m_air = m_materialManager->getMaterial("std::Air");
  
  log << MSG::DEBUG << " Build Aluminium for Vessel and Tubes " << endmsg; 

  m_alu = m_materialManager->getMaterial("std::Aluminium");
  
  log << MSG::DEBUG << " Aluminium density[g/cm3]: " << m_alu->getDensity()/(GeoModelKernelUnits::g/Gaudi::Units::cm3) << endmsg;

  log << MSG::DEBUG << " Build Cherenkov Gas " << endmsg;
  
  m_gas = new GeoExtendedMaterial("C4F10", m_lp->gasDensity, stateGas, m_lp->gasTemperature, m_lp->gasPressure);

  const GeoElement* carbon   = m_materialManager->getElement("Carbon");
  const GeoElement* fluorine = m_materialManager->getElement("Fluorine");
  
  m_gas->add(const_cast<GeoElement*>(carbon),    4*carbon->getA()  /(4*carbon->getA() + 10*fluorine->getA()));
  m_gas->add(const_cast<GeoElement*>(fluorine), 10*fluorine->getA()/(4*carbon->getA() + 10*fluorine->getA()));
  
  log << MSG::DEBUG << " gasState              : " << m_gas->getState()              << endmsg;
  log << MSG::DEBUG << " gasTemperature[kelvin]: " << m_gas->getTemperature()/Gaudi::Units::kelvin << endmsg;
  log << MSG::DEBUG << " gasPressure      [bar]: " << m_gas->getPressure()/Gaudi::Units::bar       << endmsg;
  log << MSG::DEBUG << " gasDensity     [g/cm3]: " << m_gas->getDensity()/(GeoModelKernelUnits::g/Gaudi::Units::cm3)    << endmsg;

  log << MSG::DEBUG << " Wavelength dependent properties " << endmsg;
  
  const int waveLengthNum = (int)((m_lp->waveLengthMax - m_lp->waveLengthMin)/m_lp->waveLengthStep);
  
  double* photonWaveLength = new double[waveLengthNum];
  double* photonEnergy     = new double[waveLengthNum];
  double* quartzRefIndex   = new double[waveLengthNum];
  double* gasRefIndex      = new double[waveLengthNum];
  double* gasAbsLength     = new double[waveLengthNum];
  double* tubeReflectivity = new double[waveLengthNum];
  
  double absLengthScale = (m_lp->gasPressure) ? (m_lp->gasPressure/Gaudi::Units::bar) : 1;
  
  for(int i=0; i<waveLengthNum; i++) {
    
    photonWaveLength[i] = (m_lp->waveLengthMin + i*m_lp->waveLengthStep)*Gaudi::Units::nm;
    photonEnergy    [i] = 2.*M_PI*(Gaudi::Units::hbarc/(Gaudi::Units::eV*Gaudi::Units::nm))/(photonWaveLength[i]/Gaudi::Units::nm)*Gaudi::Units::eV;
    quartzRefIndex  [i] = GetRefIndexQuartz (photonWaveLength[i]/Gaudi::Units::nm);
    gasRefIndex     [i] = GetRefIndexGas    (photonWaveLength[i]/Gaudi::Units::nm, m_lp->gasPressure/Gaudi::Units::bar, m_lp->gasTemperature/Gaudi::Units::kelvin);
    gasAbsLength    [i] = GetAbsLengthGas   (photonWaveLength[i]/Gaudi::Units::nm)*Gaudi::Units::m/absLengthScale;
    tubeReflectivity[i] = GetReflectivity   (photonWaveLength[i]/Gaudi::Units::nm);
  }
  
  log << MSG::DEBUG << " **************************************************************************************************** " << endmsg;
  log << MSG::DEBUG << " Lambda[nm] Energy[eV] QuartzRefIndex GasRefIndex GasAbsLength[m] TubeReflectivity " << endmsg;
  
  for(int i=0; i<waveLengthNum; i++) {
    
    log << MSG::DEBUG 
	<< std::setw(11) << photonWaveLength[i]/Gaudi::Units::nm
	<< std::setw(11) << photonEnergy    [i]/Gaudi::Units::eV
	<< std::setw(13) << quartzRefIndex  [i]
	<< std::setw(10) << gasRefIndex     [i]
	<< std::setw(16) << gasAbsLength    [i]/Gaudi::Units::m
	<< std::setw(17) << tubeReflectivity[i]
	<< endmsg;
  }
  
  log << MSG::DEBUG << " ********************************************************************************************************** " << endmsg;

  GeoMaterialPropertiesTable* mpt = new GeoMaterialPropertiesTable();
  
  mpt->AddProperty("RINDEX"   , photonEnergy, gasRefIndex , waveLengthNum);
  mpt->AddProperty("ABSLENGTH", photonEnergy, gasAbsLength, waveLengthNum);
  
  log << MSG::DEBUG  << "LUCID Gas Material Properties Table " << mpt << endmsg;
  
  m_gas->SetMaterialPropertiesTable(mpt); 
  m_gas->lock();
  
  log << MSG::DEBUG << " Build Quartz for PMT windows" << endmsg; 
  
  m_quartz = new GeoExtendedMaterial("SiO2", m_lp->quartzDensity, stateSolid, Gaudi::Units::STP_Temperature);
  
  const GeoElement* oxygen  = m_materialManager->getElement("Oxygen");
  const GeoElement* silicon = m_materialManager->getElement("Silicon");

  m_quartz->add(const_cast<GeoElement*>(silicon), 1*silicon->getA()/(2*oxygen->getA() + 1*silicon->getA()));
  m_quartz->add(const_cast<GeoElement*>(oxygen) , 2*oxygen->getA() /(2*oxygen->getA() + 1*silicon->getA()));
  
  mpt = new GeoMaterialPropertiesTable();
  
  mpt->AddProperty("RINDEX", photonEnergy, quartzRefIndex , waveLengthNum);
  
  log << MSG::DEBUG << "LUCID Quartz Material Properties Table " << mpt << endmsg;

  m_quartz->SetMaterialPropertiesTable(mpt); 
  m_quartz->lock();

  log << MSG::DEBUG << " Build Copper for cooling system " << endmsg; 
  
  m_cop = m_materialManager->getMaterial("std::Copper");
  
  log << MSG::DEBUG << " Copper density[g/cm3]: " << m_cop->getDensity()/(GeoModelKernelUnits::g/Gaudi::Units::cm3) << endmsg;
    
  log << MSG::DEBUG << " Build Reflective Tube Optical Surfaces " << endmsg; 
  
  m_optSur = new GeoOpticalSurface("TubeOpticalSurface",
				   GeoOpticalSurface::glisur,
				   GeoOpticalSurface::ground,
				   GeoOpticalSurface::dielectric_metal,
				   m_lp->tubePolish);
  
  mpt = new GeoMaterialPropertiesTable();
  
  mpt->AddProperty("REFLECTIVITY", photonEnergy, tubeReflectivity, waveLengthNum);
    
  log << MSG::DEBUG << "LUCID Tube Material Properties Table " << mpt << endmsg;
  
  m_optSur->SetMaterialPropertiesTable(mpt); 

  delete [] photonWaveLength;
  delete [] photonEnergy;
  delete [] quartzRefIndex;
  delete [] gasRefIndex;
  delete [] gasAbsLength;
  delete [] tubeReflectivity;
}

void LUCID_DetectorFactory::calcTubeParams() {

  MsgStream log(Athena::getMessageSvc(), "LUCID_DetectorFactory::calcTubeParams");

  log << MSG::INFO << " LUCID_DetectorFactory::calcTubeParams " << endmsg; 

  double layerRadius  [nLayers] = {m_lp->layerRadius1, m_lp->layerRadius2};
  double tubePhiOffset[nLayers];
  double tubePhiAngle [nLayers][nPmtTubesPerLayer];
  
  for (int i=0; i<nLayers; i++) {
    
    m_tubeTheta  [i] = atan(layerRadius[i]/m_lp->distanceToIP)*Gaudi::Units::rad;
    tubePhiOffset[i] = (i==0)*M_PI/8*Gaudi::Units::rad;
    
    for (int j=0; j<nPmtTubesPerLayer; j++) {
      
      tubePhiAngle[i][j] = 2*M_PI*j/nPmtTubesPerLayer*Gaudi::Units::rad + tubePhiOffset[i];
      
      double tubeDistance = layerRadius[i] + m_lp->tubeLength/2*sin(m_tubeTheta[i]/Gaudi::Units::rad);
      
      m_tubePosition[i][j][0] = tubeDistance*cos(tubePhiAngle[i][j]/Gaudi::Units::rad); if (fabs(m_tubePosition[i][j][0]) < 1e-10) m_tubePosition[i][j][0] = 0;
      m_tubePosition[i][j][1] = tubeDistance*sin(tubePhiAngle[i][j]/Gaudi::Units::rad); if (fabs(m_tubePosition[i][j][1]) < 1e-10) m_tubePosition[i][j][1] = 0;
    }
  }
  
  log << MSG::DEBUG << " ********************************************************************************************************************************* " << endmsg;
  log << MSG::DEBUG << " Layer Tube TubeRadius[mm] LayerRadius[mm] TubeTheta[deg] TubePhiOffset[deg] TubePhiAngle[deg] TubePositionX[mm] TubePositionY[mm] " << endmsg;

  log.precision(5);
  
  for (int lay=0; lay<nLayers; lay++) 
    for (int tub=0; tub<nPmtTubesPerLayer; tub++)
  
      log << MSG::DEBUG << setiosflags(std::ios::right)
	  << std::setw( 6) << lay
	  << std::setw( 5) << tub
	  << std::setw(11) << m_lp->tubeRadius/Gaudi::Units::mm
	  << std::setw(16) << layerRadius   [lay]/Gaudi::Units::mm
	  << std::setw(14) << m_tubeTheta   [lay]/Gaudi::Units::degree
	  << std::setw(19) << tubePhiOffset [lay]/Gaudi::Units::degree
	  << std::setw(18) << tubePhiAngle  [lay][tub]/Gaudi::Units::degree
	  << std::setw(19) << m_tubePosition[lay][tub][0]/Gaudi::Units::mm
	  << std::setw(18) << m_tubePosition[lay][tub][1]/Gaudi::Units::mm
	  << endmsg;
  
  log << MSG::DEBUG << " ********************************************************************************************************************************* " << endmsg;
}

void LUCID_DetectorFactory::addVJcone(GeoFullPhysVol* parent) {

  MsgStream log(Athena::getMessageSvc(), "LUCID_DetectorFactory::addVJcone");

  log << MSG::INFO << " LUCID_DetectorFactory::addVJcone " << endmsg;

  GeoShape*   aShape0 =  new GeoCons(m_lp->VJconeRadiusFront - m_lp->VJconeThickness, 
				     m_lp->VJconeRadiusBack  - m_lp->VJconeThickness,
				     m_lp->VJconeRadiusFront, 
				     m_lp->VJconeRadiusBack, 
				     (m_lp->VJconelength - (m_lp->VJconeFrontRingLength - m_lp->VJconeFrontRingOverlap))/2, 0*Gaudi::Units::deg,360*Gaudi::Units::deg);
  
  GeoLogVol*  logVol0 = new GeoLogVol("lvVJcone", aShape0, m_alu);
  GeoPhysVol* phyVol0 = new GeoPhysVol(logVol0); 
  
  parent->add(new GeoTransform(GeoTrf::TranslateZ3D((m_lp->VJconelength + (m_lp->VJconeFrontRingLength - m_lp->VJconeFrontRingOverlap))/2)));
  parent->add(new GeoNameTag("LucidVJcone"));
  parent->add(phyVol0);
  
  GeoShape*   aShape1 = new GeoTube  (m_lp->VJconeRadiusFront - m_lp->VJconeThickness - m_lp->VJconeFrontRingThickness, 
				      m_lp->VJconeRadiusFront - m_lp->VJconeThickness , m_lp->VJconeFrontRingLength/2);
  
  GeoLogVol*  logVol1 = new GeoLogVol("lvVJconeFrontRing", aShape1, m_alu);
  GeoPhysVol* phyVol1 = new GeoPhysVol(logVol1); 
  
  parent->add(new GeoTransform(GeoTrf::TranslateZ3D(m_lp->VJconeFrontRingLength/2)));
  parent->add(new GeoNameTag("LucidVJconeFrontRing"));
  parent->add(phyVol1);
}

void LUCID_DetectorFactory::addCooling(GeoFullPhysVol* parent) {

  MsgStream log(Athena::getMessageSvc(), "LUCID_DetectorFactory::addCooling");

  log << MSG::INFO << " LUCID_DetectorFactory::addCooling " << endmsg;
    
  GeoShape*   aShape = new GeoTube  (m_lp->coolingRadius, m_lp->coolingRadius + m_lp->coolingThickness, m_lp->vesselLength/2);
  GeoLogVol*  logVol = new GeoLogVol("lvCooling", aShape, m_cop);
  GeoPhysVol* phyVol = new GeoPhysVol(logVol); 

  parent->add(new GeoTransform(GeoTrf::Translate3D(0, 0, m_lp->vesselLength/2 + (m_lp->distanceToIP - m_lp->VJdistanceToIP))));
  parent->add(new GeoNameTag("LucidCooling"));
  parent->add(phyVol);
}

void LUCID_DetectorFactory::addVessel(GeoFullPhysVol* parent) {

  MsgStream log(Athena::getMessageSvc(), "LUCID_DetectorFactory::addVessel");
  log << MSG::INFO << " LUCID_DetectorFactory::addVessel " << endmsg;
  
  GeoPcon* aShape = new GeoPcon(0, 360*Gaudi::Units::deg); 
  aShape->addPlane(                 0, m_lp->vesselInnerRadius, m_lp->vesselOuterRadMin + m_lp->vesselOuterThickness);
  aShape->addPlane(m_lp->vesselLength, m_lp->vesselInnerRadius, m_lp->vesselOuterRadMax + m_lp->vesselOuterThickness);
  
  

  GeoLogVol*  logVol = new GeoLogVol("lvVessel", aShape, m_alu);
  GeoPhysVol* phyVol = new GeoPhysVol(logVol);
  
  addVesselGas(phyVol);
  
  parent->add(new GeoAlignableTransform(GeoTrf::Translate3D(0, 0,  m_lp->distanceToIP - m_lp->VJdistanceToIP  )));
  parent->add(new GeoNameTag("LucidVessel"));
  parent->add(phyVol);
}

void LUCID_DetectorFactory::addVesselGas(GeoPhysVol* parent) {

  MsgStream log(Athena::getMessageSvc(), "LUCID_DetectorFactory::addVesselGas");

  log << MSG::INFO << " LUCID_DetectorFactory::addVesselGas " << endmsg;
  
  GeoPcon* aShape = new GeoPcon(0, 360*Gaudi::Units::deg); 
  aShape->addPlane(                 0, m_lp->vesselInnerRadius + m_lp->vesselInnerThickness, m_lp->vesselOuterRadMin);
  aShape->addPlane(m_lp->vesselLength, m_lp->vesselInnerRadius + m_lp->vesselInnerThickness, m_lp->vesselOuterRadMax);

  GeoLogVol*  logVol = new GeoLogVol("lvVesselGas", aShape, m_gas);
  GeoPhysVol* phyVol = new GeoPhysVol(logVol);
  
  addBulkHeads(phyVol);
  addTubes    (phyVol);
  
  parent->add(new GeoNameTag("LucidVesselGas"));
  parent->add(phyVol);
}

void LUCID_DetectorFactory::addBulkHeads(GeoPhysVol* parent) {

  MsgStream log(Athena::getMessageSvc(), "LUCID_DetectorFactory::addBulkHeads");

  log << MSG::INFO << " LUCID_DetectorFactory::addBulkHeads " << endmsg;

  GeoShape*   aShape0 = new GeoTube  (m_lp->vesselInnerRadius + m_lp->vesselInnerThickness, m_lp->vesselOuterRadMin, m_lp->bulkheadThickness/2);
  GeoLogVol*  logVol0 = new GeoLogVol("lvBulkHead0", aShape0, m_alu);
  GeoPhysVol* phyVol0 = new GeoPhysVol(logVol0); 
  
  parent->add(new GeoTransform(GeoTrf::Translate3D(0, 0, m_lp->bulkheadThickness/2)));
  parent->add(new GeoNameTag("LucidBulkHead0"));
  parent->add(phyVol0);
  
  GeoShape*   aShape1 = new GeoTube  (m_lp->vesselInnerRadius + m_lp->vesselInnerThickness, m_lp->vesselOuterRadMax, m_lp->bulkheadThickness/2);
  GeoLogVol*  logVol1 = new GeoLogVol("lvBulkHead1", aShape1, m_alu);
  GeoPhysVol* phyVol1 = new GeoPhysVol(logVol1);   
  
  parent->add(new GeoTransform(GeoTrf::Translate3D(0, 0, m_lp->vesselLength - m_lp->bulkheadThickness/2)));
  parent->add(new GeoNameTag("LucidBulkHead1"));
  parent->add(phyVol1);
}

void LUCID_DetectorFactory::addTubes(GeoPhysVol* parent) {
  
  MsgStream log(Athena::getMessageSvc(), "LUCID_DetectorFactory::addTubes");

  log << MSG::INFO << " LUCID_DetectorFactory::addTubes " << endmsg;
  
  GeoShape*  aShape1 = new GeoTube(m_lp->tubeRadius, m_lp->tubeRadius + m_lp->tubeThickness, m_lp->tubeLength/2);
  GeoShape*  aShape2 = new GeoTube(               0, m_lp->tubeRadius                      , m_lp->tubeLength/2);
  GeoShape*  aShape3 = new GeoTube(               0, m_lp->tubeRadius                      , m_lp->pmtThickness/2);
  
  GeoLogVol* logVol1 = new GeoLogVol("lvTube"   , aShape1, m_alu);
  GeoLogVol* logVol2 = new GeoLogVol("lvTubeGas", aShape2, m_gas);
  GeoLogVol* logVol3 = new GeoLogVol("lvPmt"    , aShape3, m_quartz);

  GeoPhysVol* phyVol3 = new GeoPhysVol(logVol3);
  
  GeoBorderSurfaceContainer* bsContainer = new GeoBorderSurfaceContainer();
  bsContainer->reserve(nLayers*nPmtTubesPerLayer + nFiberTubes);
  
  log << MSG::INFO << " LUCID_DetectorFactory::addPmtTubes " << endmsg;
  
  for (int lay=0; lay<nLayers; lay++) {
    for (int tub=0; tub<nPmtTubesPerLayer; tub++) {
      
      GeoOpticalPhysVol* phyVol1 = new GeoOpticalPhysVol(logVol1); 
      GeoOpticalPhysVol* phyVol2 = new GeoOpticalPhysVol(logVol2); 
      
      int tubeNumber = tub + lay*nPmtTubesPerLayer;
      
      log << MSG::DEBUG << " Buiding Cherenkov Tube: " << tubeNumber << endmsg;
      
      double x = m_tubePosition[lay][tub][0];
      double y = m_tubePosition[lay][tub][1];
      double r = sqrt(x*x+y*y);

      char sname[256]; 
      
      log << MSG::DEBUG << " Add PMT as daugther of the TubeGas " << endmsg;
      
      sprintf(sname, "LucidPmt%d", tubeNumber);
      phyVol2->add(new GeoNameTag  (sname));
      phyVol2->add(new GeoTransform(GeoTrf::Translate3D(0, 0, (m_lp->tubeLength - m_lp->pmtThickness)/2)));
      phyVol2->add(phyVol3);
      
      log << MSG::DEBUG << " Add Alluminium Tube as daugther of the VesselGas " << endmsg;

      sprintf(sname, "LucidTube%d", tubeNumber);
      parent->add(new GeoNameTag  (sname));
      parent->add(new GeoTransform(GeoTrf::Rotate3D   (m_tubeTheta[lay],GeoTrf:: Vector3D(-y/r, x/r, 0))));
      parent->add(new GeoTransform(GeoTrf::Translate3D(x, y, m_lp->vesselLength/2 + (m_lp->vesselLength - m_lp->tubeLength)/2 - m_lp->bulkheadThickness)));
      parent->add(phyVol1);
      
      log << MSG::DEBUG << " Add TubeGas as daugther of the VesselGas " << endmsg;

      sprintf(sname, "LucidTubeGas%d", tubeNumber);
      parent->add(new GeoNameTag  (sname));
      parent->add(new GeoTransform(GeoTrf::Rotate3D   (m_tubeTheta[lay], GeoTrf::Vector3D(-y/r, x/r, 0))));
      parent->add(new GeoTransform(GeoTrf::Translate3D(x, y, m_lp->vesselLength/2 + (m_lp->vesselLength - m_lp->tubeLength)/2 - m_lp->bulkheadThickness)));
      parent->add(phyVol2);
    
      log << MSG::DEBUG << " Add the Optical Surface between Gas and Tube physica volumes " << endmsg;
      
      sprintf(sname, "OptSurLayer%d", tubeNumber);
      bsContainer->push_back(GeoBorderSurface(sname, phyVol2, phyVol1, m_optSur));
    }
  }

  log << MSG::INFO << " LUCID_DetectorFactory::addFiberTubes " << endmsg;
  
  for (int tub=0; tub<nFiberTubes; tub++) {

    GeoOpticalPhysVol* phyVol1 = new GeoOpticalPhysVol(logVol1); 
    GeoOpticalPhysVol* phyVol2 = new GeoOpticalPhysVol(logVol2); 
    
    int tubeNumber = tub + 2*nPmtTubesPerLayer;
    
    log << MSG::DEBUG << " Buiding Cherenkov Tube: " << tubeNumber << endmsg;

    double x = m_tubePosition[0][0][0];
    double y = m_tubePosition[0][0][1];
    double r = sqrt(x*x + y*y);
    
    log << MSG::DEBUG << " Set Fiber-Readout Tubes on Inner layer at 45 degrees " << endmsg;
    
    x = y = r*cos(M_PI/4); 
    double r1 = sqrt(x*x + y*y);

    if (tubeNumber==17 || tubeNumber==18) x = -x;
    if (tubeNumber==18 || tubeNumber==19) y = -y;
    
    char sname[256]; 

    log << MSG::DEBUG << " Add PMT as daugther of the TubeGas " << endmsg;
    
    sprintf(sname, "LucidPmt%d", tubeNumber);
    phyVol2->add(new GeoNameTag  (sname));
    phyVol2->add(new GeoTransform(GeoTrf::Translate3D(0, 0, (m_lp->tubeLength - m_lp->pmtThickness)/2)));
    phyVol2->add(phyVol3);
    
    log << MSG::DEBUG << " Add Alluminium Tube as daugther of the VesselGas " << endmsg;

    sprintf(sname, "LucidTube%d", tubeNumber);
    parent->add(new GeoNameTag  (sname));
    parent->add(new GeoTransform(GeoTrf::Rotate3D   (m_tubeTheta[0], GeoTrf::Vector3D(-y/r1, x/r1, 0))));
    parent->add(new GeoTransform(GeoTrf::Translate3D(x, y, m_lp->vesselLength/2 + (m_lp->vesselLength - m_lp->tubeLength)/2 - m_lp->bulkheadThickness)));
    parent->add(phyVol1);
    
    log << MSG::DEBUG << " Add TubeGas as daugther of the VesselGas " << endmsg;

    sprintf(sname, "LucidTubeGas%d", tubeNumber);
    parent->add(new GeoNameTag  (sname));
    parent->add(new GeoTransform(GeoTrf::Rotate3D   (m_tubeTheta[0], GeoTrf::Vector3D(-y/r1, x/r1, 0))));
    parent->add(new GeoTransform(GeoTrf::Translate3D(x, y, m_lp->vesselLength/2 + (m_lp->vesselLength - m_lp->tubeLength )/2 - m_lp->bulkheadThickness)));    
    parent->add(phyVol2);
    
    log << MSG::DEBUG << " Add the Optical Surface between Gas and Tube physica volumes " << endmsg;
    
    sprintf(sname, "OptSurLayer%d", tubeNumber);
    bsContainer->push_back(GeoBorderSurface(sname, phyVol2, phyVol1, m_optSur));
  }
  
  StatusCode sc = m_detectorStore->record(bsContainer, "LUCID");
  
  if (sc.isFailure()) log << MSG::ERROR << "LUCID: unable to record border surface container to DS! " << endmsg; 
}

const LUCID_DetectorManager* LUCID_DetectorFactory::getDetectorManager() const { return m_detectorManager; }
