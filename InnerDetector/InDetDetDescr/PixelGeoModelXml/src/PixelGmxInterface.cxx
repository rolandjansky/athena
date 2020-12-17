/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelGeoModelXml/PixelGmxInterface.h"

#include <cstdlib>
#include <sstream>

#include "InDetSimEvent/SiHitIdHelper.h" 

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IMessageSvc.h" 
#include "GaudiKernel/MsgStream.h"

#include "PixelReadoutGeometry/PixelDetectorManager.h"
#include "PixelReadoutGeometry/PixelModuleDesign.h"
#include "ReadoutGeometryBase/PixelDiodeMatrix.h"
#include "ReadoutGeometryBase/PixelMultipleConnection1D.h"
#include "InDetReadoutGeometry/SiDetectorDesign.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "ReadoutGeometryBase/SiCommonItems.h"

const int PixelHitIndex = 0;

using namespace std;

PixelGmxInterface::PixelGmxInterface(InDetDD::PixelDetectorManager *detectorManager, InDetDD::SiCommonItems *commonItems, ModuleTree *moduleTree):
  m_detectorManager(detectorManager),
  m_commonItems(commonItems),
  m_moduleTree(moduleTree) {
  // Logging: ref https://wiki.bnl.gov/dayabay/index.php?title=Logging
  // Turn on logging in job-options with: MessageSvc.setDebug += {"PixelGmxInterface"}
  ServiceHandle<IMessageSvc> msgh("MessageSvc", "PixelGmxInterface");
  
  m_log = std::make_unique<MsgStream>(&(*msgh), "PixelGmxInterface");
}

PixelGmxInterface::~PixelGmxInterface() {
}

int PixelGmxInterface::moduleId(map<string, int> &index){
  //
  //    Return the Simulation HitID (nothing to do with "ATLAS Identifiers" aka "Offline Identifiers")
  
  int hitIdOfModule = SiHitIdHelper::GetHelper()->buildHitId(PixelHitIndex, index["barrel_endcap"], index["layer_wheel"], 
							   index["eta_module"], index["phi_module"], index["side"]);

  *m_log << MSG::DEBUG  << "Index list: " << index["barrel_endcap"] << " " << index["layer_wheel"] << " " <<
                                     index["eta_module"] << " " << index["phi_module"] << " " << index["side"] << endmsg;
  *m_log << MSG::DEBUG << "hitIdOfModule = " << std::hex << hitIdOfModule << std::dec << endmsg;
  *m_log << MSG::DEBUG << " bec = " << SiHitIdHelper::GetHelper()->getBarrelEndcap(hitIdOfModule) <<
                                    " lay = " << SiHitIdHelper::GetHelper()->getLayerDisk(hitIdOfModule) <<
                                    " eta = " << SiHitIdHelper::GetHelper()->getEtaModule(hitIdOfModule) <<
                                    " phi = " << SiHitIdHelper::GetHelper()->getPhiModule(hitIdOfModule) <<
                                    " side = " << SiHitIdHelper::GetHelper()->getSide(hitIdOfModule) << endmsg;
  return hitIdOfModule;
}


void PixelGmxInterface::addModuleType(string clas, string typeName, map<string, string> parameters){

  *m_log << MSG::DEBUG << "PixelGmxInterface::addModuleType called for class " << clas << " typeName " << typeName <<
                                    endmsg;
  if (clas == "PixelModule") {
    makePixelModule(typeName, parameters);
  }
  else {
    *m_log << MSG::ERROR << "PixelGmxInterface::addModuleType: unrecognised module class, " << clas << endmsg;
    *m_log << MSG::ERROR << "No module design created" << endmsg;
  }
}

