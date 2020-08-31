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
  int hitIdOfWater = SiHitIdHelper::GetHelper->buildHitId(PixelHitIndex, index["barrel_endcap"], index["layer_wheel"],
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
