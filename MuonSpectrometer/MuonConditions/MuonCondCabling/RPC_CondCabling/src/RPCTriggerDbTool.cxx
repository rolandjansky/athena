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
#include <iostream>


#include "RPC_CondCabling/RPCTriggerDbTool.h" 
///#include "MuonCondInterface/IRPCTriggerDbTool.h"
///#include "MuonCondSvc/MdtStringUtils.h"


#include <map>
#include "Identifier/Identifier.h"

//**********************************************************
//* Author Monica Verducci monica.verducci@cern.ch
//*
//* Tool to retrieve the RPC trigger roads from COOL DB
//* one callback aganist the  class
//* retrieving of tables from DB
//*********************************************************


RPCTriggerDbTool::RPCTriggerDbTool (const std::string& type,
                             const std::string& name,
                             const IInterface* parent)
  : AthAlgTool(type, name, parent),  
    m_log( msgSvc(), name ),
    m_debug(false),
    m_verbose(false)
{
  
  declareInterface<IRPCTriggerDbTool>(this);


  m_DataLocation="keyRPC";
  
  declareProperty("EtaTableFolder",  m_etaTableFolder="/RPC/TRIGGER/CM_THR_ETA");
  declareProperty("PhiTableFolder",  m_phiTableFolder="/RPC/TRIGGER/CM_THR_PHI");
  declareProperty("OnlyDebug",m_onlyDebug=false);
  m_etaCM_File= "";
  m_etaTh0= "";    
  //  m_etaTh1= "";    
  //  m_etaTh2= ""; 
  m_etaSequence_Th= "";	  
  m_etaInfo= "";   
  
  m_phiCM_File= "";
  m_phiTh0= "";    
  m_etaInfo= "";   
  m_trigroads.clear();
}


//StatusCode RPCTriggerDbTool::updateAddress(SG::TransientAddress* /*tad*/)
StatusCode RPCTriggerDbTool::updateAddress(StoreID::type /*storeID*/,
                                           SG::TransientAddress* /*tad*/,
                                           const EventContext& /*ctx*/)
{
    MsgStream log(msgSvc(), name());

    return StatusCode::FAILURE;
}




