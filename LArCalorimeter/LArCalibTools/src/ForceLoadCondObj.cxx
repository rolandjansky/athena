/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ForceLoadCondObj.cxx

#include "StoreGate/StoreGateSvc.h"
#include "AthenaKernel/IClassIDSvc.h"
//#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "LArCalibTools/ForceLoadCondObj.h"


ForceLoadCondObj::ForceLoadCondObj(const std::string& name, 
					 ISvcLocator* pSvcLocator) :
  Algorithm(name, pSvcLocator),
  p_detstore( "DetectorStore", name ),
  p_clidsvc ( "ClassIDSvc",    name )
{
  declareProperty("ObjectList",m_objectList,"list of 'object#key'");
}

ForceLoadCondObj::~ForceLoadCondObj() 
{}

StatusCode ForceLoadCondObj::initialize() {
  MsgStream log(msgSvc(), name());
  log <<MSG::DEBUG <<"in initialize()" <<endreq;

  // get pointer to detector store
  if (StatusCode::SUCCESS!= p_detstore.retrieve()) {
    log << MSG::FATAL << "Detector store not found" << endreq;
    return StatusCode::FAILURE;
  }
  // get pointer to ClassIDSvc
  if (StatusCode::SUCCESS!= p_clidsvc.retrieve()) {
    log << MSG::FATAL << "ClassIDSvc not found" << endreq;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode ForceLoadCondObj::finalize() {
 
  return StatusCode::SUCCESS;
}

StatusCode ForceLoadCondObj::execute() {
  MsgStream log(messageService(), name());
  //Loop through objects
  for (unsigned int iobj=0;iobj<m_objectList.size();++iobj) {
    // if object name contains a '#', it represents a specific typename#key
    std::string::size_type ihash=m_objectList[iobj].find_first_of("#");
    if (ihash==std::string::npos) {
      log << MSG::ERROR << "Expected syntax 'object#key' for property ObjectList, got" << m_objectList[iobj] << endreq;
      return StatusCode::FAILURE;
    }
    std::string::size_type ihash2=m_objectList[iobj].find_first_of("#",ihash+1);
    if (ihash2 != std::string::npos) ihash2-=(1+ihash);
    const std::string objName=m_objectList[iobj].substr(0,ihash);
    const std::string objKey=m_objectList[iobj].substr(ihash+1,ihash2);
    
    log << MSG::INFO << "Retrieving object " << objName << ", key " << objKey << endreq;

    CLID clid;
    if (p_clidsvc->getIDOfTypeName(objName,clid).isFailure()) {
      log << MSG::ERROR << "Failed to get CLID for type" << objName << endreq;
      return StatusCode::FAILURE;
    }
    
    SG::DataProxy* proxy=p_detstore->proxy(clid,objKey);
    
    if (!proxy) {
      log << MSG::ERROR << "Could not find proxy for object of type " << objName << " with key " << objKey << endreq;
      return StatusCode::FAILURE;
    }
    
    if (proxy->accessData()!=0) { //This should trigger the conversion
      log << MSG::INFO << "Sucessfully retrieved object of type " << objName << " with key " << objKey << endreq;
    }
  }
  return StatusCode::SUCCESS;
}
