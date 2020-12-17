/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetDetDescrExample/TestTRT_Alignment.h"

#include "CLHEP/Units/SystemOfUnits.h"

#include "InDetIdentifier/TRT_ID.h"
#include "Identifier/Identifier.h"
#include "TRT_ReadoutGeometry/TRT_DetectorManager.h"
#include "ReadoutGeometryBase/InDetDD_Defs.h"

#include <iostream>
#include <vector>
#include <string>

using std::cout;
using std::endl;

using namespace InDetDD;

//anonymous namespace for utility functions at file scope
namespace{
  Amg::Transform3D zRotation(const double angle){
    const Amg::Vector3D zAxis(0., 0., 1.);
    return Amg::Transform3D(Amg::AngleAxis3D(angle,zAxis));
  }
  Amg::Transform3D yTranslation(const double distance){
    return Amg::Transform3D(Amg::Translation3D(0., distance, 0.)); 
  }
}

/////////////////////////////////////////////////////////////////////////////
//
// Prints out SiDetectorElement positions and other info.

/////////////////////////////////////////////////////////////////////////////

TestTRT_Alignment::TestTRT_Alignment(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  m_manager(0),
  m_idHelper(0)
{  
  // Get parameter values from jobOptions file
  declareProperty("ManagerName", m_managerName = "TRT");
  declareProperty("LongPrintOut", m_longPrintOut = false);
  declareProperty("TestAllStraws", m_testAllStraws = false);
  declareProperty("TestAllElements", m_testAllElements = false);
  declareProperty("ErrorRotation",m_errRot = 1e-15);      // For testing if alignment changed
  declareProperty("ErrorTranslation",m_errTrans = 1e-12); // For testing if alignment changed
  declareProperty("HardwiredShifts", m_hardwiredShifts = false);
  declareProperty("Precision", m_precision = 6);
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode TestTRT_Alignment::initialize(){
  msg(MSG::INFO) << "initialize()" << endmsg;
  msg(MSG::INFO) << "Algorithm Properties" << endmsg;
  msg(MSG::INFO) << " ManagerName:      " << m_managerName << endmsg;  
  msg(MSG::INFO) << " LongPrintOut:     " << (m_longPrintOut ? "true" : "false") << endmsg;  
  msg(MSG::INFO) << " TestAllElements:  " << (m_testAllElements ? "true" : "false") << endmsg;  
  msg(MSG::INFO) << " TestAllStraws:    " << (m_testAllStraws ? "true" : "false") << endmsg;  
  msg(MSG::INFO) << " ErrorRotation:    " << m_errRot << endmsg;  
  msg(MSG::INFO) << " ErrorTranslation: " << m_errTrans << endmsg;  
  msg(MSG::INFO) << " Precision:        " << m_precision << endmsg;  
  if (!m_testAllElements) {
    msg(MSG::INFO) << endmsg;
    msg(MSG::INFO) << " Modules only - the 4 corner straws of each module will be printed. " << endmsg;  
    if (m_testAllStraws) {
      msg(MSG::INFO) << " NB. TestAllStraws flag is ignored" << endmsg;
    }
  }
  // Retrieve GeoModel Detector Elements
  // You can either get the SCT or pixel manager or the common base class
  // manager. In this example I get the base class.
  //  const SiTrackerDetectorManager * manager;
  // or
  //  const PixelDetectorManager * manager;
  //  const SCT_DetectorManager  * manager;
  // const SiDetectorManager * manager;
  StatusCode sc = detStore()->retrieve(m_manager, m_managerName);
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
  if (m_hardwiredShifts) {
    printAlignmentShifts();
    //translation/rotation are inverted to get the same result between Eigen and CLHEP
    const unsigned int nTests(3);
    const double distances[nTests]={0.2679 * CLHEP::mm, -0.4254 * CLHEP::mm, 0.0264 * CLHEP::mm};
    const double angles[nTests]={atan(-0.0001639), atan(0.0003195), atan(0.0000053)};
    const Amg::Translation3D translation(0., 11.93*CLHEP::mm, 0.);
    const Amg::Vector3D zAxis(0., 0., 1.);
    //this really needs checking!
    Amg::Transform3D zRotations[nTests] = {zRotation(angles[0]), zRotation(angles[1]), zRotation(angles[2])};
    Amg::Transform3D yTranslations[nTests]={yTranslation(distances[0]), yTranslation(distances[1]), yTranslation(distances[2])};
    addShiftTop(Amg::Transform3D(translation));
    addShiftModule(1,0,0, zRotations[0]*yTranslations[0]);
    addShiftModule(1,0,1, zRotations[1]*yTranslations[1]);
    addShiftModule(1,0,2, zRotations[2]*yTranslations[2]);
    //
    addShiftModule(1,1,0, Amg::Transform3D::Identity());
    addShiftModule(1,1,1, Amg::Transform3D::Identity());
    addShiftModule(1,1,2, Amg::Transform3D::Identity());
    /**
    // original (pre-Eigen) code for reference:
    addShiftTop(HepGeom::Translate3D(0,11.93*CLHEP::mm,0));  
    addShiftModule(1,0,0, HepGeom::TranslateY3D(0.2679 * CLHEP::mm) * HepGeom::RotateZ3D(atan(-0.0001639)));
    addShiftModule(1,0,1, HepGeom::TranslateY3D(-0.4254 * CLHEP::mm) * HepGeom::RotateZ3D(atan(0.0003195)));
    addShiftModule(1,0,2, HepGeom::TranslateY3D(0.0264 * CLHEP::mm) * HepGeom::RotateZ3D(atan(0.0000053)));
    addShiftModule(1,1,0, HepGeom::Transform3D());
    addShiftModule(1,1,1, HepGeom::Transform3D());
    addShiftModule(1,1,2, HepGeom::Transform3D());
    **/
  }
  printAlignmentShifts();
  return StatusCode::SUCCESS;
}


void
TestTRT_Alignment::addShiftTop(const Amg::Transform3D & transform) {
  Identifier id = m_idHelper->barrel_ec_id(1);
  addShift(2, id, transform);
} 


void
TestTRT_Alignment::addShiftModule(int bec, int phiMod, int layer, const Amg::Transform3D & transform) 
{
  Identifier id = m_idHelper->module_id(bec, phiMod, layer);
  addShift(1, id, transform);
} 


void
TestTRT_Alignment::addShift(int level, const Identifier & id, const Amg::Transform3D & transform) 
{
  m_manager->setAlignableTransformDelta(level, id, transform, InDetDD::other, nullptr);
} 


StatusCode TestTRT_Alignment::execute() {
  // Part 1: Get the messaging service, print where you are
  msg(MSG::INFO) << "execute()" << endmsg;
  printAlignmentShifts();
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode TestTRT_Alignment::finalize() {
  // Part 1: Get the messaging service, print where you are
  msg(MSG::INFO) << "finalize()" << endmsg;
  return StatusCode::SUCCESS;
}  

void
TestTRT_Alignment::printAlignmentShifts() {
  int oldPrec = cout.precision(m_precision);
  if (m_testAllElements) {
    unsigned int maxHash = m_idHelper->straw_layer_hash_max();
    for (unsigned int index = 0; index < maxHash; index++) {
      IdentifierHash idHash = index;
      Identifier id = m_idHelper->layer_id(idHash);
      m_idHelper->show(id);
      const TRT_BaseElement * element;
      element = m_manager->getElement(idHash);
      if (element) {
        // Test first and last straw
        if (m_testAllStraws) {
          for (unsigned int iStraw = 0; iStraw < element->nStraws(); iStraw++) {    
            compareStraw(element, iStraw);
          } 
        } else {
          compareStraw(element, 0);
          compareStraw(element, element->nStraws() - 1);
        }
      } else {
        cout << "No Element with id hash = " << idHash << endl;
      }
    } // loop over elements
  } else { // Just modules
    TRT_ID::const_id_iterator moduleIter;
    for (moduleIter = m_idHelper->module_begin(); moduleIter != m_idHelper->module_end(); moduleIter++) {
      Identifier moduleId = *moduleIter;
      cout << "Module " << m_idHelper->show_to_string(moduleId) << endl;
      int numStrawLayers = m_idHelper->straw_layer_max(moduleId);
      if (numStrawLayers) {
        Identifier firstElementId = m_idHelper->layer_id(moduleId,0);
        Identifier lastElementId = m_idHelper->layer_id(moduleId,numStrawLayers-1);
        compareEndStraws(firstElementId);
        compareEndStraws(lastElementId);
      } else {
        cout << "ERROR: No Elements in this module!" << endl;
      }
    }
  }
  cout.precision(oldPrec);
}

void
TestTRT_Alignment::compareEndStraws(const Identifier &id){
  const TRT_BaseElement * element = m_manager->getElement(id);
  cout << " " << m_idHelper->show_to_string(id) << endl;
  if (element) {
    compareStraw(element, 0);
    compareStraw(element, element->nStraws() - 1);
  } else {
    cout << " No Element with id = " << m_idHelper->show_to_string(id) << endl;
  }
} 


void
TestTRT_Alignment::compareStraw(const TRT_BaseElement * element, int straw){
  static const Amg::Vector3D zeroPoint(0., 0., 0.); 
  static const Amg::Vector3D zAxis(0., 0., 1.); 
  Amg::Transform3D strawDefXF = element->strawTransform(straw);
  Amg::Transform3D strawXF = element->strawTransform(straw);
  
  Amg::Transform3D globalDelta = strawXF * strawDefXF.inverse();
  // test if it is the identity transform within some error
  if (testIdentity(globalDelta, m_errRot,  m_errTrans)) {
    cout << "  Straw " << straw << ": NO SHIFT" << endl;
  } else {
    cout << "  Straw " << straw << ": Global shift: " << globalDelta * zeroPoint << endl ;
    if (m_longPrintOut) {
      cout << "   center    " << strawDefXF * zeroPoint << " -> " <<  strawXF * zeroPoint << endl; 
      cout << "   diff =    " << strawXF * zeroPoint - strawDefXF * zeroPoint <<  endl; 
      cout << "   strawAxis "  << strawDefXF * zAxis << " -> " <<  strawXF * zAxis  << endl; 
    } 
  }
}


bool
TestTRT_Alignment::testIdentity(const Amg::Transform3D & transform, double errRot,double errTrans) {
  using std::abs;
  const Amg::Transform3D & t1 = transform;
  const Amg::Transform3D t2 = Amg::Transform3D::Identity(); 
  // Rotation/Scale
  for (int i=0; i < 3; i++){
    for (int j=0; j < 3; j++){
      double diff = abs(t1(i,j) - t2(i,j));
      if (diff > errRot) return false;
    }
  }
  // Translation
  for (int ii = 0; ii < 3; ii++){
    double diff = abs(t1(ii,3) -  t2(ii,3));
    if (diff > errTrans) return false;
  }
  return true;
}