StatusCode RPCTriggerDbTool::initialize()
{ 
 
  m_log.setLevel(msgLevel());
  m_debug = m_log.level() <= MSG::DEBUG;
  m_verbose = m_log.level() <= MSG::VERBOSE;
  
  if( m_debug )  m_log  << MSG::INFO << "Initializing - folders names are: eta " << m_etaTableFolder 
			<< " / phi "<< m_phiTableFolder << endmsg;
  
  
  StatusCode sc = serviceLocator()->service("DetectorStore", m_detStore);
  if ( sc.isSuccess() ) {
    if( m_debug )  m_log << MSG::DEBUG  << "Retrieved DetectorStore" << endmsg;
  }else{
    m_log << MSG::ERROR << "Failed to retrieve DetectorStore" << endmsg;
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


StatusCode RPCTriggerDbTool::loadParameters(IOVSVC_CALLBACK_ARGS_P(I,keys))
{
  
  m_log.setLevel(msgLevel());
  m_debug = m_log.level() <= MSG::DEBUG;
  m_verbose = m_log.level() <= MSG::VERBOSE;
  
    
  if( m_verbose ) m_log << MSG::VERBOSE <<"LoadParameters "<<endmsg;
  
  std::list<std::string>::const_iterator itr;
  for (itr=keys.begin(); itr!=keys.end(); ++itr) {
    m_log << MSG::INFO <<"LoadParameters "<< *itr << " I="<<I<<" "<<endmsg;
    if(*itr== m_etaTableFolder) {
        StatusCode sc = loadRPCEtaTable(I,keys);
        if (sc.isFailure())
        {
            return sc;
        }
    }
    else if (*itr==m_phiTableFolder) {
        StatusCode sc = loadRPCPhiTable(I,keys);
        if (sc.isFailure())
        {
            return sc;
        }
    }
  }
  
  return StatusCode::SUCCESS;
}



StatusCode RPCTriggerDbTool::loadRPCEtaTable(IOVSVC_CALLBACK_ARGS_P(/*I*/,/*keys*/)) 
{

  
  m_log.setLevel(msgLevel());
  m_debug = m_log.level() <= MSG::DEBUG;
  m_verbose = m_log.level() <= MSG::VERBOSE;

  StatusCode sc=StatusCode::SUCCESS;
  if( m_debug ) m_log << MSG::INFO << "loadRPCEtaTable --- Load Map from DB" << endmsg;
  
  const CondAttrListCollection * atrc;
  if( m_debug )  m_log << MSG::INFO << "Try to read from folder <" << m_etaTableFolder << ">" << endmsg;
  
  sc=m_detStore->retrieve(atrc,m_etaTableFolder);
  if(sc.isFailure())  {
    m_log << MSG::ERROR 
	  << "could not retreive the CondAttrListCollection from DB folder " 
	  << m_etaTableFolder << endmsg;
    return sc;
  }
  
  else
    if( m_debug ) m_log<<MSG::INFO<<" CondAttrListCollection from DB folder have been obtained with size "<< atrc->size() <<endmsg;
  
  
  CondAttrListCollection::const_iterator itr;
  int ic=0;
  for (itr = atrc->begin(); itr != atrc->end(); ++itr) {
    ic++;
    if( m_verbose ) m_log << MSG::VERBOSE << "Loop over CondAttrListCollection ic = "<<ic<<endmsg;
    const coral::AttributeList& atr=itr->second;
    
    // store the table columns in private members 
    m_etaCM_File = *(static_cast<const std::string*>((atr["CM_File"]).addressOfData()));    
    m_vecetaCM_File.push_back(m_etaCM_File);   
    m_etaTh0 = *(static_cast<const std::string*>((atr["Th0"]).addressOfData()));      
    m_trigroads[m_etaCM_File]=m_etaTh0;
    m_vecetaTh0.push_back(m_etaTh0);  
    //m_etaTh1 = *(static_cast<const std::string*>((atr["Th1"]).addressOfData()));    
    //m_etaTh2 = *(static_cast<const std::string*>((atr["Th2"]).addressOfData()));    
    m_etaSequence_Th = *(static_cast<const std::string*>((atr["Sequence_Th"]).addressOfData()));  
    m_vecetaSequence_Th.push_back(m_etaSequence_Th);  
    m_etaInfo = *(static_cast<const std::string*>((atr["Additional_Info"]).addressOfData()));    
    m_vecetaInfo.push_back(m_etaInfo);
    if(m_onlyDebug){
      std::string fileName =  m_etaCM_File.c_str();
      std::ofstream file;
      file.open(fileName.c_str(),std::ios::app);
      std::cout << "Opening file" << std::endl;
      if (!file.is_open()) {
	std::cout << "Failed to open file named " << fileName << std::endl;
	return StatusCode::FAILURE;
      }
      
      file <<m_etaTh0;
      file.close();
    }
  }
  std::vector<std::string>::const_iterator itrdb;

  itrdb=m_vecetaCM_File.begin();
  while(itrdb<m_vecetaCM_File.end()){
    if( m_verbose ) m_log << MSG::VERBOSE << "column eta CM_File is \n" << (*itrdb) << endmsg;
    itrdb++;
    /*
      log << MSG::VERBOSE << "column eta CM_File is \n" << m_etaCM_File << endmsg;
      log << MSG::VERBOSE << "column eta Th0 is \n" << m_etaTh0 << endmsg;
      log << MSG::VERBOSE << "column eta Th1 is \n" << m_etaTh1 << endmsg;
      log << MSG::VERBOSE << "column eta Th2 is \n" << m_etaTh2 << endmsg;
      log << MSG::VERBOSE << "column eta Sequence_Th is \n" << m_etaSequence_Th << endmsg;
      log << MSG::VERBOSE << "column eta Additional_Info is \n" << m_etaInfo << endmsg;
      
      log <<"End of Sequence load"<<endmsg;
    */
  }
  if( m_debug ) m_log << MSG::INFO<<"After Reading folder, Configuration string size is "
		      <<m_etaCM_File.size()<<endmsg;
  
  return StatusCode::SUCCESS;
  
}


StatusCode RPCTriggerDbTool::loadRPCPhiTable(IOVSVC_CALLBACK_ARGS_P(/*I*/,/*keys*/)) 
{

  
  m_log.setLevel(msgLevel());
  m_debug = m_log.level() <= MSG::DEBUG;
  m_verbose = m_log.level() <= MSG::VERBOSE;


  StatusCode sc=StatusCode::SUCCESS;
  if( m_debug ) m_log << MSG::INFO << "loadRPCPhiTable --- Load info from DB" << endmsg;
  
  const CondAttrListCollection * atrc;
  if( m_debug ) m_log << MSG::INFO << "Try to read from folder <"<<m_phiTableFolder<<">"<<endmsg;
  
  sc=m_detStore->retrieve(atrc,m_phiTableFolder);
  if(sc.isFailure())  {
    m_log << MSG::ERROR 
	  << "could not retreive the CondAttrListCollection from DB folder " 
	  << m_phiTableFolder << endmsg;
    return sc;
  }
  
  else
    if( m_debug ) m_log<<MSG::INFO 
		       <<" CondAttrListCollection from DB folder have been obtained with size "
		       << atrc->size() <<endmsg;
  
 
  CondAttrListCollection::const_iterator itr;
  for (itr = atrc->begin(); itr != atrc->end(); ++itr) {
    const coral::AttributeList& atr=itr->second;
    
    // store table coloumns in private members
    
    m_phiCM_File = *(static_cast<const std::string*>((atr["CM_File"]).addressOfData()));    
    m_vecphiCM_File.push_back(m_phiCM_File);
    m_phiTh0 = *(static_cast<const std::string*>((atr["Th0"]).addressOfData()));    
    m_vecphiTh0.push_back(m_phiTh0);
    m_trigroads[m_phiCM_File]=m_phiTh0;
    m_phiInfo = *(static_cast<const std::string*>((atr["Additional_Info"]).addressOfData()));    
    m_vecphiInfo.push_back(m_phiInfo);   
    if(m_onlyDebug){
      std::string fileName_phi =  m_phiCM_File.c_str();
      std::ofstream file_phi;
      file_phi.open(fileName_phi.c_str(),std::ios::app);
      std::cout << "Opening file" << std::endl;
      if (!file_phi.is_open()) {
	std::cout << "Failed to open file named " << fileName_phi << std::endl;
	return StatusCode::FAILURE;
      }
      
      file_phi <<m_phiTh0;
      file_phi.close();
    }  
  if( m_verbose ) {
      m_log << MSG::VERBOSE << "column phi CM_File is \n" << m_phiCM_File << endmsg;
      m_log << MSG::VERBOSE << "column phi Th0 is \n" << m_phiTh0 << endmsg;
      m_log << MSG::VERBOSE << "column phi Additional_Info is \n" << m_phiInfo << endmsg;
      
      m_log <<MSG::VERBOSE << "End of Sequence load"<<endmsg;
    }
  }
  if( m_debug ) m_log<< MSG::INFO<<"After Reading folder, Correction string size is "
		       << m_phiCM_File.size()<<endmsg;
  
  return StatusCode::SUCCESS;
  
}

