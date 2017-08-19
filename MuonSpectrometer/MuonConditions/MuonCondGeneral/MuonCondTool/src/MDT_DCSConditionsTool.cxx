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
//#include "MuonCondInterface/IMDT_DCSConditionsTool.h" 
#include "MuonCondTool/MDT_DCSConditionsTool.h"
#include "MuonCondTool/MDT_MapConversion.h"

#include "MuonCondSvc/MdtStringUtils.h"

//#include "RDBAccessSvc/IRDBAccessSvc.h"
//#include "RDBAccessSvc/IRDBRecord.h"
//#include "RDBAccessSvc/IRDBRecordset.h"
//#include "GeoModelSvc/IGeoModelSvc.h"
//**********************************************************
//* Author Monica Verducci monica.verducci@cern.ch
//*
//* Tool to retrieve the MDT DCS Info from COOL DB
//* retrieving of tables from DB
//*********************************************************


MDT_DCSConditionsTool::MDT_DCSConditionsTool (const std::string& type,
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
  
  declareInterface< IMDT_DCSConditionsTool >(this);
  
  
  m_DataLocation="keyMDTDCS";
  
  declareProperty("DropChamberFolder",     m_dropchamberFolder="/MDT/DCS/DROPPEDCH");
  declareProperty("LVFolder",     m_lvFolder="/MDT/DCS/PSLVCHSTATE");
  declareProperty("HVFolder",     m_hvFolder="/MDT/DCS/PSHVMLSTATE");
  declareProperty("JTAGFolder",     m_jtagFolder="/MDT/DCS/JTAGCHSTATE");
  declareProperty("SetPointsV0Folder",     m_setPointsV0Folder="/MDT/DCS/PSV0SETPOINTS");
  declareProperty("SetPointsV1Folder",     m_setPointsV1Folder="/MDT/DCS/PSV1SETPOINTS");
  declareProperty("Check_on_setPoint",m_check_on_setPoint=false);
  declareProperty("Simulation_Setup",m_simulation_Setup=false);
  declareProperty("MDT_MapConversion", m_condMapTool);
  
  m_MDTChamDrop.str("EMPTY");
  m_MDTLV.str("EMPTY");
  m_MDTHV.str("EMPTY");
  m_MDTJTAG.str("EMPTY");
}


//StatusCode MDT_DCSConditionsTool::updateAddress(SG::TransientAddress* /*tad*/)
StatusCode MDT_DCSConditionsTool::updateAddress(StoreID::type /*storeID*/,
                                                SG::TransientAddress* /*tad*/,
                                                const EventContext& /*ctx*/)
{
//   MsgStream m_log(msgSvc(), name());
//   CLID clid        = tad->clID();
//   std::string key  = tad->name();
 
  return StatusCode::FAILURE;
}
	