void PixelGmxInterface::makePixelModule(string typeName, map<string, string> &par){
  //
  // Get all parameters.
  // This comes from PixelModuleDesign
  //
  double thickness(2.075);
  int circuitsPerColumn(10);
  int circuitsPerRow(10);
  int cellColumnsPerCircuit(10);
  int cellRowsPerCircuit(10);
  int diodeColumnsPerCircuit(10);
  int diodeRowsPerCircuit(10);
  std::shared_ptr<InDetDD::PixelDiodeMatrix> matrix;
  InDetDD::CarrierType carrier(InDetDD::electrons);
  int readoutSide(-1);
  bool is3D(false);

  string car = getstr(typeName, "carrierType", par);
  if (car == "electrons") {
    carrier = InDetDD::electrons;
  }
  else if (car == "holes") {
    carrier = InDetDD::holes;
  }
  else {
    *m_log << MSG::FATAL <<
      "PixelGmxInterface::makePixelModule: Error: parameter carrierType should be electrons or holes for " <<
      typeName << endmsg;
    throw runtime_error("PixelGmxInterface::makePixelModule: Error: parameter carrierType should be electrons or holes for " 
			+ typeName);
  }
  string ros = getstr(typeName, "readoutSide", par);
  if (ros == "+") {
    readoutSide = 1;
  }
  else if (ros == "-") {
    readoutSide = -1;
  }
  else {
    *m_log << MSG::FATAL <<
      "PixelGmxInterface::makePixelModule: Error: parameter readoutSide should be + or - for " << typeName << endmsg;
    throw runtime_error("PixelGmxInterface::makePixelModule: Error: parameter readoutSide should be + or - for " + typeName);
  }
  getparm(typeName, "thickness", par, thickness);
  getparm(typeName, "circuitsPerColumn", par, circuitsPerColumn);
  getparm(typeName, "circuitsPerRow", par, circuitsPerRow);
  getparm(typeName, "cellColumnsPerCircuit", par, cellColumnsPerCircuit);
  getparm(typeName, "cellRowsPerCircuit", par, cellRowsPerCircuit);  
  getparm(typeName, "diodeColumnsPerCircuit", par, diodeColumnsPerCircuit);  
  getparm(typeName, "diodeRowsPerCircuit", par, diodeRowsPerCircuit);  
  //  getparm(typeName, "matrix", par, matrix);
  getparm(typeName, "is3D", par, is3D);

  //
  //   Make Module Design and add to DetectorManager
  //
  std::unique_ptr<InDetDD::PixelModuleDesign> design=std::make_unique<InDetDD::PixelModuleDesign>(thickness, 
												  circuitsPerColumn, circuitsPerRow, 
												  cellColumnsPerCircuit, cellRowsPerCircuit, 
												  diodeColumnsPerCircuit, diodeRowsPerCircuit,
												  matrix,
												  carrier, 
												  readoutSide, is3D); 
  m_detectorManager->addDesign(std::move(design));

  //    Add to map for addModule routine
  m_geometryMap[typeName] = m_detectorManager->numDesigns() -1;
}

string PixelGmxInterface::getstr(const string typeName, const string name, const map<string, string> &par) {
  map<string, string>::const_iterator found;
  if ((found = par.find(name)) != par.end()) {
    return found->second;
  }
  else {
    *m_log << MSG::FATAL << "PixelGmxInterface::addModuleType: Error: missing parameter " << name << " for " <<
                                      typeName << endmsg;
    throw runtime_error("PixelGmxInterface::addModuleType: Error: missing parameter " + name + " for " + typeName);
  }
  
}

void PixelGmxInterface::addModule(string typeName, map<string, int> &index, int /*sensitiveId*/, GeoVFullPhysVol *fpv) {
  //
  //    Get the ATLAS "Offline" wafer identifier 
  //
  const PixelID *pixelIdHelper = dynamic_cast<const PixelID *> (m_commonItems->getIdHelper());
  Identifier id = pixelIdHelper->wafer_id(index["barrel_endcap"], index["layer_wheel"], index["phi_module"], 
					  index["eta_module"]);
  IdentifierHash hashId = pixelIdHelper->wafer_hash(id);
  //
  //    Now do our best to check if this is a valid id. If either the gmx file is wrong, or the xml file
  //    defining the allowed id's is wrong, you can get disallowed id's. These cause a crash later 
  //    if allowed through. To do the check, we ask for the hash-id of this id. Invalid ids give a 
  //    special invalid hash-id (0xFFFFFFFF). But we don't exit the run, to help debug things quicker.
  //
  if (!hashId.is_valid()) {
    *m_log << MSG::ERROR <<"Invalid id for sensitive module " << typeName << " volume with indices \n";
    for (map<string, int>::iterator i = index.begin(); i != index.end(); ++i) {
      *m_log << MSG::ERROR << i->first << " = " << i->second << "; ";
    }
    *m_log << MSG::ERROR << 
      "\nRefusing to make it into a sensitive element. Incompatible gmx and identifier-xml files." << endmsg;
    return;
  }
  //
  //    Create the detector element and add to the DetectorManager
  //
  const InDetDD::SiDetectorDesign *design = m_detectorManager->getDesign(m_geometryMap[typeName]);

  if (!design) {
    *m_log << MSG::FATAL << "PixelGmxInterface::addSensor: Error: Readout sensor type " << typeName << 
      " not found.\n" << endmsg;
    throw runtime_error("PixelGmxInterface::addSensor: Error: Readout sensor type " + typeName + " not found.");
  }
  InDetDD::SiDetectorElement *detector = new InDetDD::SiDetectorElement(id, design, fpv, m_commonItems);
  m_detectorManager->addDetectorElement(detector);
  //
  //    Build up a map-structure for numerology
  //
  Module module((unsigned int) hashId);
  string errorMessage("");
  if (!m_moduleTree->add(index["barrel_endcap"], index["layer_wheel"], index["eta_module"], 
			 index["phi_module"], module, errorMessage)) {
    *m_log << MSG::ERROR << errorMessage << endmsg;
  }
  return;
}

