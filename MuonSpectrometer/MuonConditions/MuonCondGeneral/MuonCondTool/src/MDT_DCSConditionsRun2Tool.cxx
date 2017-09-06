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

#include "MuonCondTool/MDT_DCSConditionsRun2Tool.h"
#include "MuonCondTool/MDT_MapConversion.h"

#include "MuonCondSvc/MdtStringUtils.h"


//**********************************************************
//* Author Monica Verducci monica.verducci@cern.ch
//*
//* Tool to retrieve the MDT DCS Info from COOL DB
//* retrieving of tables from DB for Run2
//*********************************************************


MDT_DCSConditionsRun2Tool::MDT_DCSConditionsRun2Tool (const std::string& type,
				    const std::string& name,
				    const IInterface* parent)
	  : AthAlgTool(type, name, parent),
	    m_detStore(0),
	    m_IOVSvc(0),
	    m_mdtIdHelper(0),
	    m_chronoSvc(0),
	    m_condMapTool("MDT_MapConversion"), 
	    m_log( msgSvc(), name ),
	    m_debug(false),
	    m_verbose(false)   
{
  
  declareInterface< IMDT_DCSConditionsRun2Tool >(this);
  
  
  m_DataLocation="keyMDTDCS";
  

  declareProperty("LVFolder",     m_lvFolder="/MDT/DCS/LV");
  declareProperty("HVFolder",     m_hvFolder="/MDT/DCS/HV");
  ///declareProperty("JTAGFolder",     m_jtagFolder="/MDT/DCS/JTAGCHSTATE");
  declareProperty("MDT_MapConversion", m_condMapTool);
  //declareProperty("Simulation_Setup",m_simulation_Setup=false);
  m_MDTChamDrop.str("EMPTY");
  m_MDTLV.str("EMPTY");
  m_MDTHV.str("EMPTY");
  m_MDTJTAG.str("EMPTY");
}


//StatusCode MDT_DCSConditionsTool::updateAddress(SG::TransientAddress* /*tad*/)
StatusCode MDT_DCSConditionsRun2Tool::updateAddress(StoreID::type /*storeID*/,
                                                    SG::TransientAddress* /*tad*/,
                                                    const EventContext& /*ctx*/)
{
//   MsgStream m_log(msgSvc(), name());
//   CLID clid        = tad->clID();
//   std::string key  = tad->name();
 
  return StatusCode::FAILURE;
}
	


StatusCode MDT_DCSConditionsRun2Tool::initialize()
{

  m_log.setLevel(msgLevel());
  m_debug = m_log.level() <= MSG::DEBUG;
  m_verbose = m_log.level() <= MSG::VERBOSE;

  m_log << MSG::VERBOSE << "Initializing - folders names are: LV "<<m_lvFolder<< " / HV "<<m_hvFolder<< endmsg;
   
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
  // tool for the conversion online -> offline ID

  sc = m_condMapTool.retrieve();

    
  if ( sc.isFailure() )
    {
      
      m_log << MSG::ERROR << "Could not retrieve MDT_MapConversion" << endmsg;
      return sc;
    }
  else
    {
      if( m_debug ) m_log<<MSG::DEBUG<<"MDT_MapConversion retrieved with statusCode = "<<sc<<" pointer = "<<m_condMapTool<<endmsg;
      
      
    }
  
    return StatusCode::SUCCESS;
}


StatusCode MDT_DCSConditionsRun2Tool::loadParameters(IOVSVC_CALLBACK_ARGS_P(I,keys))
{

  m_log.setLevel(msgLevel());
  m_debug = m_log.level() <= MSG::DEBUG;
  m_verbose = m_log.level() <= MSG::VERBOSE;


  std::list<std::string>::const_iterator itr;
  for (itr=keys.begin(); itr!=keys.end(); ++itr) {
    m_log << MSG::VERBOSE <<"LoadParameters "<< *itr << " I="<<I<<" "<<endmsg;
   
    if (*itr==m_lvFolder) {
      StatusCode sc = loadLV(I,keys);
      if (sc.isFailure())
	{
	  return sc;
	}
    }
    else if(*itr==m_hvFolder){
     StatusCode sc = loadHV(I,keys);
     if (sc.isFailure())
        {
          m_log << MSG::INFO << "Failed to load HV" << endmsg;
          return sc;
        }
    }   
    
    
  }
  
  
  return StatusCode::SUCCESS;
}




