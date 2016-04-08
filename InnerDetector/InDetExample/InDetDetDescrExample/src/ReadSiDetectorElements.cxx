/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetDetDescrExample/ReadSiDetectorElements.h"

#include "StoreGate/StoreGateSvc.h"

#include "CLHEP/Units/SystemOfUnits.h"

#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetReadoutGeometry/SiDetectorManager.h"
#include "InDetReadoutGeometry/SiNumerology.h"
#include "InDetReadoutGeometry/SiCellId.h"
#include "InDetReadoutGeometry/SiIntersect.h"
#include "InDetIdentifier/PixelID.h"
#include "InDetIdentifier/SCT_ID.h"
#include "Identifier/Identifier.h"
#include "InDetCondServices/ISiLorentzAngleSvc.h"
#include "SiPropertiesSvc/ISiPropertiesSvc.h"
#include "InDetConditionsSummaryService/ISiliconConditionsSvc.h"
//#include "InDetReadoutGeometry/SCT_DetectorManager.h"
//#include "InDetReadoutGeometry/PixelDetectorManager.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "InDetReadoutGeometry/SiLocalPosition.h"


#include <iostream>
#include <vector>
#include <string>

using std::cout;
using std::endl;

using namespace InDetDD;
// or just the ones we need.
// using InDetDD::SiDetectorManager;
// using InDetDD::SiDetectorElement;
// using InDetDD::SiDetectorElementCollection;
// using InDetDD::SiLocalPosition;
// using InDetDD::SiCellId;
// using InDetDD::SiIntersect;

/////////////////////////////////////////////////////////////////////////////
//
// Prints out SiDetectorElement positions and other info.

/////////////////////////////////////////////////////////////////////////////

ReadSiDetectorElements::ReadSiDetectorElements(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  m_managerName("Pixel"),  // or SCT
  m_doLoop(true),
  m_geoModelSvc("GeoModelSvc", name),
  m_siLorentzAngleSvc("PixelLorentzAngleSvc",name),
  m_siConditionsSvc("PixelSiliconConditionsSvc",name),
  m_siPropertiesSvc("PixelSiPropertiesSvc",name),
  m_manager(0),
  m_idHelper(0),
  m_pixelIdHelper(0),
  m_sctIdHelper(0),
  m_first(true)
{  
  // Get parameter values from jobOptions file
  declareProperty("ManagerName", m_managerName);
  declareProperty("LoopOverElements", m_doLoop);
  declareProperty("DoInitialize", m_doInit = false);
  declareProperty("DoExecute",    m_doExec = true);
  declareProperty("GeoModelSvc", m_geoModelSvc);
  declareProperty("SiLorentzAngleSvc", m_siLorentzAngleSvc);
  declareProperty("SiConditionsSvc", m_siConditionsSvc);
  declareProperty("SiPropertiesSvc", m_siPropertiesSvc);
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode ReadSiDetectorElements::initialize(){
  msg(MSG::INFO) << "initialize()" << endreq; 
  // Retrieve GeoModel Detector Elements
  // You can either get the SCT or pixel manager or the common base class
  // manager. In this example I get the base class.
  //  const SiDetectorManager * manager;
  // or
  //  const PixelDetectorManager * manager;
  //  const SCT_DetectorManager  * manager;

  // GeoModelSvc
  if (m_geoModelSvc.retrieve().isFailure()) {
    msg(MSG::FATAL) << "Could not locate GeoModelSvc" << endreq;
    return StatusCode::FAILURE;
  }
  StatusCode sc;
  if(m_geoModelSvc->geoInitialized()) {
    msg(MSG::INFO) << "Geometry already initialized. Call geoInitialize."  << endreq;
    sc = geoInitialize();
  } else {
    msg(MSG::INFO) << "Geometry not yet initialized. Registering callback"  << endreq;
    // Register callback to check when TagInfo has changed
    sc =  detStore()->regFcn(&IGeoModelSvc::geoInit, &*m_geoModelSvc,&ReadSiDetectorElements::geoInitCallback, this);
    if (sc.isFailure()) {
      msg(MSG::ERROR) << "Cannot register geoInitCallback function "  << endreq;
    } else {
      msg(MSG::DEBUG) << "Registered geoInitCallback callback  " << endreq;
    }
  }
  return sc;
}


StatusCode ReadSiDetectorElements::geoInitCallback(IOVSVC_CALLBACK_ARGS){
  msg(MSG::INFO) <<"geoInitCallback is called" << endreq; 
  return geoInitialize();
}

StatusCode ReadSiDetectorElements::geoInitialize() {
  StatusCode sc = detStore()->retrieve(m_manager,m_managerName);
  if (sc.isFailure()) {
    msg(MSG::FATAL) << "Could not find the Manager: "    << m_managerName << " !" << endreq;
    return StatusCode::FAILURE;
  } else {
    msg(MSG::DEBUG) << "Manager found" << endreq;
  }

  if (m_managerName == "Pixel") {
    //
    // Get Pixel ID helper
    //
    // Pixel ID helper: const PixelID * m_pixelIdHelper;
    if (detStore()->retrieve(m_pixelIdHelper, "PixelID").isFailure()) {
      msg(MSG::FATAL) << "Could not get Pixel ID helper" << endreq;
      return StatusCode::FAILURE;
    }

    // If common pixel/SCT code can copy to pointer to AtlasDetectorID
    m_idHelper = m_pixelIdHelper;

  } else {   
    //
    // Get SCT ID helper
    //
    // SCT ID helper: const SCT_ID * m_sctIdHelper;
    if (detStore()->retrieve(m_sctIdHelper, "SCT_ID").isFailure()) {
      msg(MSG::FATAL) << "Could not get SCT ID helper" << endreq;
      return StatusCode::FAILURE;
    }

    // If common pixel/SCT code can copy to pointer to AtlasDetectorID
    m_idHelper = m_sctIdHelper;
  }
  sc = m_siLorentzAngleSvc.retrieve();
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Could not retrieve Lorentz Angle Svc: " << m_siLorentzAngleSvc.name() << endreq;
  }
  sc = m_siPropertiesSvc.retrieve();
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Could not retrieve silicon properties svc: " << m_siPropertiesSvc.name() << endreq;
  }
  sc = m_siConditionsSvc.retrieve();
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Could not retrieve silicon conditions service: " << m_siConditionsSvc.name() << endreq;
  }
  // Print during initialize
  if (m_doInit) {
    printAllElements();
    printRandomAccess();
  }
  return StatusCode::SUCCESS;
}


