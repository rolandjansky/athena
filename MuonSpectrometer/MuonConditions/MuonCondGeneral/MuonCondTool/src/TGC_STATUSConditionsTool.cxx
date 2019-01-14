/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
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
#include "MuonIdHelpers/TgcIdHelper.h"

#include "PathResolver/PathResolver.h"
#include <fstream>
#include <string>
#include <stdio.h>
#include <map>

#include "MuonCondTool/TGC_STATUSConditionsTool.h"
#include "MuonCondInterface/ITGC_STATUSConditionsTool.h"

#include "GeoModelInterfaces/IGeoModelSvc.h"



TGC_STATUSConditionsTool::TGC_STATUSConditionsTool (const std::string& type,
				      const std::string& name,
				      const IInterface* parent)
  : AthAlgTool(type, name, parent), 
    m_IOVSvc(0),
    m_chronoSvc(0),
    m_log( msgSvc(), name ),
    m_debug(false),
    m_verbose(false)  
{
  declareInterface< ITGC_STATUSConditionsTool >(this);
        
  m_tgcDqStatusDataLocation="TgcDqStatusKey";

  m_tgcIdHelper = 0;
  declareProperty("TgcDqFolder",m_FolderName="TGC/1/DetectorStatus");

}



//StatusCode TGC_STATUSConditionsTool::updateAddress(SG::TransientAddress* tad)
StatusCode TGC_STATUSConditionsTool::updateAddress(StoreID::type /*storeID*/,
                                                   SG::TransientAddress* tad,
                                                   const EventContext& /*ctx*/)
{
  
  m_log.setLevel(msgLevel());
  m_debug = m_log.level() <= MSG::DEBUG;
  m_verbose = m_log.level() <= MSG::VERBOSE;
  // CLID clid        = tad->clID();
  std::string key  = tad->name();
 
  return StatusCode::FAILURE;
}
	


StatusCode TGC_STATUSConditionsTool::initialize()
{
  m_log.setLevel(msgLevel());
  m_debug = m_log.level() <= MSG::DEBUG;
  m_verbose = m_log.level() <= MSG::VERBOSE;

  m_log << MSG::INFO << "Initializing - folders names are: Chamber Status "<<m_FolderName << endmsg;
   
  // Get interface to IOVSvc
  m_IOVSvc = 0;
  bool CREATEIF(true);
  StatusCode sc = service( "IOVSvc", m_IOVSvc, CREATEIF );
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

  sc = detStore()->retrieve(m_tgcIdHelper, "TGCIDHELPER" );
  if (sc.isFailure())
    {
      m_log<< MSG::FATAL << " Cannot retrieve TgcIdHelper " << endmsg;
      return sc;
    }
  
    
  return StatusCode::SUCCESS;
 

}

StatusCode TGC_STATUSConditionsTool::loadParameterStatus(IOVSVC_CALLBACK_ARGS_P(I,keys))
{
 
  m_log.setLevel(msgLevel());
  m_debug = m_log.level() <= MSG::DEBUG;
  m_verbose = m_log.level() <= MSG::VERBOSE;	 
  std::list<std::string>::const_iterator itr;
  for (itr=keys.begin(); itr!=keys.end(); ++itr) {
    m_log << MSG::INFO <<"LoadParameters "<< *itr << " I="<<I<<" "<<endmsg;
    if(*itr==m_FolderName) {
        StatusCode sc =loadTgcDqStatus(I,keys);
      if (sc.isFailure())
	{
	  return sc;
	}
    }
   
  }
  
  return StatusCode::SUCCESS;
}




      

StatusCode TGC_STATUSConditionsTool::loadTgcDqStatus(IOVSVC_CALLBACK_ARGS_P(I,keys)) 
{
  m_log.setLevel(msgLevel());
  m_debug = m_log.level() <= MSG::DEBUG;
  m_verbose = m_log.level() <= MSG::VERBOSE;
 
  StatusCode sc=StatusCode::SUCCESS;
  m_log << MSG::INFO << "Load Tgc Status flags  from DB" << endmsg;

  // Print out callback information
   if( m_debug )  m_log << MSG::DEBUG << "Level " << I << " Keys: ";
  std::list<std::string>::const_iterator keyIt = keys.begin();
  for (; keyIt != keys.end(); ++ keyIt)  if( m_debug ) m_log << MSG::DEBUG << *keyIt << " ";
   if( m_debug )  m_log << MSG::DEBUG << endmsg;
  
  const CondAttrListCollection * atrc = nullptr;
  m_log << MSG::INFO << "Try to read from folder <"<< m_FolderName <<">"<<endmsg;

  sc=detStore()->retrieve(atrc,m_FolderName);
  if(sc.isFailure())  {
    m_log << MSG::ERROR
	<< "could not retreive the CondAttrListCollection from DB folder " 
	<< m_FolderName << endmsg;
    return sc;
	  }
  
  else
    m_log<<MSG::INFO<<" CondAttrListCollection from DB folder have been obtained with size "<< atrc->size() <<endmsg;
  
 
  CondAttrListCollection::const_iterator itr;
  for (itr = atrc->begin(); itr != atrc->end(); ++itr) {
     const coral::AttributeList& atr=itr->second;
     int detector_status;
 
     detector_status=*(static_cast<const int*>((atr["detector_status"]).addressOfData()));
      if( m_debug ) m_log << MSG::DEBUG << "TGC detector status is " << detector_status << endmsg;

     if (detector_status!=0){
       int channum=itr->first;
       Identifier chamberId = m_tgcIdHelper->elementID(Identifier(channum));
       //m_cachedDeadStationsId.push_back(Identifier(channum));
       m_cachedDeadStationsId.push_back(chamberId);
     }
  } 
  
    
   if( m_debug ) m_log << MSG::VERBOSE << "Collection CondAttrListCollection CLID "
       << atrc->clID() << endmsg;


 
   return  sc; 
    
}