StatusCode MDT_DCSConditionsRun2Tool::loadHV(IOVSVC_CALLBACK_ARGS_P(I,keys))
{
 

  m_log.setLevel(msgLevel());
  m_debug = m_log.level() <= MSG::DEBUG;
  m_verbose = m_log.level() <= MSG::VERBOSE;
  StatusCode sc=StatusCode::SUCCESS;
  m_log << MSG::VERBOSE << "Load HV from DCS DB" << endmsg;
  const CondAttrListCollection * atrc;
  m_log << MSG::VERBOSE << "Try to read from folder <"<<m_hvFolder<<">"<<endmsg;

  // Print out callback information
  if( m_debug ) m_log << MSG::DEBUG << "Level " << I << " Keys: ";
  std::list<std::string>::const_iterator keyIt = keys.begin();
  for (; keyIt != keys.end(); ++ keyIt) if( m_debug ) m_log << MSG::DEBUG << *keyIt << " ";
  if( m_debug ) m_log << MSG::DEBUG << endmsg;
  

  sc=m_detStore->retrieve(atrc,m_hvFolder);
  
  if(sc.isFailure())  {
    m_log << MSG::ERROR
	<< "could not retreive the CondAttrListCollection from DB folder " 
	<<  m_hvFolder << endmsg;
    return sc;
  }
  
  else
    m_log<<MSG::VERBOSE<<" CondAttrListCollection from DB folder have been obtained with size "<< atrc->size() <<endmsg;



  //////////////////////////////
 
  CondAttrListCollection::const_iterator itr;
  
  unsigned int chan_index=0; 

  //  m_log<<MSG::INFO<<"index "<<chan_index<< "  chanNum :" <<atrc->chanNum(chan_index)<< " " << atrc->size()<< endmsg;
  for (itr = atrc->begin(); itr != atrc->end(); ++itr){
    

    unsigned int chanNum=atrc->chanNum(chan_index);
    std::string hv_name_ml1, hv_name_ml2;
    std::string hv_payload=atrc->chanName(chanNum);
    float hv_v0_ml1, hv_v0_ml2, hv_v1_ml1, hv_v1_ml2;
    itr=atrc->chanAttrListPair(chanNum);
    const coral::AttributeList& atr=itr->second;

    //m_log<<MSG::INFO<<" CondAttrListCollection ChanNum : "<<chanNum<<" AttributeList  size : " << atr.size() <<endmsg;

    if(atr.size()) {
      hv_name_ml1=*(static_cast<const std::string*>((atr["fsmCurrentState_ML1"]).addressOfData()));
      hv_name_ml2=*(static_cast<const std::string*>((atr["fsmCurrentState_ML2"]).addressOfData()));
      hv_v0_ml1=*(static_cast<const float*>((atr["v0set_ML1"]).addressOfData()));
      hv_v1_ml1=*(static_cast<const float*>((atr["v1set_ML1"]).addressOfData()));
      hv_v0_ml2=*(static_cast<const float*>((atr["v0set_ML2"]).addressOfData()));
      hv_v1_ml2=*(static_cast<const float*>((atr["v1set_ML2"]).addressOfData()));
      
            
      std::string delimiter2 = "_";
      std::vector<std::string> tokens2;
      
      MuonCalib::MdtStringUtils::tokenize(hv_payload,tokens2,delimiter2);
      
      for (unsigned int i=0; i<tokens2.size(); i++) {
	//m_log << MSG::INFO << "Sequence for name string load is \n" << tokens2[i]<< endmsg; 
	if(tokens2[i]!="0"){
	  // m_log << MSG::INFO << "Sequence for name string load is \n" << tokens2[i]<< endmsg; 
	}
	
      }
    
     
      if(hv_name_ml1 !="ON" && hv_name_ml1 !="STANDBY" && hv_name_ml1 !="UNKNOWN"){
	if( m_verbose ) m_log << MSG::VERBOSE << "NOT ON and NOT STANDBY HV : " <<hv_name_ml1  << " ChamberName : "<<tokens2[0] << "multilayer 1"  <<endmsg;	
	//	m_log << MSG::INFO << "NOT ON and NOT STANDBY HV : " <<hv_name_ml1  << " ChamberName : "<<tokens2[0] << "multilayer 1"  <<endmsg;	
	//	m_cachedDeadMultiLayers.push_back(1);
	int multilayer =1;
	std::string chamber_name = tokens2[0];
	Identifier ChamberId= m_condMapTool->ConvertToOffline(chamber_name);
	Identifier MultiLayerId = m_mdtIdHelper->channelID(ChamberId,multilayer,1,1);
	m_cachedDeadMultiLayersId.push_back(MultiLayerId);

      }
      
      if(hv_name_ml1=="STANDBY" && hv_v0_ml1 != hv_v1_ml1){
	
	if( m_verbose ) m_log << MSG::VERBOSE << "STANDBY HV : " << hv_name_ml1<< " ChamberName : "<<tokens2[0] << "multilayer 1"<<endmsg;	
	//m_log << MSG::INFO << "STANDBY HV : " << hv_name_ml1<< " ChamberName : "<<tokens2[0] << "multilayer 1"<<endmsg;	
	
	int multilayer =1;
	std::string chamber_name = tokens2[0];
	Identifier ChamberId= m_condMapTool->ConvertToOffline(chamber_name);
	Identifier MultiLayerId = m_mdtIdHelper->channelID(ChamberId,multilayer,1,1);
	m_cachedDeadMultiLayersId.push_back(MultiLayerId);
      }

      if(hv_name_ml2 !="ON" && hv_name_ml2 !="STANDBY" && hv_name_ml2 !="UNKNOWN"){
	if( m_verbose ) m_log << MSG::VERBOSE << "NOT ON and NOT STANDBY HV : " <<hv_name_ml2  << " ChamberName : "<<tokens2[0] << "multilayer 2"  <<endmsg;
	//m_log << MSG::INFO << "NOT ON and NOT STANDBY HV : " <<hv_name_ml2  << " ChamberName : "<<tokens2[0] << "multilayer 2"  <<endmsg;	
	//m_cachedDeadMultiLayers.push_back(2);
	int multilayer =1;
	std::string chamber_name = tokens2[0];
	Identifier ChamberId= m_condMapTool->ConvertToOffline(chamber_name);
	Identifier MultiLayerId = m_mdtIdHelper->channelID(ChamberId,multilayer,1,1);
	m_cachedDeadMultiLayersId.push_back(MultiLayerId);

      }
      
      if(hv_name_ml2=="STANDBY" && hv_v0_ml2 != hv_v1_ml2){
	
	if( m_verbose ) m_log << MSG::VERBOSE << "STANDBY HV : " << hv_name_ml2<< " ChamberName : "<<tokens2[0] << "multilayer 2"<<endmsg;	
	//m_log << MSG::INFO << "STANDBY HV : " << hv_name_ml2<< " ChamberName : "<<tokens2[0] << "multilayer 2"<<endmsg;	
	
	int multilayer =2;
	std::string chamber_name = tokens2[0];
	Identifier ChamberId= m_condMapTool->ConvertToOffline(chamber_name);
	Identifier MultiLayerId = m_mdtIdHelper->channelID(ChamberId,multilayer,1,1);
	m_cachedDeadMultiLayersId.push_back(MultiLayerId);
      }

      if(hv_name_ml2 !="ON" && hv_name_ml2 !="STANDBY" && hv_name_ml2 !="UNKNOWN" && hv_name_ml1 !="ON" && hv_name_ml1 !="STANDBY" && hv_name_ml1 !="UNKNOWN" ){
	
      m_cachedDeadStations.push_back(tokens2[0]);
      std::string chamber_name=tokens2[0];
      
      Identifier ChamberId= m_condMapTool->ConvertToOffline(chamber_name);
      m_cachedDeadStationsId.push_back(ChamberId);

      }
      if(hv_name_ml2=="STANDBY" && hv_v0_ml2 != hv_v1_ml2 && hv_name_ml1=="STANDBY" && hv_v0_ml1 != hv_v1_ml1){
 	
	m_cachedDeadStations.push_back(tokens2[0]);
      std::string chamber_name=tokens2[0];
      
      Identifier ChamberId= m_condMapTool->ConvertToOffline(chamber_name);
      m_cachedDeadStationsId.push_back(ChamberId);
      
      }

      
      
    }  
    chan_index++;}
  
  
  return StatusCode::SUCCESS;
}
StatusCode MDT_DCSConditionsRun2Tool::loadLV(IOVSVC_CALLBACK_ARGS_P(I,keys))
{
  m_log.setLevel(msgLevel());
  m_debug = m_log.level() <= MSG::DEBUG;
  m_verbose = m_log.level() <= MSG::VERBOSE;

  StatusCode sc=StatusCode::SUCCESS;
  m_log << MSG::VERBOSE << "Load LV from DCS DB" << endmsg;
  const CondAttrListCollection * atrc;
  m_log << MSG::VERBOSE << "Try to read from folder <"<<m_lvFolder<<">"<<endmsg;

  // Print out callback information
  m_log << MSG::DEBUG << "Level " << I << " Keys: ";
  std::list<std::string>::const_iterator keyIt = keys.begin();
  for (; keyIt != keys.end(); ++ keyIt) if( m_debug ) m_log << MSG::DEBUG << *keyIt << " ";
  if( m_debug ) m_log << MSG::DEBUG << endmsg;

  sc=m_detStore->retrieve(atrc,m_lvFolder);

  if(sc.isFailure())  {
    m_log << MSG::ERROR
        << "could not retreive the CondAttrListCollection from DB folder "
        << m_lvFolder << endmsg;
    return sc;
  }
  
  else
     if( m_verbose ) m_log<<MSG::VERBOSE<<" CondAttrListCollection from DB folder have been obtained with size "<< atrc->size() <<endmsg;
  
  CondAttrListCollection::const_iterator itr;
  
  unsigned int chan_index=0;
  
  for (itr = atrc->begin(); itr != atrc->end(); ++itr){
    
    m_log<<MSG::DEBUG<<"index "<<chan_index<< "  chanNum :" <<atrc->chanNum(chan_index)<< endmsg;
    unsigned int chanNum=atrc->chanNum(chan_index);
    std::string hv_name;
    std::string hv_payload=atrc->chanName(chanNum);
    
    itr=atrc-> chanAttrListPair(chanNum);
    const coral::AttributeList& atr=itr->second;
    //m_log<<MSG::DEBUG<<" CondAttrListCollection ChanNum : "<<chanNum<<" AttributeList  size : " << atr.size() <<endmsg;
    
    if(atr.size()){
      hv_name=*(static_cast<const std::string*>((atr["fsmCurrentState_LV"]).addressOfData()));
      //m_log<<MSG::DEBUG<<" CondAttrListCollection ChanNum : "<<chanNum<<" ChanName : " << atrc->chanName(chanNum) <<endmsg;
      std::string delimiter = " ";
      std::vector<std::string> tokens;
      MuonCalib::MdtStringUtils::tokenize(hv_name,tokens,delimiter);
      for (unsigned int i=0; i<tokens.size(); i++)
	{
	  
	  if(tokens[i]!="0"){
	    //m_log << MSG::VERBOSE << "Sequence for status string load is " << tokens[i]<< endmsg;
	  }
	}
       
      std::string delimiter2 = "_";
      std::vector<std::string> tokens2;
      MuonCalib::MdtStringUtils::tokenize(hv_payload,tokens2,delimiter2);
      
      for (unsigned int i=0; i<tokens2.size(); i++) {
	if(tokens2[i]!="0"){
	  //m_log << MSG::VERBOSE << "Sequence for name string load is " << tokens2[i]<< endmsg;
	}
	
      }
      
      if(tokens[0]!="ON"){
	 if( m_verbose ) m_log << MSG::VERBOSE << "NOT ON LV: " << tokens[0]<< " ChamberName : "<<tokens2[0] <<endmsg;
	m_cachedDeadLVStations.push_back(tokens2[0]);
	std::string chamber_name= tokens2[0];
	Identifier ChamberId= m_condMapTool->ConvertToOffline(chamber_name);
	m_cachedDeadLVStationsId.push_back(ChamberId);
	 if( m_verbose ) m_log<<MSG::VERBOSE<<"Chamber off from LV Chamber !=ON "<<tokens2[0] <<endmsg;
	 m_log<<MSG::VERBOSE<<"Chamber off from LV Chamber !=ON "<<tokens2[0] <<endmsg;
      }
     
    }
    chan_index++;
  }
  
  //merge deadStations with deadLVStations, then sort the vector elements and
  //finally remove duplicates
   if( m_verbose ) m_log << MSG::VERBOSE << "Now merging the  DeadStations with DeadLVStations" <<  endmsg;
  m_cachedDeadStations.insert( m_cachedDeadStations.end(),
			       m_cachedDeadLVStations.begin(),m_cachedDeadLVStations.end());

  std::sort(m_cachedDeadStations.begin(),m_cachedDeadStations.end());  
  std::vector<std::string>::const_iterator it;
  it= std::unique(m_cachedDeadStations.begin(),m_cachedDeadStations.end());
  m_cachedDeadStations.resize(it -m_cachedDeadStations.begin());
  
  //merge deadStationsId with deadLVStationsId, then sort the vector elements and
  //finally remove duplicates

   if( m_verbose ) m_log << MSG::VERBOSE << "Now merging the  DeadStationsId with DeadLVStationsId" <<  endmsg;
  m_cachedDeadStationsId.insert( m_cachedDeadStationsId.end(),
				 m_cachedDeadLVStationsId.begin(),m_cachedDeadLVStationsId.end());
  std::sort(m_cachedDeadStationsId.begin(),m_cachedDeadStationsId.end(),compareId);  
  std::vector<Identifier>::const_iterator itId;
  itId= std::unique(m_cachedDeadStationsId.begin(),m_cachedDeadStationsId.end());
  m_cachedDeadStationsId.resize(itId -m_cachedDeadStationsId.begin());
  
  
  return StatusCode::SUCCESS;
  
}


