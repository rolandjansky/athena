/*
   Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
 */


#include "InDetGeoModelUtils/DistortedMaterialManager.h"
#include "GeoModelInterfaces/StoredMaterialManager.h"
#include "GeoModelUtilities/DecodeVersionKey.h"
#include "StoreGate/StoreGateSvc.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"

namespace InDetDD {
  DistortedMaterialManager::DistortedMaterialManager() {
    ISvcLocator* svcLocator = Gaudi::svcLocator(); // from Bootstrap

    MsgStream log(Athena::getMessageSvc(), "ExtraMaterialManager");
    log << MSG::DEBUG << "Initialized InDet Distorted Material Manager" << endmsg;

    StoreGateSvc* detStore;
    StatusCode sc = svcLocator->service("DetectorStore", detStore);
    if (sc.isFailure()) log << MSG::FATAL << "Could not locate DetectorStore" << endmsg;

    IRDBAccessSvc* rdbSvc;
    sc = svcLocator->service("RDBAccessSvc", rdbSvc);
    if (sc.isFailure()) log << MSG::FATAL << "Could not locate RDBAccessSvc" << endmsg;

    // Get version tag and node for InDet.
    DecodeVersionKey versionKey("InnerDetector");
    const std::string& detectorKey = versionKey.tag();
    const std::string& detectorNode = versionKey.node();

    log << MSG::DEBUG << "Retrieving Record Sets from database ..." << endmsg;
    log << MSG::DEBUG << "Key = " << detectorKey << " Node = " << detectorNode << endmsg;

    m_xMatTable = rdbSvc->getRecordsetPtr("InDetExtraMaterial", detectorKey, detectorNode);

    StoredMaterialManager* theGeoMaterialManager = nullptr;
    sc = detStore->retrieve(theGeoMaterialManager, "MATERIALS");
    if (sc.isFailure()) log << MSG::FATAL << "Could not locate GeoModel Material manager" << endmsg;
    m_materialManager = theGeoMaterialManager;
  }
} // end namespace
