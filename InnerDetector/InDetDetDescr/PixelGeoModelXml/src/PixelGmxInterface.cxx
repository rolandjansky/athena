/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
#include "PixelReadoutGeometry/PixelDiodeMap.h"
#include "PixelReadoutGeometry/PixelDiodeMatrix.h"
#include "PixelReadoutGeometry/PixelReadoutScheme.h"
#include "PixelReadoutGeometry/PixelMultipleConnection1D.h"
#include "InDetReadoutGeometry/SiDetectorDesign.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetReadoutGeometry/SiCommonItems.h"

const int PixelHitIndex = 0;

using namespace std;

PixelGmxInterface::PixelGmxInterface(InDetDD::PixelDetectorManager *detectorManager, InDetDD::SiCommonItems *commonItems, WaferTree *waferTree):
  m_detectorManager(detectorManager),
  m_commonItems(commonItems),
  m_waferTree(waferTree) {
  
  ServiceHandle<IMessageSvc> msgh("MessageSvc", "PixelGmxInterface");
  
  m_log = new MsgStream(&(*msgh), "PixelGmxInterface");
}

PixelGmxInterface::~PixelGmxInterface() {
  delete m_log;
}

int PixelGmxInterface::moduleId(map<string, int> &index){
  //
  //    Return the Simulation HitID (nothing to do with "ATLAS Identifiers" aka "Offline Identifiers")
  int hitIdOfWafer = SiHitIdHelper::GetHelper->buildHitId(PixelHitIndex, index["barrel_endcap"], index["layer_wheel"],
							  index["eta_module"], index["phi_module"], index["side"]);

  *m_log << MSG::DEBUG  << "Index list: " << index["barrel_endcap"] << " " << index["layer_wheel"] << " " <<
                                     index["eta_module"] << " " << index["phi_module"] << " " << index["side"] << endmsg;
  *m_log << MSG::DEBUG << "hitIdOfWafer = " << std::hex << hitIdOfWafer << std::dec << endmsg;
  *m_log << MSG::DEBUG << " bec = " << SiHitIdHelper::GetHelper()->getBarrelEndcap(hitIdOfWafer) <<
                                    " lay = " << SiHitIdHelper::GetHelper()->getLayerDisk(hitIdOfWafer) <<
                                    " eta = " << SiHitIdHelper::GetHelper()->getEtaModule(hitIdOfWafer) <<
                                    " phi = " << SiHitIdHelper::GetHelper()->getPhiModule(hitIdOfWafer) <<
                                    " side = " << SiHitIdHelper::GetHelper()->getSide(hitIdOfWafer) << endmsg;
  return hitIdOfWafer;
}


void PixelGmxInterface::addModuleType(string clas, string typeName, map<string, string> parameters){

  *m_log << MSG::DEBUG << "PixelGmxInterface::addModuleType called for class " << clas << " typeName " << typeName <<
                                    endmsg;
  if (clas == "SingleChip") {
    makeSingleChipModule(typeName, parameters);
  }
  else if (clas == "QuadChip") {
    makeQuadChipModule(typeName, parameters);
  }
  else {
    *m_log << MSG::ERROR << "PixelGmxInterface::addModuleType: unrecognised module class, " << clas << endmsg;
    *m_log << MSG::ERROR << "No module design created" << endmsg;
  }
}

void PixelGmxInterface::makeSingleChipModule(string typeName, map<string, string> &par){
  //
  // Get all parameters.
  //
  double chipLength(20.0);
  double chipWidth(19.2);
  double pitchEta(0.050);
  double pitchPhi(0.050);
  double nCols(400);
  double nRows(384);
  
}

void PixelGmxInterface::makeQuadChipModule(string typeName, map<string, string> &par){

}

string PixelGmxInterface::getstr(const string typeName, const string name, const map<string, string> &par) {
  map<string, string>::cons_iterator found;
  if ((found = par.find(name)) != par.end()) {
    return found->second;
  }
  else {
    *m_log << MSG::FATAL << "PixelGmxInterface::addModuleType: Error: missing parameter " << name << " for " <<
                                      typeName << endmsg;
    exit(999);
  }
  
}

void StripGmxInterface::addModule(string typeName, map<string, int> &index, int /*sensitiveId*/, GeoVFullPhysVol *fpv) {
  //
  //    Get the ATLAS "Offline" wafer identifier 
  //
  const Pixel_ID *pixelIdHelper = dynamic_cast<const Pixel_ID *> (m_commonItems->getIdHelper());
  Identifier id = pixelIdHelper->module_id(index["barrel_endcap"], index["layer_wheel"], index["phi_module"], 
					index["eta_module"], index["side"]);
  IdentifierHash hashId = pixelIdHelper->module_hash(id);
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
    exit(999);
  }
  InDetDD::SiDetectorElement *detector = new InDetDD::SiDetectorElement(id, design, fpv, m_commonItems);
  m_detectorManager->addDetectorElement(detector);
  //
  //    Build up a map-structure for numerology
  //
  Module module((unsigned int) hashId);
  string errorMessage("");
  if (!m_moduleTree->add(index["barrel_endcap"], index["layer_wheel"], index["eta_module"], 
			index["phi_module"], index["side"], module, errorMessage)) {
    *m_log << MSG::ERROR << errorMessage << endmsg;
  }
  return;
}