/*
StatusCode MDT_DCSConditionsRun2Tool::loadJTAG(IOVSVC_CALLBACK_ARGS_P(I,keys))
{

  m_log.setLevel(msgLevel());
  m_debug = m_log.level() <= MSG::DEBUG;
  m_verbose = m_log.level() <= MSG::VERBOSE;
  StatusCode sc=StatusCode::SUCCESS;
  m_log << MSG::INFO << "Load JTAG from DCS DB" << endmsg;
  const CondAttrListCollection * atrc;
  
  m_log << MSG::INFO << "Try to read from folder <"<<m_jtagFolder<<">"<<endmsg;
  
  // Print out callback information
  if( m_debug ) m_log << MSG::DEBUG << "Level " << I << " Keys: ";
  std::list<std::string>::const_iterator keyIt = keys.begin();
  for (; keyIt != keys.end(); ++ keyIt)  if( m_debug ) m_log << MSG::DEBUG << *keyIt << " ";
  if( m_debug ) m_log << MSG::DEBUG << endmsg;
  
  sc=m_detStore->retrieve(atrc,m_jtagFolder);
  
  if(sc.isFailure())  {
    m_log << MSG::ERROR
        << "could not retreive the CondAttrListCollection from DB folder "
        <<  m_jtagFolder << endmsg;
    return sc;
  }
  
  else
     if( m_debug ) m_log<<MSG::DEBUG<<" CondAttrListCollection from DB folder have been obtained with size "<< atrc->size() <<endmsg;
  
  CondAttrListCollection::const_iterator itr;
  
  unsigned int chan_index=0;
  for (itr = atrc->begin(); itr != atrc->end(); ++itr){
    
    //m_log<<MSG::DEBUG<<"index "<<chan_index<< "  chanNum :" <<atrc->chanNum(chan_index)<< endmsg;
    unsigned int chanNum=atrc->chanNum(chan_index);
    std::string hv_name;
    std::string hv_payload=atrc->chanName(chanNum);
    
    itr=atrc-> chanAttrListPair(chanNum);
    const coral::AttributeList& atr=itr->second;
        
    if(atr.size()==1){
      hv_name=*(static_cast<const std::string*>((atr["fsmCurrentState_JTAG"]).addressOfData()));
      std::string delimiter = " ";
      std::vector<std::string> tokens;
      MuonCalib::MdtStringUtils::tokenize(hv_name,tokens,delimiter);
      for (unsigned int i=0; i<tokens.size(); i++)
	{
	  if(tokens[i]!="0"){
	  }
	}
      
      std::string delimiter2 = "_";
      std::vector<std::string> tokens2;
      MuonCalib::MdtStringUtils::tokenize(hv_payload,tokens2,delimiter2);
      
      for (unsigned int i=0; i<tokens2.size(); i++) {
	if(tokens2[i]!="0"){
	}
	
      }
      
      if(tokens[0]!="INITIALIZED"){

	m_cachedDeadJTAGStatus.push_back(tokens2[0]);
	std::string chamber_name= tokens2[0];
	Identifier ChamberId= m_condMapTool->ConvertToOffline(chamber_name);
	m_cachedDeadJTAGStatusId.push_back(ChamberId);
      }
    }
    
    chan_index++;
  }
    
  
  return StatusCode::SUCCESS;
  
  
  
}
*/