StatusCode MDT_DCSConditionsTool::initialize()
{

  m_log.setLevel(msgLevel());
  m_debug = m_log.level() <= MSG::DEBUG;
  m_verbose = m_log.level() <= MSG::VERBOSE;

  m_log << MSG::INFO << "Initializing - folders names are: ChamberDropped "<<m_dropchamberFolder <<" / LV "<<m_lvFolder<< " / HV "<<m_hvFolder<< endmsg;
   
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


StatusCode MDT_DCSConditionsTool::loadParameters(IOVSVC_CALLBACK_ARGS_P(I,keys))
{

  m_log.setLevel(msgLevel());
  m_debug = m_log.level() <= MSG::DEBUG;
  m_verbose = m_log.level() <= MSG::VERBOSE;

  bool isHVfolder=false, isV0folder=false, isV1folder=false; 
  std::list<std::string>::const_iterator itr;
  for (itr=keys.begin(); itr!=keys.end(); ++itr) {
    m_log << MSG::INFO <<"LoadParameters "<< *itr << " I="<<I<<" "<<endmsg;
    if(*itr==m_dropchamberFolder) {
      StatusCode sc = loadDropChamber(I,keys);
      if (sc.isFailure())
	{
	  return sc;
	}
    }
    else if (*itr==m_lvFolder) {
      StatusCode sc = loadLV(I,keys);
      if (sc.isFailure())
	{
	  return sc;
	}
    }
    else if (*itr==m_jtagFolder && m_simulation_Setup==false) {
      StatusCode sc = loadJTAG(I,keys);
      if (sc.isFailure())
        {
          return sc;
        }
    }
    // array of folders for the hv : hv, setpointsV0, setpointsV1 

    else if(*itr==m_hvFolder && m_simulation_Setup==false) isHVfolder=true;
    else if(*itr==m_setPointsV0Folder && m_simulation_Setup==false) isV0folder=true;
    else if(*itr==m_setPointsV1Folder && m_simulation_Setup==false) isV1folder= true;

  }

  if(isHVfolder && isV0folder && isV1folder && m_simulation_Setup==false){
     StatusCode sc = loadHV(I,keys);
     if (sc.isFailure())
        {
          m_log << MSG::INFO << "Failed to load HV" << endmsg;
          return sc;
        }
  }


  
  return StatusCode::SUCCESS;
}


	
StatusCode MDT_DCSConditionsTool::loadDropChamber(IOVSVC_CALLBACK_ARGS_P(I,keys))
{

  m_log.setLevel(msgLevel());
  m_debug = m_log.level() <= MSG::DEBUG;
  m_verbose = m_log.level() <= MSG::VERBOSE; 
  StatusCode sc=StatusCode::SUCCESS;
  m_log << MSG::INFO << "Load Dropped Chamber from DCS DB" << endmsg;
  
  // Print out callback information
  if( m_debug ) m_log << MSG::DEBUG << "Level " << I << " Keys: ";
  std::list<std::string>::const_iterator keyIt = keys.begin();
  for (; keyIt != keys.end(); ++ keyIt) if( m_debug ) m_log << MSG::DEBUG << *keyIt << " ";
  if( m_debug ) m_log << MSG::DEBUG << endmsg;
 
  
	
  const CondAttrListCollection * atrc;
  m_log << MSG::INFO << "Try to read from folder <"<<m_dropchamberFolder<<">"<<endmsg;
  
  sc=m_detStore->retrieve(atrc,m_dropchamberFolder);
  if(sc.isFailure())  {
    m_log << MSG::ERROR
	<< "could not retreive the CondAttrListCollection from DB folder " 
	<< m_dropchamberFolder << endmsg;
    return sc;
	  }
  
  else
    if( m_debug ) m_log<<MSG::DEBUG<<" CondAttrListCollection from DB folder have been obtained with size "<< atrc->size() <<endmsg;
  
 
  CondAttrListCollection::const_iterator itr;
 
  for (itr = atrc->begin(); itr != atrc->end(); ++itr) {
 
 
    const coral::AttributeList& atr=itr->second;
    
    std::string chamber_dropped;
    
    chamber_dropped=*(static_cast<const std::string*>((atr["Chambers_disabled"]).addressOfData()));
    
    std::string delimiter = " ";
    std::vector<std::string> tokens;
    MuonCalib::MdtStringUtils::tokenize(chamber_dropped,tokens,delimiter);
    for (unsigned int i=0; i<tokens.size(); i++) {
      if(tokens[i]!="0"){

        m_cachedDeadStations.push_back(tokens[i]);
	std::string chamber_name=tokens[i];
	 
	Identifier ChamberId= m_condMapTool->ConvertToOffline(chamber_name);
	m_cachedDeadStationsId.push_back(ChamberId);
	//m_log<<MSG::DEBUG<<"Chamber off from DROPPED Chamber  "<<tokens[i] <<endmsg;
      }
    }
     
  }
 
  
  
  return StatusCode::SUCCESS;
}


StatusCode MDT_DCSConditionsTool::loadHV(IOVSVC_CALLBACK_ARGS_P(I,keys))
{
 

  m_log.setLevel(msgLevel());
  m_debug = m_log.level() <= MSG::DEBUG;
  m_verbose = m_log.level() <= MSG::VERBOSE;
  StatusCode sc=StatusCode::SUCCESS;
  m_log << MSG::INFO << "Load HV from DCS DB" << endmsg;
  const CondAttrListCollection * atrc;
  m_log << MSG::INFO << "Try to read from folder <"<<m_hvFolder<<">"<<endmsg;

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
    m_log<<MSG::INFO<<" CondAttrListCollection from DB folder have been obtained with size "<< atrc->size() <<endmsg;


  // setpointsV0

  const CondAttrListCollection * atrc_v0;
  m_log << MSG::INFO << "Try to read from folder <"<<m_setPointsV0Folder<<">"<<endmsg;

  sc=m_detStore->retrieve(atrc_v0,m_setPointsV0Folder);
  
  if(sc.isFailure())  {
    m_log << MSG::ERROR
	<< "could not retreive the CondAttrListCollection from DB folder V0" 
	<<  m_setPointsV0Folder << endmsg;
    return sc;
  }
  
  else
    if( m_debug ) m_log<<MSG::DEBUG<<" CondAttrListCollection from DB  set points V0 folder have been obtained with size "<< atrc_v0->size() <<endmsg;


  // setpointsV1

  const CondAttrListCollection * atrc_v1;
  m_log << MSG::INFO << "Try to read from folder <"<<m_setPointsV1Folder<<">"<<endmsg;

  sc=m_detStore->retrieve(atrc_v1,m_setPointsV1Folder);
  
  if(sc.isFailure())  {
    m_log << MSG::ERROR
	<< "could not retrieve the CondAttrListCollection from DB folder V1" 
	<<  m_setPointsV1Folder << endmsg;
    return sc;
  }
  
  else
    if( m_verbose ) m_log<<MSG::VERBOSE<<" CondAttrListCollection set points V1 from DB folder have been obtained with size "<< atrc_v1->size() <<endmsg;

  //////////////////////////////
 
  CondAttrListCollection::const_iterator itr;
  
  unsigned int chan_index=0; 
  
  for (itr = atrc->begin(); itr != atrc->end(); ++itr){
    

    unsigned int chanNum=atrc->chanNum(chan_index);
    std::string hv_name;
    std::string hv_payload=atrc->chanName(chanNum);
    
    itr=atrc->chanAttrListPair(chanNum);
    const coral::AttributeList& atr=itr->second;
    //m_log<<MSG::DEBUG<<" CondAttrListCollection ChanNum : "<<chanNum<<" AttributeList  size : " << atr.size() <<endmsg;
    
    if(atr.size()==1){
      hv_name=*(static_cast<const std::string*>((atr["fsm_currentState"]).addressOfData()));
      //m_log<<MSG::DEBUG<<" CondAttrListCollection ChanNum  : "<<chanNum<<" ChanName : " << atrc->chanName(chanNum) <<endmsg;
      std::string delimiter = " ";
      std::vector<std::string> tokens;
      MuonCalib::MdtStringUtils::tokenize(hv_name,tokens,delimiter);
      for (unsigned int i=0; i<tokens.size(); i++)
	{
	  
	  if(tokens[i]!="0"){
	    //m_log << MSG::DEBUG << "Sequence for status string load is \n" << tokens[i]<< endmsg;
	  }
	}
      
      
      std::string delimiter2 = "_";
      std::vector<std::string> tokens2;
      MuonCalib::MdtStringUtils::tokenize(hv_payload,tokens2,delimiter2);
      
      for (unsigned int i=0; i<tokens2.size(); i++) {
	if(tokens2[i]!="0"){
	  //m_log << MSG::DEBUG << "Sequence for name string load is \n" << tokens2[i]<< endmsg; 
	}
	
      }
      
      if(tokens[0]!="ON" && tokens[0]!="STANDBY" && tokens[0]!="UNKNOWN"){
	
	if( m_verbose ) m_log << MSG::VERBOSE << "NOT ON and NOT STANDBY HV : " << tokens[0]<< " ChamberName : "<<tokens2[2] << "multilayer" << tokens2[3]<<endmsg;	
	m_cachedDeadMultiLayers.push_back(tokens2[2]);
	int multilayer =atoi(const_cast<char*>(tokens2[3].c_str()));
	std::string chamber_name = tokens2[2];
	Identifier ChamberId= m_condMapTool->ConvertToOffline(chamber_name);
	Identifier MultiLayerId = m_mdtIdHelper->channelID(ChamberId,multilayer,1,1);
	m_cachedDeadMultiLayersId.push_back(MultiLayerId);
      }
      if(tokens[0]=="STANDBY"){
	
	if( m_verbose ) m_log << MSG::VERBOSE << "STANDBY HV : " << tokens[0]<< " ChamberName : "<<tokens2[2] << "multilayer" << tokens2[3]<<endmsg;	
	
	int multilayer =atoi(const_cast<char*>(tokens2[3].c_str()));
	std::string chamber_name = tokens2[2];
	Identifier ChamberId= m_condMapTool->ConvertToOffline(chamber_name);
	Identifier MultiLayerId = m_mdtIdHelper->channelID(ChamberId,multilayer,1,1);
	m_cachedDeadMultiLayersId_standby.push_back(MultiLayerId);
	
	//m_Chamber_Naming_standby.insert(std::make_pair(int(chanNum),MultiLayerId));
	m_Chamber_Naming_standby[int(chanNum)]=MultiLayerId;
      }



    }
    chan_index++;
  }


  if(m_check_on_setPoint){
  // check on Setpoints V0 
  CondAttrListCollection::const_iterator itr_v0;
  
  unsigned int chan_index_v0=0; 
    
  for (itr_v0 = atrc_v0->begin(); itr_v0 != atrc_v0->end(); ++itr_v0){
    
    //m_log<<MSG::DEBUG<<"index "<<chan_index_v0<< "  chanNum :" <<atrc_v0->chanNum(chan_index_v0)<< endmsg;
    unsigned int chanNum=atrc_v0->chanNum(chan_index_v0);
    
    float setPointsV0_name;
    std::string setPointsV0_payload=atrc_v0->chanName(chanNum);
       
    itr_v0=atrc_v0->chanAttrListPair(chanNum);
    const coral::AttributeList& atr_v0=itr_v0->second;
    //m_log<<MSG::DEBUG<<" CondAttrListCollection ChanNum : "<<chanNum<<" AttributeList  size : " <<endmsg;
        
    if(atr_v0.size()==1){
      setPointsV0_name=*(static_cast<const float*>((atr_v0["readBackSettings_v0"]).addressOfData()));
      //m_log << MSG::DEBUG << "Sequence for name string load is inside V0\n" << "  "<<setPointsV0_name<< endmsg; 
      //m_log<<MSG::DEBUG<<" CondAttrListCollection ChanName : "<<atrc_v0->chanName(chanNum) <<endmsg;
      std::string delimiter2 = "_";
      std::vector<std::string> tokens2;
      MuonCalib::MdtStringUtils::tokenize(setPointsV0_payload,tokens2,delimiter2);
      //m_ChamberML_V0_chanum.insert(std::make_pair(int(chanNum),float(setPointsV0_name)));
      m_ChamberML_V0_chanum[int(chanNum)]=float(setPointsV0_name);

      //m_log << MSG::DEBUG << "Sequence for name string load is inside V0\n" << setPointsV0_payload<< endmsg; 

      for (unsigned int i=0; i<tokens2.size(); i++) {
	//	m_log << MSG::DEBUG << "Sequence for name string load is inside V0\n" <<i << "  "<<tokens2[i]<< endmsg; 
	if(tokens2[i]!="0"){
	
	}

      }
      
      int multilayer =atoi(const_cast<char*>(tokens2[3].c_str()));
      std::string chamber_name = tokens2[2];
      
      Identifier ChamberId= m_condMapTool->ConvertToOffline(chamber_name);
      Identifier MultiLayerId = m_mdtIdHelper->channelID(ChamberId,multilayer,1,1);
      //m_ChamberML_V0.insert(std::make_pair(MultiLayerId,setPointsV0_name));
      m_ChamberML_V0[MultiLayerId]=setPointsV0_name;
    
    }
    chan_index_v0++;
  }


 
  
  // check on Setpoints V1

  CondAttrListCollection::const_iterator itr_v1;
  
  unsigned int chan_index_v1=0; 
  
  for (itr_v1 = atrc_v1->begin(); itr_v1 != atrc_v1->end(); ++itr_v1){
    
    //m_log<<MSG::DEBUG<<"index "<<chan_index_v1<< "  chanNum :" <<atrc_v1->chanNum(chan_index_v1)<< endmsg;
    unsigned int chanNum=atrc_v1->chanNum(chan_index_v1);
    float setPointsV1_name;
    std::string setPointsV1_payload=atrc_v1->chanName(chanNum);
    
    itr_v1=atrc_v1-> chanAttrListPair(chanNum);
    const coral::AttributeList& atr_v1=itr_v1->second;
  
    if(atr_v1.size()==1){
      setPointsV1_name=*(static_cast<const float*>((atr_v1["readBackSettings_v1"]).addressOfData()));
         
      std::string delimiter2 = "_";
      std::vector<std::string> tokens2;
      MuonCalib::MdtStringUtils::tokenize(setPointsV1_payload,tokens2,delimiter2);
      //m_ChamberML_V1_chanum.insert(std::make_pair(int(chanNum),float(setPointsV1_name)));
      m_ChamberML_V1_chanum[int(chanNum)]=float(setPointsV1_name);
      
      for (unsigned int i=0; i<tokens2.size(); i++) {
	if(tokens2[i]!="0"){
	  //m_log << MSG::DEBUG << "Sequence for name string load is inside V1\n" <<i << "  "<<tokens2[i]<< endmsg; 
	}
	
      }
     
      int multilayer =atoi(const_cast<char*>(tokens2[3].c_str()));
      std::string chamber_name = tokens2[2];
      Identifier ChamberId= m_condMapTool->ConvertToOffline(chamber_name);
      Identifier MultiLayerId = m_mdtIdHelper->channelID(ChamberId,multilayer,1,1);
      //m_ChamberML_V1.insert(std::make_pair(MultiLayerId,setPointsV1_name));
      m_ChamberML_V1[MultiLayerId]=setPointsV1_name;
      
    }
    chan_index_v1++;
  }


  // check  for chamber standby the correct value of Setpoint V0 vs SetpointV1
  // for chamber StandBy --> V0==V1 to be on

  
  for (unsigned int vect=0; vect<m_cachedDeadMultiLayersId_standby.size(); vect++){
    Identifier MultilayerId_ch = m_cachedDeadMultiLayersId_standby[vect];
    
    if (m_ChamberML_V1.find(MultilayerId_ch)->second == m_ChamberML_V0.find(MultilayerId_ch)->second){
      
      if( m_debug ) m_log<<MSG::DEBUG<<" Chamber has  correct Voltage V1 = "<< m_ChamberML_V1.find(MultilayerId_ch)->second<<" V0=   " << m_ChamberML_V0.find(MultilayerId_ch)->second <<endmsg; 
    }else{
      if( m_debug ) m_log<<MSG::DEBUG<<" Chamber has  wrong correct Voltage V1 = "<< m_ChamberML_V1.find(MultilayerId_ch)->second<<" V0=   " << m_ChamberML_V0.find(MultilayerId_ch)->second <<endmsg;
      if( m_debug ) m_log<<MSG::DEBUG<<" Has to be masked!!! "<<endmsg;
      m_cachedDeadMultiLayersId.push_back(MultilayerId_ch);
    }
    
  }
  
  }
  return StatusCode::SUCCESS;
}
StatusCode MDT_DCSConditionsTool::loadLV(IOVSVC_CALLBACK_ARGS_P(I,keys))
{
  m_log.setLevel(msgLevel());
  m_debug = m_log.level() <= MSG::DEBUG;
  m_verbose = m_log.level() <= MSG::VERBOSE;

  StatusCode sc=StatusCode::SUCCESS;
  m_log << MSG::INFO << "Load LV from DCS DB" << endmsg;
  const CondAttrListCollection * atrc;
  m_log << MSG::INFO << "Try to read from folder <"<<m_lvFolder<<">"<<endmsg;

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
    
    //m_log<<MSG::DEBUG<<"index "<<chan_index<< "  chanNum :" <<atrc->chanNum(chan_index)<< endmsg;
    unsigned int chanNum=atrc->chanNum(chan_index);
    std::string hv_name;
    std::string hv_payload=atrc->chanName(chanNum);
    
    itr=atrc-> chanAttrListPair(chanNum);
    const coral::AttributeList& atr=itr->second;
    //m_log<<MSG::DEBUG<<" CondAttrListCollection ChanNum : "<<chanNum<<" AttributeList  size : " << atr.size() <<endmsg;
    
    if(atr.size()==1){
      hv_name=*(static_cast<const std::string*>((atr["fsm_currentState"]).addressOfData()));
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
	 if( m_verbose ) m_log << MSG::VERBOSE << "NOT ON LV: " << tokens[0]<< " ChamberName : "<<tokens2[2] <<endmsg;
	m_cachedDeadLVStations.push_back(tokens2[2]);
	std::string chamber_name= tokens2[2];
	Identifier ChamberId= m_condMapTool->ConvertToOffline(chamber_name);
	m_cachedDeadLVStationsId.push_back(ChamberId);
	 if( m_verbose ) m_log<<MSG::VERBOSE<<"Chamber off from LV Chamber !=ON "<<tokens2[2] <<endmsg;

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



StatusCode MDT_DCSConditionsTool::loadJTAG(IOVSVC_CALLBACK_ARGS_P(I,keys))
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
      hv_name=*(static_cast<const std::string*>((atr["fsm_currentState"]).addressOfData()));
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

	m_cachedDeadJTAGStatus.push_back(tokens2[2]);
	std::string chamber_name= tokens2[2];
	Identifier ChamberId= m_condMapTool->ConvertToOffline(chamber_name);
	m_cachedDeadJTAGStatusId.push_back(ChamberId);
      }
    }
    
    chan_index++;
  }
    
  
  return StatusCode::SUCCESS;
  
  
  
}





