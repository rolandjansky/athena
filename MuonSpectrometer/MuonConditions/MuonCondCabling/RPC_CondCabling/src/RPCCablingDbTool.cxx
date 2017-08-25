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

#include "MuonIdHelpers/RpcIdHelper.h"
#include "PathResolver/PathResolver.h"

#include <fstream>
#include <string>
#include <stdlib.h>



#include "RPC_CondCabling/RPCCablingDbTool.h" 
#include "MuonCondInterface/IRPCCablingDbTool.h"


#include <map>
#include "Identifier/Identifier.h"

//**********************************************************
//* Author Monica Verducci monica.verducci@cern.ch
//*
//* Tool to retrieve the RPC Cabling Map from COOL DB
//* one callback aganist the  class
//* retrieving of tables from DB
//*********************************************************


RPCCablingDbTool::RPCCablingDbTool (const std::string& type,
                             const std::string& name,
                             const IInterface* parent)
  : AthAlgTool(type, name, parent)  
{

  declareInterface<IRPCCablingDbTool>(this);


  m_DataLocation="keyRPC";
  
  declareProperty("MapConfigurationFolder",  m_mapFolder="/RPC/CABLING/MAP_SCHEMA");
  declareProperty("MapCorrectionFolder",     m_corrFolder="/RPC/CABLING/MAP_SCHEMA_CORR");
  m_map ="";
  m_corr="";
  
}

//StatusCode RPCCablingDbTool::updateAddress(SG::TransientAddress* /*tad*/)
StatusCode RPCCablingDbTool::updateAddress(StoreID::type /*storeID*/,
                                           SG::TransientAddress* /*tad*/,
                                           const EventContext& /*ctx*/)
{
    MsgStream log(msgSvc(), name());
    //CLID clid        = tad->clID();
    //std::string key  = tad->name();
    //    if ( == clid && m_DataLocation == key)
    //{
    //    log << MSG::DEBUG << "OK " << endmsg;
    //    return StatusCode::SUCCESS;
    //}
    return StatusCode::FAILURE;
}




StatusCode RPCCablingDbTool::initialize()
{ 
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "Initializing - folders names are: conf "<<m_mapFolder <<" / corr "<<m_corrFolder<< endmsg;
   				    

  StatusCode sc = serviceLocator()->service("DetectorStore", m_detStore);
  if ( sc.isSuccess() ) {
    log << MSG::DEBUG << "Retrieved DetectorStore" << endmsg;
  }else{
    log << MSG::ERROR << "Failed to retrieve DetectorStore" << endmsg;
    return sc;
  }

  // retrieve the rpc id helper
  sc = m_detStore->retrieve(m_rpcIdHelper, "RPCIDHELPER" );
  if (!sc.isSuccess()) {
    log << MSG::ERROR << "Can't retrieve RpcIdHelper" << endmsg;
    return sc;
  }

  // Get interface to IOVSvc
  m_IOVSvc = 0;
  bool CREATEIF(true);
  sc = service( "IOVSvc", m_IOVSvc, CREATEIF );
  if ( sc.isFailure() )
  {
       log << MSG::ERROR << "Unable to get the IOVSvc" << endmsg;
       return StatusCode::FAILURE;
  }

  if(sc.isFailure()) return StatusCode::FAILURE;



  // initialize the chrono service
  sc = service("ChronoStatSvc",m_chronoSvc);
  if (sc != StatusCode::SUCCESS) {
    log << MSG::ERROR << "Could not find the ChronoSvc" << endmsg;
    return sc;
  }

 
//   const DataHandle<CondAttrListCollection> MapData;
//   sc=m_detStore->regFcn(&IRPCCablingDbTool::loadRPCMap,
// 			dynamic_cast<IRPCCablingDbTool*>(this),
// 			MapData, m_mapFolder);
  
  if(sc.isFailure()) return StatusCode::FAILURE;
  
  


   return StatusCode::SUCCESS;

}