StatusCode ReadSiDetectorElements::execute() {
  msg(MSG::INFO) << "execute()" << endreq;
  // Only print out on first event
  if (m_first && m_doExec) {
    m_first = false;
    printAllElements();
    printRandomAccess();
  }
  return StatusCode::SUCCESS;
}

void ReadSiDetectorElements::printAllElements() {
  msg(MSG::INFO) << "printAllElements()" << endreq;
  // There are various ways you can access the elements. eg
  // m_manager->getDetectorElement(idHash);
  // m_manager->getDetectorElement(identifier);
  //
  // or access the whole collection or the iterators. 
  if (m_doLoop) {
    SiDetectorElementCollection::const_iterator iter;
    for (iter = m_manager->getDetectorElementBegin(); iter != m_manager->getDetectorElementEnd(); ++iter){
      const SiDetectorElement * element = *iter; 
      if (element) {
        cout << m_idHelper->show_to_string(element->identify()) << endl;;
        // The id helper is also available through  the elements
        //
        // element->getIdHelper()->show(element->identify());
        //
  
        cout << " center = " << element->center() << endl;
        cout << " sin(tilt), sin(stereo) = " <<  element->sinTilt() << " " 
             << element->sinStereo() << endl;
        cout << " width, minWidth, maxWidth, length (mm) = " 
             << element->width()/CLHEP::mm << " " 
             << element->minWidth()/CLHEP::mm << " " 
             << element->maxWidth()/CLHEP::mm << " " 
             << element->length()/CLHEP::mm << endl;
  
        // These are no longer accessed through the detector element.
        IdentifierHash hashId = element->identifyHash();
        cout << " Temperature (C), bias voltage, depletion voltage: "
             << m_siConditionsSvc->temperature(hashId) << " "
             << m_siConditionsSvc->biasVoltage(hashId) << " "
             << m_siConditionsSvc->depletionVoltage(hashId) << endl;
  
        //cout << "Via SiDetectorElement:"
        cout << " Lorentz correction (mm), tanLorentzPhi = "
             << element->getLorentzCorrection()/CLHEP::mm << " " 
             << element->getTanLorentzAnglePhi() << endl;
  
        //cout << "Direct from SiLorentzAngleSvc:"
        //cout << " Lorentz correction (mm), tanLorentzPhi = "
        //     << m_siLorentzAngleSvc->getLorentzShift(hashId)/CLHEP::mm << " "  
        //     << m_siLorentzAngleSvc->getTanLorentzAngle(hashId) << endl;
  
        // These are no longer accessed through the detector element.
        const InDet::SiliconProperties & siProperties =  m_siPropertiesSvc->getSiProperties(hashId);
        cout << " Hall Mobility (cm2/volt/s), Drift mobility (cm2/volt/s), diffusion constant (cm2/s) = " 
             << siProperties.hallMobility(element->carrierType()) /(CLHEP::cm2/CLHEP::volt/CLHEP::s) << " " 
             << siProperties.driftMobility(element->carrierType()) /(CLHEP::cm2/CLHEP::volt/CLHEP::s) << " " 
             << siProperties.diffusionConstant(element->carrierType()) /(CLHEP::cm2/CLHEP::s) << endl;
        //cout << element->hitDepthDirection() << " "
        //   << element->hitPhiDirection() << " "
        //   << element->hitEtaDirection() << " "
        //   << element->swapPhiReadoutDirection() << " "
        //   << element->swapEtaReadoutDirection() << endl;
        //cout << "isBarrel: " << element->isBarrel() << endl;

        cout << " HashId, Id : " << hashId << "\t" << element->identify().getString() << endl;

        // Make some consistency tests for the identifier.
        Identifier idTest;
        IdentifierHash idHashTest;
        if (m_pixelIdHelper) {
          const PixelID * idHelper = m_pixelIdHelper;
          idTest = idHelper->wafer_id(hashId);
          idHashTest = idHelper->wafer_hash(idTest);
        } else if (m_sctIdHelper) {
          const SCT_ID * idHelper = m_sctIdHelper;
          idTest = idHelper->wafer_id(hashId);
          idHashTest = idHelper->wafer_hash(idTest);
        }
        const SiDetectorElement * elementtest1 = m_manager->getDetectorElement(element->identify());
        const SiDetectorElement * elementtest2 = m_manager->getDetectorElement(hashId);
        bool idOK = true;
        if (idHashTest != hashId) {cout << " Id test 1 FAILED!" << endl; idOK = false;}
        if (idTest != element->identify()) {cout << " Id test 2 FAILED!" << endl; idOK = false;}
        if (elementtest1 != element) {cout << " Id test 3 FAILED!" << endl; idOK = false;}
        if (elementtest2 != element) {cout << " Id test 4 FAILED!" << endl; idOK = false;}
        if (idOK) cout << " ID tests OK" << std::endl;
      } else {
        //  cout << "Missing element!!!!!!!!!!!" << endl;
      }
    }
  }
  cout << endl;
  // Testing numerology
  const SiNumerology  siNumerology(m_manager->numerology());
  int nSides = 1;
  if (m_sctIdHelper) nSides = 2;
  int barrelCount = 0;
  int barrelCountError = 0;
  // Barrel
  for (int iBarrelIndex = 0; iBarrelIndex < siNumerology.numBarrels(); iBarrelIndex++) {
    int iBarrel = siNumerology.barrelId(iBarrelIndex);
    cout << "Barrel: " << iBarrel << endl;
    cout << " Num layers: " << siNumerology.numLayers() << endl;
    for (int iLayer = 0; iLayer < siNumerology.numLayers(); iLayer++) {
      cout << " Layer: " << iLayer << endl;
      if (!siNumerology.useLayer(iLayer)) cout << "  Layer not present" << endl;
      cout << "  Num Modules in Phi: " << siNumerology.numPhiModulesForLayer(iLayer) << endl;
      cout << "  Num Modules in Eta: " << siNumerology.numEtaModulesForLayer(iLayer) << endl;
      for (int iPhi = 0; iPhi < siNumerology.numPhiModulesForLayer(iLayer); iPhi++) {
        for (int iEta = siNumerology.beginEtaModuleForLayer(iLayer); iEta < siNumerology.endEtaModuleForLayer(iLayer); iEta++) {
          if (!iEta && siNumerology.skipEtaZeroForLayer(iLayer)) continue;
          for (int iSide = 0; iSide < nSides; iSide++) {
            Identifier id;
            if (m_pixelIdHelper){
              id = m_pixelIdHelper->wafer_id(iBarrel,iLayer,iPhi,iEta);
            } else {
              id = m_sctIdHelper->wafer_id(iBarrel,iLayer,iPhi,iEta,iSide);
            }
            const SiDetectorElement * element = m_manager->getDetectorElement(id);
            barrelCount++;
            if (!element) {
              barrelCountError++;
              cout << "   No element found for id: " << m_idHelper->show_to_string(id) << endl;
            } else {
            // For extra safety in case some strip modules do not have two sides (eg in future geometries) one could add.
              if (!element->otherSide()) iSide++;
              cout << "   " << m_idHelper->show_to_string(id) << endl;
            }     
          } // iSide
        } // iEta
      } //iPhi
    } //iLayer
  } // Barrel

  int endcapCount = 0;
  int endcapCountError = 0;
  // Endcap
  for (int iEndcapIndex = 0; iEndcapIndex < siNumerology.numEndcaps(); iEndcapIndex++) {
    int iEndcap = siNumerology.endcapId(iEndcapIndex);
    cout << "Endcap: " << iEndcap << endl;
    cout << " Num disks: " << siNumerology.numDisks() << endl;
    for (int iDisk = 0; iDisk < siNumerology.numDisks(); iDisk++) {
      cout << " Disk: " << iDisk << endl;
    if (!siNumerology.useDisk(iDisk)) cout << "  Disk not present" << endl;
      cout << "  Num Rings: " << siNumerology.numRingsForDisk(iDisk) << endl;
      for (int iEta = 0; iEta < siNumerology.numRingsForDisk(iDisk); iEta++) {
        cout << "  Ring: " << iEta << endl; 
        cout << "   Num Modules in Phi: " << siNumerology.numPhiModulesForDiskRing(iDisk,iEta) << endl;    
        for (int iPhi = 0; iPhi < siNumerology.numPhiModulesForDiskRing(iDisk,iEta); iPhi++) {
          for (int iSide = 0; iSide < nSides; iSide++) {
            Identifier id;
            if (m_pixelIdHelper){
              id = m_pixelIdHelper->wafer_id(iEndcap,iDisk,iPhi,iEta);
            } else {
              id = m_sctIdHelper->wafer_id(iEndcap,iDisk,iPhi,iEta,iSide);
            }
            const SiDetectorElement * element = m_manager->getDetectorElement(id);
            endcapCount++;
            if (!element) {
              endcapCountError++;
              cout << "    No element found for id: " << m_idHelper->show_to_string(id) << endl;
            } else {
            // For extra safety in case some strip modules do not have two sides (eg in future geometries) one could add.
              if (!element->otherSide()) iSide++;
              cout << "    " << m_idHelper->show_to_string(id) << endl;
            }
          } // iSide
        } // iEta
      } //iPhi
    } //iDisk
  } // Endcap;

  cout << "Number of barrel elements : " << barrelCount << endl;
  cout << "Number not found          : " << barrelCountError << endl;
  cout << "Number of endcap elements : " << endcapCount << endl;
  cout << "Number not found          : " << endcapCountError << endl;

  // Maximums 
  cout << "MaxNumBarrelEta:   " <<  siNumerology.maxNumBarrelEta() << endl;
  cout << "MaxNumEndcapRings: " <<  siNumerology.maxNumEndcapRings() << endl;
  cout << "MaxNumStrips:      " <<  siNumerology.maxNumStrips() << endl;
  cout << "MaxNumPhiCells:    " <<  siNumerology.maxNumPhiCells() << endl;
  cout << "MaxNumEtaCells:    " <<  siNumerology.maxNumEtaCells() << endl;

  cout << "Num Designs: " <<  m_manager->numDesigns() << endl;
}


