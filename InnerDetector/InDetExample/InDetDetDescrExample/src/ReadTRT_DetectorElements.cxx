/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetDetDescrExample/ReadTRT_DetectorElements.h"

#include "CLHEP/Units/SystemOfUnits.h"

#include "TRT_ReadoutGeometry/TRT_BarrelElement.h"
#include "TRT_ReadoutGeometry/TRT_EndcapElement.h"
#include "InDetIdentifier/TRT_ID.h"
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"
#include "TRT_ReadoutGeometry/TRT_DetectorManager.h"
#include "TRT_ReadoutGeometry/TRT_Numerology.h"

#include <iostream>
#include <vector>


using std::cout;
using std::endl;
using namespace InDetDD;

namespace{
  const Amg::Vector3D origin(0., 0., 0.); 
}

// Prints out TRT DetectorElement positions.

/////////////////////////////////////////////////////////////////////////////

ReadTRT_DetectorElements::ReadTRT_DetectorElements(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  m_managerName("TRT"),
  m_manager(nullptr),
  m_idHelper(nullptr),
  m_maxdiff(0),
  m_first(true)
{  
  // Get parameter values from jobOptions file
  declareProperty("ManagerName",  m_managerName);
  declareProperty("DoInitialize", m_doInit = true);
  declareProperty("DoExecute",    m_doExec = false);
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode ReadTRT_DetectorElements::initialize(){
  msg(MSG::INFO) << "initialize()" << endmsg;  
  // Get the manager
  StatusCode sc=detStore()->retrieve(m_manager, m_managerName);
  if (sc.isFailure() || !m_manager) {
    msg(MSG::FATAL) << "Could not find the Manager: "<< m_managerName << " !" << endmsg;
    return StatusCode::FAILURE;
  } else {
    msg(MSG::DEBUG) << "Manager found" << endmsg;
  }
  // Get ID helper
  if (detStore()->retrieve(m_idHelper, "TRT_ID").isFailure()) {
    msg(MSG::FATAL) << "Could not get TRT ID helper" << endmsg;
    return StatusCode::FAILURE;
  }
  if (m_doInit) printAllElements();
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode ReadTRT_DetectorElements::execute() {
  msg(MSG::INFO) << "execute()" << endmsg;
  // Only print out on first event
  if (m_first && m_doExec) {
    m_first = false;
    printAllElements();
  }
  return StatusCode::SUCCESS;
}

void ReadTRT_DetectorElements::printAllElements(){
  // Retrieve GeoModel Detector Elements
  // Get the id helper
  const TRT_ID * idHelper = m_idHelper;
  // Examples are shown below for accessing via index for barrel, then endcap and
  // then accessing via hash
  //
  // Access via indices.
  //
  //
  //
  cout << "======================================" <<endl;
  cout << " BARREL ELEMENTS - access via index" << endl;
  cout << "======================================" <<endl;
  cout << endl;
  //cache numerology
  const TRT_Numerology * numerology=m_manager->getNumerology();
  //avoid getting these inside loops
  const unsigned int nBarrelRings(numerology->getNBarrelRings());
  const unsigned int nBarrelPhi(numerology->getNBarrelPhi());
  
  for (int iSide = 0; iSide < 2; iSide++) {
    for (unsigned int iLayer = 0; iLayer != nBarrelRings; iLayer++) {
      const unsigned int nBarrelLayers(numerology->getNBarrelLayers(iLayer));
      for (unsigned int iPhiModule = 0; iPhiModule != nBarrelPhi; iPhiModule++) {
        for (unsigned int iStrawLayer = 0; iStrawLayer !=nBarrelLayers; iStrawLayer++) {
          cout << "Barrel element " << iSide << ", " << iLayer << ", " << iPhiModule << ", "<< iStrawLayer << endl;
          const TRT_BarrelElement *element = m_manager->getBarrelElement(iSide, iLayer, iPhiModule, iStrawLayer);
          // Make identifier.
          Identifier newId = idHelper->layer_id((iSide ? 1:-1), iPhiModule, iLayer, iStrawLayer);
          idHelper->show(newId);
          IdentifierHash idHash = idHelper->straw_layer_hash(newId);
          cout << "idHash = " << idHash << endl;
          if (element) {
            Amg::Vector3D center = element->transform() * origin;
            cout << " center = " << center << endl;
            //cout << " strawDir = " << element->strawDirection() << endl;
            cout << " number of straws = " << element->nStraws() << endl;
            // Print out straw details of some elements
            if ((iPhiModule == 0) || (iPhiModule == 1) || (iPhiModule == 7) ||  (iPhiModule == 31)) {
              for (unsigned int istraw = 0; istraw < 15; istraw++) {
                printStraw(element, istraw);
                //printBarrelStraw(element, istraw);
              }
            }
            // Compare methods 
            for (unsigned int istraw = 0; istraw < element->nStraws(); istraw++) {
              compareBarrelStraw(element, istraw);
            }
          } else {
            cout << "No Element found " << endl;
          }
        }
      }
    }
  }

  //
  //
  cout << "======================================" <<endl;
  cout << " ENDCAP ELEMENTS - access via index" << endl;
  cout << "======================================" <<endl;
  cout << endl;
  const unsigned int nWheels(numerology->getNEndcapWheels());
  const unsigned int nEndcapPhi(numerology->getNEndcapPhi());
  for (int iSide = 0; iSide < 2; iSide++) {
    for (unsigned int iWheel = 0; iWheel !=nWheels ; iWheel++) {
      const unsigned int nEndcapLayers(numerology->getNEndcapLayers(iWheel));
      for (unsigned int iStrawLayer = 0; iStrawLayer != nEndcapLayers; iStrawLayer++){
        for(unsigned int iPhi = 0; iPhi != nEndcapPhi; iPhi++){
          cout << "Endcap element " << iSide << ", "<< iWheel << ", "<< iStrawLayer << ", "<< iPhi << endl;
          const TRT_EndcapElement * element = m_manager->getEndcapElement(iSide, iWheel, iStrawLayer, iPhi);

          // Make identifier.
          Identifier newId = idHelper->layer_id((iSide ? 2:-2), iPhi, iWheel, iStrawLayer);
          idHelper->show(newId);
          IdentifierHash idHash = idHelper->straw_layer_hash(newId);
          cout << "idHash = " << idHash << endl;
  
          if (element) {
            Amg::Vector3D center = element->transform() * origin;
            cout << " center = " << center << endl;
            cout << " number of straws = " << element->nStraws() << endl;
            // Print out straw details of some elements
            int iPhiModule = iPhi;
            if ((iPhiModule == 0) || (iPhiModule == 1) || (iPhiModule == 7) ||  (iPhiModule == 31)) {
              for (unsigned int istraw = 0; istraw < 15; istraw++) {
                printStraw(element, istraw);
                //printEndcapStraw(element, istraw);
              }
            }
            // Compare methods 
            for (unsigned int istraw = 0; istraw < element->nStraws(); istraw++) {
              compareEndcapStraw(element, istraw);
            }
          } else {
            cout << "No Element found " << endl;
          }
        }
      }
    }
  }



  unsigned int maxHash = idHelper->straw_layer_hash_max();

  //
  // Try and iterate over all elements. 
  // There is no iterator but we can get the elements via the idHash.
  //

  cout << "======================================" <<endl;
  cout << " ALL ELEMENTS - access via hash" << endl;
  cout << "======================================" <<endl;
  cout << endl;

  cout << " maxHash = " << maxHash << endl;

  for (unsigned int index = 0; index < maxHash; index++) {

    IdentifierHash idHash = index;

    Identifier id = idHelper->layer_id(idHash);
    idHelper->show(id);

    const TRT_BaseElement * element;

    element = m_manager->getElement(idHash);
   
    if (element) {
      Amg::Vector3D center = element->transform() * origin;
      cout << " center = " << center << endl;
    } else {
      cout << "No Element with id hash = " << idHash << endl;
    }
  }

  
  cout << "======================================" <<endl;
  cout << " ALL ELEMENTS - access via index from hash" << endl;
  cout << " (no printout) " << endl;
  cout << "======================================" <<endl;
  cout << endl;  for (unsigned int index = 0; index < maxHash; index++) {

    IdentifierHash idHash = index;

    Identifier id = idHelper->layer_id(idHash);
    //idHelper->show(id);


    int idBarrelEndcap = idHelper->barrel_ec(id);
    int idSide = (idBarrelEndcap > 0);
    int idLayerWheel = idHelper->layer_or_wheel(id);
    int idPhiModule = idHelper->phi_module(id);
    int idStrawLayer = idHelper->straw_layer(id);

    // Make identifier again,
    //    Identifier newId = idHelper->layer_id(idBarrelEndcap, idPhiModule, idLayerWheel, idStrawLayer);
    // idHelper->show(newId);

       
    const TRT_BaseElement * element;
    
    
    if (idHelper->is_barrel(id)) {
      element = m_manager->getBarrelElement(idSide, idLayerWheel, idPhiModule, idStrawLayer); 
    } else {
      element = m_manager->getEndcapElement(idSide, idLayerWheel, idStrawLayer, idPhiModule); 
    }
    
    if (element) {
      //nop
      //Amg::Vector3D center = element->transform() * origin;
      //cout << " center = " << center << endl;
    } else {
      cout << "No Element with id hash = " << idHash << endl;
    }
  }
 
  cout << endl;

  cout << "Max diff = " << m_maxdiff << endl;

} 



// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode ReadTRT_DetectorElements::finalize() {
  msg(MSG::INFO) << "finalize()" << endmsg;
  return StatusCode::SUCCESS;
}


void
ReadTRT_DetectorElements::printBarrelStraw(const TRT_BarrelElement * element, unsigned int strawNum){
  // It is recommended to use the methods in printStraw() rather than the ones here
  if (strawNum >= element->nStraws()) {
    cout << "Straw number out of range:" << strawNum << " >= " << element->nStraws() << endl;
    return;
  }
  
  double x = element->strawXPos(strawNum);
  double y = element->strawYPos(strawNum);
  double z = element->strawZPos(strawNum);
  double r = sqrt(x*x+y*y);
  double phi = atan2(y,x);
  cout << "strawNum = " << strawNum << ", "
       << "x = " << x << ", "
       << "y = " << y << ", "
       << "z = " << z << ", "
       << "r = " << r << ", "
       << "phi = " << phi << ", "
       << "length = " << element->strawLength() 
       << endl;
 
  
}

void
ReadTRT_DetectorElements::printEndcapStraw(const TRT_EndcapElement * element, unsigned int strawNum)
{
  // It is recommended to use the methods in printStraw() rather than the ones here

  if (strawNum >= element->nStraws()) {
    cout << "Straw number out of range:" << strawNum << " >= " << element->nStraws() << endl;
    return;
  }

  double phiStart = element->getDescriptor()->startPhi();
  double pitch    = element->getDescriptor()->strawPitch();
  double phi = phiStart + strawNum*pitch;
  phi = atan2(sin(phi), cos(phi)); // Get phi b/n -pi and pi. 
  double z = (element->transform() * origin).z();
  cout << "strawNum = " << strawNum << ", "
       << "phiStart = " << phiStart << ", "
       << "pitch = " << pitch << ", "
       << "z = " << z << ", "
       << "r = " << element->getDescriptor()->innerRadius() + 0.5 * element->getDescriptor()->strawLength() << ", "
       << "phi = " << phi << ", "
       << "length = " << element->getDescriptor()->strawLength()
       << endl;

}



void
ReadTRT_DetectorElements::printStraw(const TRT_BaseElement * element, unsigned int strawNum)
{  
  if (strawNum >= element->nStraws()) {
    cout << "Straw number out of range:" << strawNum << " >= " << element->nStraws() << endl;
    return;
  }

  const Amg::Transform3D& transform = element->strawTransform(strawNum);
  Amg::Vector3D strawCenter = transform * origin;
  cout << "strawNum = " << strawNum << ", "
       << "x = " << strawCenter.x() << ", "
       << "y = " << strawCenter.y() << ", "
       << "z = " << strawCenter.z() << ", "
       << "r = " << strawCenter.perp() << ", " 
       << "phi = " << strawCenter.phi() << ", " 
       << "length = " << element->strawLength() << ", "
       << "localAxis = " << transform * Amg::Vector3D(0,0,1) 
       << endl;
}

void
ReadTRT_DetectorElements::compareBarrelStraw(const TRT_BarrelElement * element, unsigned int strawNum){
  if (strawNum >= element->nStraws()) {
    cout << "Straw number out of range:" << strawNum << " >= " << element->nStraws() << endl;
    return;
  }
  
  double x = element->strawXPos(strawNum);
  double y = element->strawYPos(strawNum);
  double z = element->strawZPos(strawNum);

  Amg::Transform3D transform = element->strawTransform(strawNum);
  Amg::Vector3D strawPos = transform * origin;
  
  if (!comparePos(strawPos, Amg::Vector3D(x,y,z), 1e-9)) {
    cout << "Straw Positions DIFFER !!!!!" << endl;
    printStraw(element, strawNum);
    printBarrelStraw(element, strawNum);
  }
}

void
ReadTRT_DetectorElements::compareEndcapStraw(const TRT_EndcapElement * element, unsigned int strawNum){
  if (strawNum >= element->getDescriptor()->nStraws()) {
    cout << "Straw number out of range:" << strawNum << " >= " << element->getDescriptor()->nStraws() << endl;
    return;
  }
  double phiStart = element->getDescriptor()->startPhi();
  double pitch    = element->getDescriptor()->strawPitch();
  double phi = phiStart + strawNum*pitch;
  double r = element->getDescriptor()->innerRadius() + 0.5 * element->getDescriptor()->strawLength();
  double z = (element->transform() * origin).z();
 
  Amg::Transform3D transform = element->strawTransform(strawNum);
  Amg::Vector3D strawPos = transform * origin;
 
  if (!comparePos(strawPos, Amg::Vector3D(r*cos(phi), r*sin(phi), z), 1e-5)) {
    cout << "Straw Positions DIFFER !!!!!" << endl;
    printStraw(element, strawNum);
    printEndcapStraw(element, strawNum);
  }
}


bool 
ReadTRT_DetectorElements::comparePos(const Amg::Vector3D & p1, const Amg::Vector3D & p2, double err) {
  for (int i = 0; i < 3; i++) {
    double diff = std::abs(p1[i] - p2[i]);
    m_maxdiff = std::max(diff,m_maxdiff);
    if (diff > err) return false;
  }
  return true;
}
