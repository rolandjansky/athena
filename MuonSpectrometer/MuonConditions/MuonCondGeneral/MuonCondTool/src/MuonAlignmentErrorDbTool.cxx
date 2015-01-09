/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/MsgStream.h"

#include "StoreGate/StoreGateSvc.h"
#include "SGTools/TransientAddress.h"
#include "CoralBase/Attribute.h"
#include "CoralBase/AttributeListSpecification.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"

#include "Identifier/IdentifierHash.h"
#include "Identifier/Identifier.h"
#include "MuonIdHelpers/MdtIdHelper.h"

#include "PathResolver/PathResolver.h"
#include <fstream>
#include <string>
#include <algorithm>
#include <stdio.h>
#include <map>

#include "MuonCondTool/MuonAlignmentErrorDbTool.h"
//#include "MuonCondTool/MDT_MapConversion.h"

#include "MuonCondSvc/MdtStringUtils.h"

//#include "RDBAccessSvc/IRDBAccessSvc.h"
//#include "RDBAccessSvc/IRDBRecord.h"
//#include "RDBAccessSvc/IRDBRecordset.h"
//#include "GeoModelSvc/IGeoModelSvc.h"
//**********************************************************
//* Author Monica Verducci monica.verducci@cern.ch
//*
//* Tool to retrieve the MDT DQ Info from COOL DB
//* retrieving of tables from DB
//*********************************************************


MuonAlignmentErrorDbTool::MuonAlignmentErrorDbTool (const std::string& type,
				    const std::string& name,
				    const IInterface* parent)
	  : AthAlgTool(type, name, parent),
	    log( msgSvc(), name ),
	    m_debug(false),
	    m_verbose(false)   
{
  
  declareInterface< IMuonAlignmentErrorDbTool >(this);
  
  declareProperty("ErrorFolder",     m_errorFolder="/MUONALIGN/ERRORS");

  }



StatusCode MuonAlignmentErrorDbTool::updateAddress(StoreID::type /*storeID*/, SG::TransientAddress* /*tad*/)
{
//   MsgStream log(msgSvc(), name());
//   CLID clid        = tad->clID();
//   std::string key  = tad->name();
 
  return StatusCode::FAILURE;
}
	


StatusCode MuonAlignmentErrorDbTool::initialize()
{

  log.setLevel(outputLevel());
  m_debug = log.level() <= MSG::DEBUG;
  m_verbose = log.level() <= MSG::VERBOSE;

  log << MSG::INFO << "Initializing - folders names are: Error "<<m_errorFolder << endreq;
   
  StatusCode sc = serviceLocator()->service("DetectorStore", m_detStore);
  if ( sc.isSuccess() ) {
    if( m_debug ) log << MSG::DEBUG << "Retrieved DetectorStore" << endreq;
  }else{
    log << MSG::ERROR << "Failed to retrieve DetectorStore" << endreq;
    return sc;
  }
  


  sc = m_detStore->retrieve(m_mdtIdHelper, "MDTIDHELPER" );
  if (sc.isFailure())
    {
      log << MSG::FATAL << " Cannot retrieve MdtIdHelper " << endreq;
      return sc;
    }
  
    
  // Get interface to IOVSvc
  m_IOVSvc = 0;
  bool CREATEIF(true);
  sc = service( "IOVSvc", m_IOVSvc, CREATEIF );
  if ( sc.isFailure() )
    {
      log << MSG::ERROR << "Unable to get the IOVSvc" << endreq;
      return StatusCode::FAILURE;
    }
  
  if(sc.isFailure()) return StatusCode::FAILURE;
  
  
  
  // initialize the chrono service
  sc = service("ChronoStatSvc",m_chronoSvc);
  if (sc != StatusCode::SUCCESS) {
    log << MSG::ERROR << "Could not find the ChronoSvc" << endreq;
    return sc;
  }
	
  if(sc.isFailure()) return StatusCode::FAILURE;
  
  return StatusCode::SUCCESS;
}


StatusCode MuonAlignmentErrorDbTool::loadParameters(IOVSVC_CALLBACK_ARGS_P(I,keys))
{

  log.setLevel(outputLevel());
  m_debug = log.level() <= MSG::DEBUG;
  m_verbose = log.level() <= MSG::VERBOSE;

  StatusCode sc = loadAlignmentError(I,keys);

  if (sc.isFailure())
    {
      return sc;
    }
    
  return StatusCode::SUCCESS;
}


StatusCode MuonAlignmentErrorDbTool::loadAlignmentError(IOVSVC_CALLBACK_ARGS_P(I,keys))
{

  log.setLevel(outputLevel());
  m_debug = log.level() <= MSG::DEBUG;
  m_verbose = log.level() <= MSG::VERBOSE; 
  StatusCode sc=StatusCode::SUCCESS;
  log << MSG::INFO << "Load ERRORS from DB" << endreq;
  
  // Print out callback information
  if( m_debug ) log << MSG::DEBUG << "Level " << I << " Keys: ";
  std::list<std::string>::const_iterator keyIt = keys.begin();
  for (; keyIt != keys.end(); ++ keyIt) if( m_debug ) log << MSG::DEBUG << *keyIt << " ";
  if( m_debug ) log << MSG::DEBUG << endreq;
 
  
	
  const CondAttrListCollection * atrc;
  log << MSG::INFO << "Try to read from folder <"<<m_errorFolder<<">"<<endreq;
  
  sc=m_detStore->retrieve(atrc,m_errorFolder);
  if(sc.isFailure())  {
    log << MSG::ERROR
	<< "could not retrieve the CondAttrListCollection from DB folder " 
	<< m_errorFolder << endreq;
    return sc;
	  }
  
  else
    if( m_debug ) log<<MSG::DEBUG<<" CondAttrListCollection from DB folder have been obtained with size "<< atrc->size() <<endreq;
  
 
  CondAttrListCollection::const_iterator itr;
 
  for (itr = atrc->begin(); itr != atrc->end(); ++itr) {
  
    const coral::AttributeList& atr=itr->second;
    
    std::string list_error, version;
    
        
    list_error=*(static_cast<const std::string*>((atr["syserrors"]).addressOfData()));
    version=*(static_cast<const std::string*>((atr["version"]).addressOfData()));
    m_errorlistValue = list_error;
  }
     
  
  return StatusCode::SUCCESS;
}