void ReadSiDetectorElements::printRandomAccess() {
  msg(MSG::INFO) << "printRandomAccess()" << endreq;
  // Some random access
  if (m_manager->getName() == "Pixel") {
    //const PixelID * idHelper = dynamic_cast<const PixelID *>(m_manager->getIdHelper());
    const PixelID * idHelper = m_pixelIdHelper;
    if (idHelper) {
      Identifier id;
      std::vector<SiCellId> cellIds;
      std::vector<Amg::Vector2D> positions;
      // wafer_id(barrel_ec, layer_disk, phi_module, eta_module)
      // A barrel element
      cout << "----------------------------------------------" << endl;
      cout << " A Pixel Barrel element (non B-layer) "         << endl;
      cout << "----------------------------------------------" << endl;
      id = idHelper->wafer_id(0,1,15,-3);
      cellIds.push_back(SiCellId(32,8)); // phi,eta
      //add a range of cells from 151 to 175
      for (int i(151);i != 176; ++i){
        cellIds.push_back(SiCellId(i,8)); // phi,eta
      }
      cellIds.push_back(SiCellId(-1,1)); // phi,eta
      cellIds.push_back(SiCellId(0,1)); // phi,eta
      cellIds.push_back(SiCellId(1,-1)); // phi,eta
      cellIds.push_back(SiCellId(1,0)); // phi,eta
      cellIds.push_back(SiCellId(327,1)); // phi,eta
      cellIds.push_back(SiCellId(328,1)); // phi,eta
      cellIds.push_back(SiCellId(1,143)); // phi,eta
      cellIds.push_back(SiCellId(1,144)); // phi,eta
      positions.push_back(Amg::Vector2D(12.727*CLHEP::mm, 4.534*CLHEP::mm)); // eta,phi
      testElement(id, cellIds, positions);

      // A barrel element (B-Layer)
      cout << "----------------------------------------------" << endl;
      cout << " A Pixel Barrel element (B-layer)     "         << endl;
      cout << "----------------------------------------------" << endl;
      id = idHelper->wafer_id(0,0,7,-3);
      cellIds.clear();
      positions.clear();
      cellIds.push_back(SiCellId(32,8)); // phi,eta
      positions.push_back(Amg::Vector2D(12.727*CLHEP::mm, 4.534*CLHEP::mm)); // eta,phi
      testElement(id, cellIds, positions);

      // An endcap element
      cout << "----------------------------------------------" << endl;
      cout << " A Pixel Endcap element"                        << endl;
      cout << "----------------------------------------------" << endl;
      id = idHelper->wafer_id(2,2,13,0);
      cellIds.push_back(SiCellId(182,75)); // phi,eta
      positions.push_back(Amg::Vector2D(0*CLHEP::mm, 0*CLHEP::mm)); // eta,phi
      positions.push_back(Amg::Vector2D(30.4*CLHEP::mm, 8.2*CLHEP::mm)); // eta,phi - on edge
      positions.push_back(Amg::Vector2D(12*CLHEP::mm, -8.15*CLHEP::mm)); // eta,phi - near edge
      positions.push_back(Amg::Vector2D(12*CLHEP::mm, -8.25*CLHEP::mm)); // eta,phi - near edge
      positions.push_back(Amg::Vector2D(12*CLHEP::mm, -8.35*CLHEP::mm)); // eta,phi - outside
      testElement(id, cellIds, positions);

    }
  } else if (m_manager->getName() == "SCT") {
    
    //const SCT_ID * idHelper = dynamic_cast<const SCT_ID *>(m_manager->getIdHelper());
    const SCT_ID * idHelper = m_sctIdHelper;
    if (idHelper) {

      Identifier id;
      std::vector<SiCellId> cellIds;
      std::vector<Amg::Vector2D> positions;


      // wafer_id(barrel_ec, layer_disk, phi_module, eta_module, side)
      // A barrel element
      cout << "----------------------------------------------" << endl;
      cout << " A SCT Barrel element"                          << endl;
      cout << "----------------------------------------------" << endl;
      id = idHelper->wafer_id(0,1,15,-3,0);
      cellIds.clear();
      positions.clear();
      cellIds.push_back(SiCellId(32)); // phi,eta
      cellIds.push_back(SiCellId(1)); // phi,eta
      cellIds.push_back(SiCellId(0)); // phi,eta
      cellIds.push_back(SiCellId(-1)); // phi,eta
      cellIds.push_back(SiCellId(-2)); // phi,eta
      cellIds.push_back(SiCellId(-3)); // phi,eta
      cellIds.push_back(SiCellId(767)); // phi,eta
      cellIds.push_back(SiCellId(768)); // phi,eta
      positions.push_back(Amg::Vector2D(12.727*CLHEP::mm, 4.534*CLHEP::mm)); // eta,phi
      testElement(id, cellIds, positions);

      // A barrel element (other side of above)
      cout << "----------------------------------------------" << endl;
      cout << " A SCT Barrel element (other side of above)   " << endl;
      cout << "----------------------------------------------" << endl;
      id = idHelper->wafer_id(0,1,15,-3,1);
      cellIds.clear();
      positions.clear();
      cellIds.push_back(SiCellId(32)); // phi,eta
      positions.push_back(Amg::Vector2D(12.727*CLHEP::mm, 4.534*CLHEP::mm)); // eta,phi
      testElement(id, cellIds, positions);

      // A outer fwd
      cout << "----------------------------------------------" << endl;
      cout << " A SCT Endcap element (outer type)"             << endl;
      cout << "----------------------------------------------" << endl;
      id = idHelper->wafer_id(2,3,15,0,0);
      cellIds.clear();
      positions.clear();
      cellIds.push_back(SiCellId(532)); // phi,eta
      cellIds.push_back(SiCellId(0)); // phi,eta
      cellIds.push_back(SiCellId(-1)); // phi,eta
      cellIds.push_back(SiCellId(767)); // phi,eta
      cellIds.push_back(SiCellId(768)); // phi,eta
      positions.push_back(Amg::Vector2D(12.727*CLHEP::mm, 20.534*CLHEP::mm)); // eta,phi
      positions.push_back(Amg::Vector2D(12.727*CLHEP::mm, -20.534*CLHEP::mm)); // eta,phi
      positions.push_back(Amg::Vector2D(3*CLHEP::mm, -25*CLHEP::mm)); // eta,phi
      testElement(id, cellIds, positions);

      cout << "----------------------------------------------" << endl;
      cout << " A SCT Endcap element (outer type) other side"             << endl;
      cout << "----------------------------------------------" << endl;
      id = idHelper->wafer_id(2,3,15,0,1);
      cellIds.clear();
      positions.clear();
      cellIds.push_back(SiCellId(532)); // phi,eta
      positions.push_back(Amg::Vector2D(12.727*CLHEP::mm, 20.534*CLHEP::mm)); // eta,phi
      positions.push_back(Amg::Vector2D(12.727*CLHEP::mm, -20.534*CLHEP::mm)); // eta,phi
      positions.push_back(Amg::Vector2D(3*CLHEP::mm, -25*CLHEP::mm)); // eta,phi
      testElement(id, cellIds, positions);

      // A middle fwd
      cout << "----------------------------------------------" << endl;
      cout << " A SCT Endcap element (middle type)"            << endl;
      cout << "----------------------------------------------" << endl;
      id = idHelper->wafer_id(2,1,15,1,0);
      cellIds.clear();
      positions.clear();
      cellIds.push_back(SiCellId(532)); // phi,eta
      positions.push_back(Amg::Vector2D(12.727*CLHEP::mm, 4.534*CLHEP::mm)); // eta,phi
      testElement(id, cellIds, positions);

      // A truncated middle
      cout << "----------------------------------------------" << endl;
      cout << " A SCT Endcap element (truncated middle type)"  << endl;
      cout << "----------------------------------------------" << endl;
      id = idHelper->wafer_id(2,7,15,1,0);
      cellIds.clear();
      positions.clear();
      cellIds.push_back(SiCellId(532)); // phi,eta
      positions.push_back(Amg::Vector2D(12.727*CLHEP::mm, 4.534*CLHEP::mm)); // eta,phi
      testElement(id, cellIds, positions);

      // A inner fwd
      cout << "----------------------------------------------" << endl;
      cout << " A SCT Endcap element (inner type)"             << endl;
      cout << "----------------------------------------------" << endl;
      id = idHelper->wafer_id(2,1,15,2,0);
      cellIds.clear();
      positions.clear();
      cellIds.push_back(SiCellId(532)); // phi,eta
      positions.push_back(Amg::Vector2D(12.727*CLHEP::mm, 4.534*CLHEP::mm)); // eta,phi
      testElement(id, cellIds, positions);
    }
  } // if manager = Pixel,SCT
} 


