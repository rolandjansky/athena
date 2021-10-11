/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ForceLoadCondObj.cxx

#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/IClassIDSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "LArCalibTools/ForceLoadCondObj.h"


ForceLoadCondObj::ForceLoadCondObj(const std::string& name, 
					 ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  p_clidsvc ( "ClassIDSvc",    name )
{
  declareProperty("ObjectList",m_objectList,"list of 'object#key'");
}

ForceLoadCondObj::~ForceLoadCondObj() 
{}

StatusCode ForceLoadCondObj::initialize() {
  ATH_MSG_DEBUG ("in initialize()" );
  ATH_CHECK( p_clidsvc.retrieve() );
  return StatusCode::SUCCESS;
}

StatusCode ForceLoadCondObj::finalize() {
 
  return StatusCode::SUCCESS;
}

StatusCode ForceLoadCondObj::execute() {
  //Loop through objects
  for (unsigned int iobj=0;iobj<m_objectList.size();++iobj) {
    // if object name contains a '#', it represents a specific typename#key
    std::string::size_type ihash=m_objectList[iobj].find_first_of("#");
    if (ihash==std::string::npos) {
      ATH_MSG_ERROR ( "Expected syntax 'object#key' for property ObjectList, got" << m_objectList[iobj] );
      return StatusCode::FAILURE;
    }
    std::string::size_type ihash2=m_objectList[iobj].find_first_of("#",ihash+1);
    if (ihash2 != std::string::npos) ihash2-=(1+ihash);
    const std::string objName=m_objectList[iobj].substr(0,ihash);
    const std::string objKey=m_objectList[iobj].substr(ihash+1,ihash2);
    
    ATH_MSG_INFO ( "Retrieving object " << objName << ", key " << objKey );

    CLID clid;
    ATH_CHECK( p_clidsvc->getIDOfTypeName(objName,clid) );
    
    SG::DataProxy* proxy=detStore()->proxy(clid,objKey);
    
    if (!proxy) {
      ATH_MSG_ERROR ( "Could not find proxy for object of type " << objName << " with key " << objKey );
      return StatusCode::FAILURE;
    }
    
    if (proxy->accessData()!=0) { //This should trigger the conversion
      ATH_MSG_INFO ( "Sucessfully retrieved object of type " << objName << " with key " << objKey );
    }
  }
  return StatusCode::SUCCESS;
}
