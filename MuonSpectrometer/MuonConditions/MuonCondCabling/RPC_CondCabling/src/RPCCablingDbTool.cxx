/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "RPC_CondCabling/RPCCablingDbTool.h" 

#include "SGTools/TransientAddress.h"
#include "CoralBase/Attribute.h"
#include "CoralBase/AttributeListSpecification.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "Identifier/IdentifierHash.h"
#include "Identifier/Identifier.h"
#include "PathResolver/PathResolver.h"

#include <fstream>
#include <string>
#include <stdlib.h>
#include <map>

//**********************************************************
//* Author Monica Verducci monica.verducci@cern.ch
//*
//* Tool to retrieve the RPC Cabling Map from COOL DB
//* one callback aganist the  class
//* retrieving of tables from DB
//*********************************************************


RPCCablingDbTool::RPCCablingDbTool (const std::string& type, const std::string& name, const IInterface* parent) :
  AthAlgTool(type, name, parent) {
  declareInterface<IRPCCablingDbTool>(this);
  m_DataLocation="keyRPC";
  declareProperty("MapConfigurationFolder",  m_mapFolder="/RPC/CABLING/MAP_SCHEMA");
  declareProperty("MapCorrectionFolder",     m_corrFolder="/RPC/CABLING/MAP_SCHEMA_CORR");
  m_map ="";
  m_corr="";
}

StatusCode RPCCablingDbTool::updateAddress(StoreID::type /*storeID*/,
                                           SG::TransientAddress* /*tad*/,
                                           const EventContext& /*ctx*/)
{
    return StatusCode::FAILURE;
}

StatusCode RPCCablingDbTool::initialize()
{ 
  ATH_MSG_INFO("Initializing - folders names are: conf "<<m_mapFolder <<" / corr "<<m_corrFolder);
  // Get interface to IOVSvc
  m_IOVSvc = 0;
  bool CREATEIF(true);
  ATH_CHECK(service( "IOVSvc", m_IOVSvc, CREATEIF));

  // initialize the chrono service
  ATH_CHECK(service("ChronoStatSvc",m_chronoSvc));
  return StatusCode::SUCCESS;
}


StatusCode RPCCablingDbTool::loadParameters(IOVSVC_CALLBACK_ARGS_P(I,keys))
{  
  std::list<std::string>::const_iterator itr;
  for (itr=keys.begin(); itr!=keys.end(); ++itr) {
    ATH_MSG_INFO("LoadParameters "<< *itr << " I="<<I<<" ");
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
  ATH_MSG_INFO("loadRPCMap --- Load Map from DB");
  
  const CondAttrListCollection* atrc=nullptr;
  ATH_MSG_INFO("Try to read from folder <"<<m_mapFolder<<">");
  
  ATH_CHECK(detStore()->retrieve(atrc,m_mapFolder));
  ATH_MSG_INFO(" CondAttrListCollection from DB folder have been obtained with size "<< atrc->size());
  
 
  CondAttrListCollection::const_iterator itr;
  int ic=0;
  for (itr = atrc->begin(); itr != atrc->end(); ++itr) {
      ic++;
      ATH_MSG_DEBUG("Loop over CondAttrListCollection ic = "<<ic);
    const coral::AttributeList& atr=itr->second;
    
    // store configuration map in private member RPCConfMap
    m_map = *(static_cast<const std::string*>((atr["Map"]).addressOfData()));    
    
    ATH_MSG_VERBOSE("Sequence load is \n" << m_map);
    ATH_MSG_VERBOSE("End of Sequence load");

  }
  ATH_MSG_INFO("After Reading folder, Configuration string size is "<<m_map.size());
  
  return StatusCode::SUCCESS;

}


StatusCode RPCCablingDbTool::loadRPCCorr(IOVSVC_CALLBACK_ARGS_P(/*I*/,/*keys*/)) 
{
  ATH_MSG_INFO("loadRPCCorr --- Load Corrections from DB");
  
  const CondAttrListCollection* atrc=nullptr;
  ATH_MSG_INFO("Try to read from folder <"<<m_corrFolder<<">");
  
  ATH_CHECK(detStore()->retrieve(atrc,m_corrFolder));
  ATH_MSG_INFO(" CondAttrListCollection from DB folder have been obtained with size "<< atrc->size());
 
  CondAttrListCollection::const_iterator itr;
  for (itr = atrc->begin(); itr != atrc->end(); ++itr) {
    const coral::AttributeList& atr=itr->second;
    
    // store correction map in private member RPCCorrMap
    m_corr = *(static_cast<const std::string*>((atr["Map"]).addressOfData()));
   
    ATH_MSG_VERBOSE("Sequence load is \n" << m_corr);
    ATH_MSG_VERBOSE("End of Sequence load");

  }
  ATH_MSG_INFO("After Reading folder, Correction string size is "<<m_corr.size());
  return StatusCode::SUCCESS;
}