void
ReadSiDetectorElements::testElement(const Identifier & id, 
            const std::vector<SiCellId> & cellIdVec, 
            const std::vector<Amg::Vector2D> & positionsVec) const{
  cout << "----------------------------------------------" << endl;
  const SiDetectorElement * element = m_manager->getDetectorElement(id);
  if (element) {
    IdentifierHash hashId = element->identifyHash();
    cout << element->getIdHelper()->show_to_string(id) << endl;
    cout << " width, minWidth, maxWidth, length, thickness (mm) = " 
   << element->width()/CLHEP::mm << " " 
   << element->minWidth()/CLHEP::mm << " " 
   << element->maxWidth()/CLHEP::mm << " " 
   << element->length()/CLHEP::mm << " "
   << element->thickness()/CLHEP::mm
   << endl;
    cout << " average etaPitch = " << element->etaPitch()/CLHEP::micrometer << " microns" << endl;
    cout << " average phiPitch = " << element->phiPitch()/CLHEP::micrometer << " microns" << endl;
    cout << " rMin, rMax, zMin, zMax (mm), phiMin, phiMax (deg) = " 
   << element->rMin()/CLHEP::mm << " "
   << element->rMax()/CLHEP::mm << " "
   << element->zMin()/CLHEP::mm << " "
   << element->zMax()/CLHEP::mm << " "
   << element->phiMin()/CLHEP::degree << " "
   << element->phiMax()/CLHEP::degree
   << endl;
    cout << " center, normal, etaAxis, phiAxis = "
   << element->center() << " "
   << element->normal() << " "
   << element->etaAxis() << " "
   << element->phiAxis() 
   << endl;
    cout << " center: r (mm) = " <<  element->center().perp()/CLHEP::mm 
   << ", phi (deg) = " <<  element->center().phi()/CLHEP::deg << endl;
    const InDet::SiliconProperties & siProperties =  m_siPropertiesSvc->getSiProperties(hashId);
    cout << " Lorentz correction (mm), mobility (cm2/V/s), tanLorentzPhi = "
   << element->getLorentzCorrection()/CLHEP::mm << " " 
   << siProperties.hallMobility(element->carrierType()) /(CLHEP::cm2/CLHEP::volt/CLHEP::s) << " " 
   << element->getTanLorentzAnglePhi() << endl;
    cout << " Temperature (C), bias voltage, depletion voltage: " 
   << m_siConditionsSvc->temperature(hashId) << " "
   << m_siConditionsSvc->biasVoltage(hashId) << " "
   << m_siConditionsSvc->depletionVoltage(hashId) << endl;
    cout << " sin(tilt), tilt (deg), sin(stereo), stereo (deg) = " 
   << element->sinTilt() << ", " 
   << asin(element->sinTilt())/CLHEP::degree << ", "
   << element->sinStereo() << ", " 
   << asin(element->sinStereo())/CLHEP::degree << endl;
    cout << " Neighbours: " << endl;
    cout << "  nextInEta: " << printElementId(element->nextInEta())  << endl;
    cout << "  prevInEta: " << printElementId(element->prevInEta())  << endl;
    cout << "  nextInPhi: " << printElementId(element->nextInPhi())  << endl;
    cout << "  prevInPhi: " << printElementId(element->prevInPhi())  << endl;
    cout << "  otherSide: " << printElementId(element->otherSide())  << endl;

    for (unsigned int iTestCell = 0; iTestCell < cellIdVec.size(); iTestCell++) {
      SiCellId cellId = cellIdVec[iTestCell];
      cout << endl;
      cout << " cell [phiIndex.etaIndex] = " << cellId << endl;
      
      // Test cell Id -> Identifier
      Identifier fullCellId = element->identifierFromCellId(cellId);
      cout << " identifier = ";
      element->getIdHelper()->show(fullCellId);
      
      // Test Identifier -> cell Id 
      SiCellId cellId2 = element->cellIdFromIdentifier(fullCellId);
      cout << " extracted cell id [phiIndex.etaIndex] = " << cellId2 << endl;
      
      InDetDD::SiLocalPosition localPosRaw1 = element->rawLocalPositionOfCell(cellId);
      InDetDD::SiLocalPosition localPosRaw2 = element->rawLocalPositionOfCell(fullCellId);
      InDetDD::SiLocalPosition localPos1 = element->localPositionOfCell(cellId);
      InDetDD::SiLocalPosition localPos2 = element->localPositionOfCell(fullCellId);
      cout << " raw localPosition (using cell id) (xPhi,xEta) = " 
     << localPosRaw1.xPhi() << ", " << localPosRaw1.xEta() << endl;
      cout << " raw localPosition (using full id) (xPhi,xEta) = " 
     << localPosRaw2.xPhi() << ", " << localPosRaw2.xEta() << endl;
      SiCellId cellIdRaw(element->cellIdOfPosition(localPosRaw1));
      cout << " corresponding cell (phiIndex,etaIndex) = " 
     << cellIdRaw << endl; 
      cout << " lorentz corrected localPosition (using cell id) (xPhi,xEta) = " 
     << localPos1.xPhi() << ", " << localPos1.xEta() << endl;
      cout << " lorentz corrected localPosition (using cell id) (xPhi,xEta) = " 
     << localPos2.xPhi() << ", " << localPos2.xEta() << endl;
      SiCellId cellIdNew(element->cellIdOfPosition(localPos1));
      cout << " corresponding cell (phiIndex,etaIndex) = " 
     << cellIdNew << endl; 
      cout << " Number of connected cells (2 means ganged): " 
     << element->numberOfConnectedCells(cellId) << endl;
      cout << " Connected cells";
      for (int iCell=0; iCell < element->numberOfConnectedCells(cellId) ; iCell++) {
        SiCellId connectedCellId =  element->connectedCell(cellId, iCell);
        cout << ", ";
        cout << iCell << ": " << connectedCellId;
      }
      cout << endl; 
      cout << "In range: " << element->design().cellIdInRange(cellId) << endl;
    }
    
    for (unsigned int iTestPos = 0; iTestPos < positionsVec.size(); iTestPos++) {
      const InDetDD::SiLocalPosition & localPosOrig = positionsVec[iTestPos];
      cout << endl;
      cout << " Requested local pos (xPhi,xEta) = " << localPosOrig.xPhi() << ", " << localPosOrig.xEta() << endl;
      //lost out to HepGeom here
      HepGeom::Point3D<double> globalPos(element->globalPositionCLHEP(localPosOrig));
      cout << " Global pos = " << globalPos << ", r (mm) = " << globalPos.perp()/CLHEP::mm<< ", phi (deg) = " << globalPos.phi()/CLHEP::degree << endl;

      //...because i need a HepGeom::Point3D<double> to pass to element->localPosition...
      InDetDD::SiLocalPosition localPosNew(element->localPosition(globalPos));
      cout << " Returned local Pos (xPhi,xEta) =  " << localPosNew.xPhi() << ", " << localPosNew.xEta() << endl;
      // Some arbitrary tolerance picked out of the air.
      double tolerance = 100*CLHEP::micrometer;
      SiIntersect intersectState = element->inDetector(globalPos, tolerance, tolerance);
      cout << " Intersects (tolerance = " << tolerance/CLHEP::mm << " mm) " 
      << " (in,out,nearBoundary,mayIntersect) : " 
     << intersectState.in() << ","
     << intersectState.out() << ","
     << intersectState.nearBoundary() << ","
     << intersectState.mayIntersect() << endl;
      cout << " Near bond gap: (tolerance = " << tolerance/CLHEP::mm << " mm) : " 
     <<  element->nearBondGap(globalPos, tolerance) << endl;
      SiCellId returnedCellId = element->cellIdOfPosition(localPosNew);
      //      cout << " Returned cell Id (phiIndex,etaIndex) = " 
      //     << returnedCellId.phiIndex() << ", " << returnedCellId.etaIndex() << endl; 
      cout << " Returned cell Id [phiIndex.etaIndex] = " 
     << returnedCellId << endl; 
      cout << " using global position sin(tilt), tilt (deg), sin(stereo), stereo (deg) = "
     << element->sinTilt(globalPos) << ", "
     << asin(element->sinTilt(globalPos))/CLHEP::degree << ", "
     << element->sinStereo(globalPos) << ", "
     << asin(element->sinStereo(globalPos))/CLHEP::degree << endl;
    }   
  } else { // element == 0
  
    cout << " ELEMENT MISSING!!!!!!!!!! " << endl;
  }
  cout << "----------------------------------------------" << endl;
}

std::string 
ReadSiDetectorElements::printElementId(const SiDetectorElement * element) const{
  static std::string noElementString("NONE");
  if (element) {
    return element->getIdHelper()->show_to_string(element->identify());
  } else {
    return noElementString;
  }
}
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode ReadSiDetectorElements::finalize() {
  // Part 1: Get the messaging service, print where you are
  msg(MSG::INFO) << "finalize()" << endreq;
  return StatusCode::SUCCESS;
}