StatusCode RPCCablingDbTool::loadParameters(IOVSVC_CALLBACK_ARGS_P(I,keys))
{
  MsgStream log(msgSvc(), name());
  
  std::list<std::string>::const_iterator itr;
  for (itr=keys.begin(); itr!=keys.end(); ++itr) {
    log << MSG::INFO <<"LoadParameters "<< *itr << " I="<<I<<" "<<endmsg;
    if(*itr==m_mapFolder) {
        StatusCode sc = loadRPCMap(I,keys);
        if (sc.isFailure())
        {
            return sc;
        }
    }
    else if (*itr==m_corrFolder) {
        StatusCode sc = loadRPCCorr(I,keys);
        if (sc.isFailure())
        {
            return sc;
        }
    }
  }
  
  return StatusCode::SUCCESS;
}



StatusCode RPCCablingDbTool::loadRPCMap(IOVSVC_CALLBACK_ARGS_P(/*I*/,/*keys*/)) 
{
  MsgStream log(msgSvc(), name());
  StatusCode sc=StatusCode::SUCCESS;
  log << MSG::INFO << "loadRPCMap --- Load Map from DB" << endmsg;
  
  const CondAttrListCollection * atrc;
  log << MSG::INFO << "Try to read from folder <"<<m_mapFolder<<">"<<endmsg;
  
  sc=m_detStore->retrieve(atrc,m_mapFolder);
  if(sc.isFailure())  {
    log << MSG::ERROR 
	<< "could not retreive the CondAttrListCollection from DB folder " 
	<< m_mapFolder << endmsg;
    return sc;
  }
  
  else
       log<<MSG::INFO<<" CondAttrListCollection from DB folder have been obtained with size "<< atrc->size() <<endmsg;
  
 
  CondAttrListCollection::const_iterator itr;
  int ic=0;
  for (itr = atrc->begin(); itr != atrc->end(); ++itr) {
      ic++;
      log << MSG::INFO << "Loop over CondAttrListCollection ic = "<<ic<<endmsg;
    const coral::AttributeList& atr=itr->second;
    
    // store configuration map in private member RPCConfMap
    m_map = *(static_cast<const std::string*>((atr["Map"]).addressOfData()));    
    
    log << MSG::VERBOSE << "Sequence load is \n" << m_map <<endmsg;
    log <<"End of Sequence load"<<endmsg;

  }
  log<< MSG::INFO<<"After Reading folder, Configuration string size is "<<m_map.size()<<endmsg;
  
  return StatusCode::SUCCESS;

}


StatusCode RPCCablingDbTool::loadRPCCorr(IOVSVC_CALLBACK_ARGS_P(/*I*/,/*keys*/)) 
{
  MsgStream log(msgSvc(), name());
  StatusCode sc=StatusCode::SUCCESS;
  log << MSG::INFO << "loadRPCCorr --- Load Corrections from DB" << endmsg;
  
  const CondAttrListCollection * atrc;
  log << MSG::INFO << "Try to read from folder <"<<m_corrFolder<<">"<<endmsg;
  
  sc=m_detStore->retrieve(atrc,m_corrFolder);
  if(sc.isFailure())  {
    log << MSG::ERROR 
	<< "could not retreive the CondAttrListCollection from DB folder " 
	<< m_corrFolder << endmsg;
    return sc;
  }
  
  else
       log<<MSG::INFO<<" CondAttrListCollection from DB folder have been obtained with size "<< atrc->size() <<endmsg;
  
 
  CondAttrListCollection::const_iterator itr;
  for (itr = atrc->begin(); itr != atrc->end(); ++itr) {
    const coral::AttributeList& atr=itr->second;
    
    // store correction map in private member RPCCorrMap
    m_corr = *(static_cast<const std::string*>((atr["Map"]).addressOfData()));
   
    log << MSG::VERBOSE << "Sequence load is \n" << m_corr <<endmsg;
    log <<"End of Sequence load"<<endmsg;

  }
  log<< MSG::INFO<<"After Reading folder, Correction string size is "<<m_corr.size()<<endmsg;

  return StatusCode::SUCCESS;
 
}

