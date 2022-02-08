/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetDetDescrExample/PrintSiElements.h"

#include "StoreGate/ReadCondHandle.h"

#include "CLHEP/Units/SystemOfUnits.h"

#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetIdentifier/PixelID.h"
#include "InDetIdentifier/SCT_ID.h"
#include "Identifier/Identifier.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "GeoPrimitives/CLHEPtoEigenConverter.h"

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
namespace{
  const Amg::Vector3D origin(0.,0., 0.);
}

/////////////////////////////////////////////////////////////////////////////
//
// Prints out SiDetectorElement transform

/////////////////////////////////////////////////////////////////////////////

PrintSiElements::PrintSiElements(const std::string& name, ISvcLocator* pSvcLocator) 
  : AthAlgorithm(name, pSvcLocator), 
    m_firstEvent(true),
    m_geoModelSvc("GeoModelSvc",name),
    m_detManagerNames{"Pixel","SCT"}
{  
  // Get parameter values from jobOptions file
  declareProperty("ModulesOnly", m_modulesOnly = true, "Print transforms of modules");
  declareProperty("ExpandId", m_expandId = true, "Print fields of identifier");
  declareProperty("NominalPosition", m_nominal = true, "Print out nominal geometry");
  declareProperty("AlignedPosition", m_aligned = false, "Print out aligned geometry");
  declareProperty("FullRotationMatrix", m_fullRotationMatrix = false, "If true prints the 9 elements of the rotation matrix");
  declareProperty("OutputFile", m_outputFileName = "geometry.dat", "Output file name");
  declareProperty("GeoModelSvc", m_geoModelSvc);
  declareProperty("DetectorManagerNames", m_detManagerNames);
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode PrintSiElements::initialize(){
  // Get GeoModelSvc
  const IGeoModelSvc * geoModel;
  ATH_CHECK(service("GeoModelSvc",geoModel));
  m_fileout.open(m_outputFileName.c_str());
  ATH_MSG_DEBUG( "Opening output file " << m_outputFileName );
  if (!m_fileout) {
    ATH_MSG_ERROR( "Could not open file " << m_outputFileName );
    return StatusCode::FAILURE;
  }
  // Print version infomration
  m_fileout << "# ATLAS tag: " << geoModel->atlasVersion() << std::endl;
  m_fileout << "# InDet tag: " << geoModel->inDetVersionOverride() << std::endl;
  m_fileout << "# Pixel tag: " << geoModel->pixelVersionOverride() << std::endl;
  m_fileout << "# SCT   tag: " << geoModel->SCT_VersionOverride() << std::endl;
  m_fileout << "# TRT   tag: " << geoModel->TRT_VersionOverride() << std::endl;
  // ReadCondHandleKey
  ATH_CHECK(m_pixelDetEleCollKey.initialize(std::find(m_detManagerNames.begin(), m_detManagerNames.end(), "Pixel") != m_detManagerNames.end()));
  ATH_CHECK(m_SCTDetEleCollKey.initialize(std::find(m_detManagerNames.begin(), m_detManagerNames.end(), "SCT") != m_detManagerNames.end()));
  ATH_CHECK(m_ITkPixelDetEleCollKey.initialize(std::find(m_detManagerNames.begin(), m_detManagerNames.end(), "ITkPixel") != m_detManagerNames.end()));
  ATH_CHECK(m_ITkStripDetEleCollKey.initialize(std::find(m_detManagerNames.begin(), m_detManagerNames.end(), "ITkStrip") != m_detManagerNames.end()));
  return StatusCode::SUCCESS;
}


StatusCode 
PrintSiElements::printElements(const std::string & managerName){
  const InDetDD::SiDetectorElementCollection * elements = nullptr;
  if (managerName=="Pixel") {
    SG::ReadCondHandle<InDetDD::SiDetectorElementCollection> pixelDetEleHandle(m_pixelDetEleCollKey);
    elements = *pixelDetEleHandle;
    if (not pixelDetEleHandle.isValid() or elements==nullptr) {
      ATH_MSG_WARNING(m_pixelDetEleCollKey.fullKey() << " is not available.");
      return StatusCode::RECOVERABLE;
    }
  } else if (managerName=="SCT") {
    SG::ReadCondHandle<InDetDD::SiDetectorElementCollection> sctDetEleHandle(m_SCTDetEleCollKey);
    elements = *sctDetEleHandle;
    if (not sctDetEleHandle.isValid() or elements==nullptr) {
      ATH_MSG_WARNING(m_SCTDetEleCollKey.fullKey() << " is not available.");
      return StatusCode::RECOVERABLE;
    }
  }
  else if (managerName=="ITkPixel") {
    SG::ReadCondHandle<InDetDD::SiDetectorElementCollection> pixelDetEleHandle(m_ITkPixelDetEleCollKey);
    elements = *pixelDetEleHandle;
    if (not pixelDetEleHandle.isValid() or elements==nullptr) {
      ATH_MSG_WARNING(m_ITkPixelDetEleCollKey.fullKey() << " is not available.");
      return StatusCode::RECOVERABLE;
    }
  }
  else if (managerName=="ITkStrip") {
    SG::ReadCondHandle<InDetDD::SiDetectorElementCollection> stripDetEleHandle(m_ITkStripDetEleCollKey);
    elements = *stripDetEleHandle;
    if (not stripDetEleHandle.isValid() or elements==nullptr) {
      ATH_MSG_WARNING(m_ITkStripDetEleCollKey.fullKey() << " is not available.");
      return StatusCode::RECOVERABLE;
    }
  }
  if (elements==nullptr) {
    ATH_MSG_FATAL("SiDetectorElementCollection elements is nullptr");
    return StatusCode::FAILURE;
  }

  for (const InDetDD::SiDetectorElement * element: *elements) {
    if (element) {
      Identifier id = element->identify();
      int det = 0;
      int bec = 0;
      int layer_disk = 0;
      int phi_module = 0;
      int eta_module = 0;
      int side = 0;
      
      // Get identifier fields. 
      if (element->isPixel()) {
        const PixelID * pixIdHelper = dynamic_cast<const PixelID *>(element->getIdHelper());
        if (pixIdHelper) {
					det = 1;
					bec = pixIdHelper->barrel_ec(id);
					layer_disk = pixIdHelper->layer_disk(id);
					phi_module = pixIdHelper->phi_module(id);
					eta_module = pixIdHelper->eta_module(id);
					side = 0;
        }
      } else { // SCT
        const SCT_ID * sctIdHelper = dynamic_cast<const SCT_ID *>(element->getIdHelper());
        if (sctIdHelper){
					det = 2;
					bec = sctIdHelper->barrel_ec(id);
					layer_disk = sctIdHelper->layer_disk(id);
					phi_module = sctIdHelper->phi_module(id);
					eta_module = sctIdHelper->eta_module(id);
					side = sctIdHelper->side(id);
        }
      }
      
      // skip side=1 elements if only print module transforms
      if (m_modulesOnly && side) continue;
      //
      Amg::Transform3D trans;
      Amg::Transform3D defTrans;
      if (m_modulesOnly) {
	      trans = element->moduleTransform();
        defTrans = element->defModuleTransform();
        //see: https://svnweb.cern.ch/trac/atlasoff/browser/InnerDetector/InDetDetDescr/InDetReadoutGeometry/trunk/InDetReadoutGeometry/SiDetectorElement.h
        //I dont see appropriate Eigen-returning member
      } else {
        trans = element->transform();
        defTrans = element->defTransform();
      }
      
      std::ostringstream idstr;
      if (m_expandId) {
        idstr << 2 << " " << det << " " 
          << bec << " " << layer_disk << " " 
          << phi_module << " " << eta_module << " " 
          << side << " ";
      } else {
        idstr << element->getIdHelper()->show_to_string(id) << " ";
      }
      if (m_nominal) {
        m_fileout << idstr.str() << printTransform(defTrans) << std::endl;
      }
      if (m_aligned) {
        m_fileout << idstr.str() << printTransform(trans) << std::endl;
      }
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode PrintSiElements::execute() {
  if (m_firstEvent) {
    m_firstEvent = false;
    for(const std::string& detManagerName : m_detManagerNames){
      printElements(detManagerName).ignore();
    }
  }
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode PrintSiElements::finalize() {
  m_fileout.close();
  return StatusCode::SUCCESS;
}


std::string 
PrintSiElements::printTransform(const Amg::Transform3D & trans) const{
  Amg::Vector3D xyz = trans * origin;
  std::ostringstream ostr;
  ostr << xyz.x() << " " << xyz.y() << " " << xyz.z() << " ";
  if (m_fullRotationMatrix) {
    for (int i=0; i < 3; i++) {
      for (int j=0; j < 3; j++) {
        ostr << trans(i,j) << " ";
      }
    }
  } else {
    double alpha=0, beta=0, gamma=0;
    extractAlphaBetaGamma(trans, alpha, beta, gamma);
    ostr << alpha << " " << beta << " " << gamma;
  }
  //CLHEP::HepRotation rot = trans.getRotation();
  //ostr << std::endl << rot;
  return ostr.str();
}

void 
PrintSiElements::extractAlphaBetaGamma(const Amg::Transform3D & trans, 
               double& alpha, double& beta, double &gamma) const 
{
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


