/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetDetDescrExample/TestSiAlignment.h"

#include "StoreGate/StoreGateSvc.h"

#include "CLHEP/Units/SystemOfUnits.h"

#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetReadoutGeometry/SiDetectorManager.h"
#include "InDetReadoutGeometry/SiCellId.h"
#include "InDetReadoutGeometry/SiIntersect.h"
#include "InDetReadoutGeometry/SiLocalPosition.h"
#include "InDetIdentifier/PixelID.h"
#include "InDetIdentifier/SCT_ID.h"
#include "Identifier/Identifier.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "GeoPrimitives/CLHEPtoEigenConverter.h"


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

TestSiAlignment::TestSiAlignment(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  m_manager(nullptr),
  m_managerName(""),
  m_geoModelSvc("GeoModelSvc", name)
{  
  // Get parameter values from jobOptions file
  declareProperty("ManagerName",  m_managerName);
  declareProperty("LongPrintOut", m_longPrintOut = false);
  declareProperty("ErrorRotation",m_errRot = 1e-15);      // For testing if alignment changed
  declareProperty("ErrorTranslation",m_errTrans = 1e-12); // For testing if alignment changed
  declareProperty("GeoModelSvc",  m_geoModelSvc);
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode TestSiAlignment::initialize(){

  ATH_MSG_INFO( "Algorithm Properties" );
  ATH_MSG_INFO( " ManagerName:      " << m_managerName );  
  ATH_MSG_INFO( " LongPrintOut:     " << (m_longPrintOut ? "true" : "false") );  
  ATH_MSG_INFO( " ErrorRotation:    " << m_errRot );  
  ATH_MSG_INFO( " ErrorTranslation: " << m_errTrans );  
  // GeoModelSvc
  ATH_CHECK (m_geoModelSvc.retrieve());
  StatusCode sc;
  if(m_geoModelSvc->geoInitialized()) {
    ATH_MSG_INFO( "Geometry already initialized. Call geoInitialize." );
    sc = geoInitialize();
  } else {
    ATH_MSG_INFO( "Geometry not yet initialized. Registering callback"  );
    // Register callback to check when TagInfo has changed
    sc =  detStore()->regFcn(&IGeoModelSvc::geoInit, &*m_geoModelSvc,&TestSiAlignment::geoInitCallback, this);
    if (sc.isFailure()) {
      ATH_MSG_ERROR( "Cannot register geoInitCallback function "  );
    } else {
      ATH_MSG_DEBUG( "Registered geoInitCallback callback  " );
    }
  }
  return sc;
}

StatusCode TestSiAlignment::geoInitCallback(IOVSVC_CALLBACK_ARGS){
  return geoInitialize();
}


StatusCode TestSiAlignment::geoInitialize() {
  // Retrieve Detector Manager
  ATH_CHECK(detStore()->retrieve(m_manager, m_managerName));
  printAlignmentShifts();
  return StatusCode::SUCCESS;
}


void
TestSiAlignment::printAlignmentShifts() {
  static const Amg::Vector3D zeroPoint(0., 0., 0.); 
  static const Amg::Vector3D phiAxis(1, 0, 0);
  static const Amg::Vector3D etaAxis(0, 1, 0);
  static const Amg::Vector3D depAxis(0, 0, 1);
  SiDetectorElementCollection::const_iterator iter(m_manager->getDetectorElementBegin());
  const SiDetectorElementCollection::const_iterator elementEnd(m_manager->getDetectorElementEnd());
  for (;iter != elementEnd; ++iter){
    const SiDetectorElement * element = *iter; 
    if (element) {
      // The id helper is available either through the manager or the elements
      cout << element->getIdHelper()->show_to_string(element->identify());
      // Get the default transform
      Amg::Transform3D elementDefXF = element->defTransform();
      Amg::Transform3D elementXF = element->transform();
      Amg::Transform3D localDelta  = elementDefXF.inverse() * elementXF;
      Amg::Transform3D globalDelta = elementXF * elementDefXF.inverse();
      Amg::Vector3D globalShift = elementXF * zeroPoint - elementDefXF * zeroPoint;
      double alphaL,betaL,gammaL;
      extractAlphaBetaGamma(localDelta,alphaL,betaL,gammaL);
      // test if it is the identity transform within some error
      if (testIdentity(localDelta, m_errRot,  m_errTrans)) {
        cout << " NO SHIFT" << endl;
      } else {
        if (m_longPrintOut) {
          double alphaG,betaG,gammaG;
          extractAlphaBetaGamma(globalDelta,alphaG,betaG,gammaG);
          cout << endl;
          cout << " Local Shift" << endl;
          cout << "  delta trans " << localDelta * zeroPoint << " Rot: {" << alphaL << "," << betaL << "," << gammaL << "}" <<endl; 
          cout << "  phiAxis " << phiAxis   << " -> " <<  localDelta * phiAxis << endl; 
          cout << "  etaAxis " << etaAxis   << " -> " <<  localDelta * etaAxis << endl; 
          cout << "  depAxis " << depAxis   << " -> " <<  localDelta * depAxis << endl; 
          cout << " Global Shift" << endl;
          cout << "  shift        " << globalShift << endl; 
          cout << "  delta trans  " << globalDelta * zeroPoint << " Rot: {" << alphaG << "," << betaG << "," << gammaG << "}" <<endl; 
          cout << "  center  " << elementDefXF * zeroPoint << " -> " <<  elementXF * zeroPoint << endl; 
          cout << "  phiAxis " << elementDefXF * phiAxis   << " -> " <<  elementXF * phiAxis << endl; 
          cout << "  etaAxis " << elementDefXF * etaAxis   << " -> " <<  elementXF * etaAxis << endl;
          cout << "  depAxis " << elementDefXF * depAxis   << " -> " <<  elementXF * depAxis << endl;     
        } else {
          cout << " Global shift: " << globalShift << "  Local shift/Rot: " << localDelta * zeroPoint << " Rot: {" 
                                  << alphaL << "," << betaL << "," << gammaL << "}" <<endl;  
        } 
      }
    } else {
      cout << "Missing element!!!!!!!!!!!" << endl;
    }
  }
  cout << endl;
}

bool
TestSiAlignment::testIdentity(const Amg::Transform3D & transform, double errRot,double errTrans) {
  using std::abs;
  const Amg::Transform3D & t1 = transform;
  const Amg::Transform3D t2; // identity
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

void  TestSiAlignment::extractAlphaBetaGamma(const Amg::Transform3D & trans, double& alpha, double& beta, double &gamma) const {
  double siny = trans(0,2);
  beta = asin(siny);
  // Check if cosy = 0. This requires special treatment.
  // can check either element (1,2),(2,2) both equal zero
  // or (0,1) and (0,0)
  if ((trans(1,2) == 0) && (trans(2,2) == 0)) {
    // alpha and gamma are degenerate. We arbitrarily choose
    // gamma = 0.
    gamma = 0;
    alpha = atan2(trans(1,1),trans(2,1));
  } else {
    alpha = atan2(-trans(1,2),trans(2,2));
    gamma = atan2(-trans(0,1),trans(0,0));
    if (alpha == 0) alpha = 0; // convert -0 to 0
    if (gamma == 0) gamma = 0; // convert -0 to 0
  }
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode TestSiAlignment::execute() {
  printAlignmentShifts();
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode TestSiAlignment::finalize() {
  return StatusCode::SUCCESS;
}




