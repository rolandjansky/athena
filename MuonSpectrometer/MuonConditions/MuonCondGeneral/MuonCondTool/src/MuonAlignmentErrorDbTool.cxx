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
	    m_detStore(0),
            m_IOVSvc(0),
            m_mdtIdHelper(0),
            m_chronoSvc(0),
	    m_log( msgSvc(), name ),
	    m_debug(false),
	    m_verbose(false)   
{
  
  declareInterface< IMuonAlignmentErrorDbTool >(this);
  
  declareProperty("ErrorFolder",     m_errorFolder="/MUONALIGN/ERRS");

  }



StatusCode MuonAlignmentErrorDbTool::updateAddress(StoreID::type /*storeID*/,
                                                   SG::TransientAddress* /*tad*/,
                                                   const EventContext& /*ctx*/)
{
//   MsgStream m_log(msgSvc(), name());
//   CLID clid        = tad->clID();
//   std::string key  = tad->name();
 
  return StatusCode::FAILURE;
}
	


StatusCode MuonAlignmentErrorDbTool::initialize()
{

  m_log.setLevel(msgLevel());
  m_debug = m_log.level() <= MSG::DEBUG;
  m_verbose = m_log.level() <= MSG::VERBOSE;

  m_log << MSG::INFO << "Initializing - folders names are: Error "<<m_errorFolder << endmsg;
   
  StatusCode sc = serviceLocator()->service("DetectorStore", m_detStore);
  if ( sc.isSuccess() ) {
    if( m_debug ) m_log << MSG::DEBUG << "Retrieved DetectorStore" << endmsg;
  }else{
    m_log << MSG::ERROR << "Failed to retrieve DetectorStore" << endmsg;
    return sc;
  }
  


  sc = m_detStore->retrieve(m_mdtIdHelper, "MDTIDHELPER" );
  if (sc.isFailure())
    {
      m_log << MSG::FATAL << " Cannot retrieve MdtIdHelper " << endmsg;
      return sc;
    }
  
    
  // Get interface to IOVSvc
  m_IOVSvc = 0;
  bool CREATEIF(true);
  sc = service( "IOVSvc", m_IOVSvc, CREATEIF );
  if ( sc.isFailure() )
    {
      m_log << MSG::ERROR << "Unable to get the IOVSvc" << endmsg;
      return StatusCode::FAILURE;
    }
  
  if(sc.isFailure()) return StatusCode::FAILURE;
  
  
  
  // initialize the chrono service
  sc = service("ChronoStatSvc",m_chronoSvc);
  if (sc != StatusCode::SUCCESS) {
    m_log << MSG::ERROR << "Could not find the ChronoSvc" << endmsg;
    return sc;
  }
	
  if(sc.isFailure()) return StatusCode::FAILURE;
  
  return StatusCode::SUCCESS;
}


StatusCode MuonAlignmentErrorDbTool::loadParameters(IOVSVC_CALLBACK_ARGS_P(I,keys))
{

  m_log.setLevel(msgLevel());
  m_debug = m_log.level() <= MSG::DEBUG;
  m_verbose = m_log.level() <= MSG::VERBOSE;

  StatusCode sc = loadAlignmentError(I,keys);

  if (sc.isFailure())
    {
      return sc;
    }
    
  return StatusCode::SUCCESS;
}


StatusCode MuonAlignmentErrorDbTool::loadAlignmentError(IOVSVC_CALLBACK_ARGS_P(I,keys))
{

  m_log.setLevel(msgLevel());
  m_debug = m_log.level() <= MSG::DEBUG;
  m_verbose = m_log.level() <= MSG::VERBOSE; 
  StatusCode sc=StatusCode::SUCCESS;
  m_log << MSG::INFO << "Load errors from DB" << endmsg;
  
  // Print out callback information
  if( m_debug ) m_log << MSG::DEBUG << "Level " << I << " Keys: ";
  std::list<std::string>::const_iterator keyIt = keys.begin();
  for (; keyIt != keys.end(); ++ keyIt) if( m_debug ) m_log << MSG::DEBUG << *keyIt << " ";
  if( m_debug ) m_log << MSG::DEBUG << endmsg;
 
  
	
  const CondAttrListCollection * atrc;
  m_log << MSG::INFO << "Try to read from folder <"<<m_errorFolder<<">"<<endmsg;
  
  sc=m_detStore->retrieve(atrc,m_errorFolder);
  if(sc.isFailure())  {
    m_log << MSG::ERROR
	<< "could not retrieve the CondAttrListCollection from DB folder " 
	<< m_errorFolder << endmsg;
    return sc;
	  }
  
  else
    if( m_debug ) m_log<<MSG::DEBUG<<" CondAttrListCollection from DB folder have been obtained with size "<< atrc->size() <<endmsg;
  
 
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





