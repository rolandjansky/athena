/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "InDetGeoModelUtils/DistortedMaterialManager.h"
#include "GeoModelInterfaces/StoredMaterialManager.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "GeoModelUtilities/DecodeVersionKey.h"
#include "AthenaKernel/MsgStreamMember.h"
#include "StoreGate/StoreGate.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"

namespace InDetDD {

DistortedMaterialManager::DistortedMaterialManager()
{
  
  ISvcLocator* svcLocator = Gaudi::svcLocator(); // from Bootstrap

  Athena::MsgStreamMember log("ExtraMaterialManager");
  log << MSG::DEBUG << "Initialized InDet Distorted Material Manager" << endreq;

  StoreGateSvc * detStore;
  StatusCode sc;
  sc = svcLocator->service("DetectorStore", detStore );
  if (sc.isFailure()) log << MSG::FATAL << "Could not locate DetectorStore" << endreq;  
 
  IRDBAccessSvc *rdbSvc;
  sc = svcLocator->service("RDBAccessSvc",rdbSvc);
  if (sc.isFailure()) log << MSG::FATAL << "Could not locate RDBAccessSvc" << endreq;

  IGeoModelSvc *geoModel;
  sc = svcLocator->service ("GeoModelSvc",geoModel);
  if (sc.isFailure()) log << MSG::FATAL << "Could not locate GeoModelSvc" << endreq;

  // Get version tag and node for InDet.
  DecodeVersionKey versionKey(geoModel, "InnerDetector");
  std::string detectorKey  = versionKey.tag();
  std::string detectorNode = versionKey.node();

  log << MSG::DEBUG << "Retrieving Record Sets from database ..." << endreq;
  log << MSG::DEBUG << "Key = " << detectorKey << " Node = " << detectorNode << endreq;

  m_xMatTable = rdbSvc->getRecordsetPtr("InDetExtraMaterial", detectorKey, detectorNode);

  const StoredMaterialManager * theGeoMaterialManager = 0;
  sc = detStore->retrieve(theGeoMaterialManager, "MATERIALS");
  if (sc.isFailure()) log << MSG::FATAL << "Could not locate GeoModel Material manager" << endreq;
  m_materialManager = theGeoMaterialManager;
  
}  

} // end namespace
