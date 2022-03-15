/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/MsgStream.h"

#include "SGTools/TransientAddress.h"
#include "CoralBase/Attribute.h"
#include "CoralBase/AttributeListSpecification.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"

#include "Identifier/IdentifierHash.h"
#include "Identifier/Identifier.h"
#include "MuonIdHelpers/MdtIdHelper.h"

#include "PathResolver/PathResolver.h"
#include <algorithm>
#include <cstdio>
#include <fstream>
#include <map>
#include <string>
//#include "MuonCondInterface/IMDT_DQConditionsTool.h" 
#include "MDT_DQConditionsTool.h"
#include "MDT_MapConversion.h"

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


MDT_DQConditionsTool::MDT_DQConditionsTool (const std::string& type,
				    const std::string& name,
				    const IInterface* parent)
	  : AthAlgTool(type, name, parent),
            m_IOVSvc(nullptr),
            m_mdtIdHelper(nullptr),
            m_chronoSvc(nullptr),
	    m_condMapTool("MDT_MapConversion"), 
	    m_log( msgSvc(), name ),
	    m_debug(false),
	    m_verbose(false)   
{
  
  declareInterface< IMDT_DQConditionsTool >(this);
  
  
  //m_DataLocation="keyMDTDQ";
  
  declareProperty("DeadFolder",     m_deadFolder="/MDT/DEAD/DROPPEDCH");
  declareProperty("NoisyFolder",     m_noisyFolder="/MDT/NOISY/PSLVCHSTATE");
  declareProperty("Simulation_Setup",m_simulation_Setup=false);
  declareProperty("MDT_MapConversion", m_condMapTool);
  
  // MDTChamDrop.str("EMPTY");
  }


//StatusCode MDT_DQConditionsTool::updateAddress(SG::TransientAddress* /*tad*/)
StatusCode MDT_DQConditionsTool::updateAddress(StoreID::type /*storeID*/,
                                               SG::TransientAddress* /*tad*/,
                                               const EventContext& /*ctx*/)
{
//   MsgStream m_log(msgSvc(), name());
//   CLID clid        = tad->clID();
//   std::string key  = tad->name();
 
  return StatusCode::FAILURE;
}
	


StatusCode MDT_DQConditionsTool::initialize()
{

  m_log.setLevel(msgLevel());
  m_debug = m_log.level() <= MSG::DEBUG;
  m_verbose = m_log.level() <= MSG::VERBOSE;

  m_log << MSG::INFO << "Initializing - folders names are: ChamberDropped "<<m_deadFolder <<" / LV "<<m_noisyFolder<< endmsg;
   
  StatusCode sc = detStore()->retrieve(m_mdtIdHelper, "MDTIDHELPER" );
  if (sc.isFailure())
    {
      m_log << MSG::FATAL << " Cannot retrieve MdtIdHelper " << endmsg;
      return sc;
    }
  
    
  // Get interface to IOVSvc
  m_IOVSvc = nullptr;
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


StatusCode MDT_DQConditionsTool::loadParameters(IOVSVC_CALLBACK_ARGS_P(I,keys))
{

  m_log.setLevel(msgLevel());
  m_debug = m_log.level() <= MSG::DEBUG;
  m_verbose = m_log.level() <= MSG::VERBOSE;

  // bool isNoisyfolder=false, isDeadfolder=false;
 
  std::list<std::string>::const_iterator itr;
  for (itr=keys.begin(); itr!=keys.end(); ++itr) {
    m_log << MSG::INFO <<"LoadParameters "<< *itr << " I="<<I<<" "<<endmsg;
    if(*itr==m_deadFolder) {
      StatusCode sc = loadDeadChamber(I,keys);
      if (sc.isFailure())
	{
	  return sc;
	}
    }
    if (*itr==m_noisyFolder) {
      StatusCode sc = loadNoisyChamber(I,keys);
      if (sc.isFailure())
	{
	  return sc;
	}
    }
   
  }
    
  return StatusCode::SUCCESS;
}


StatusCode MDT_DQConditionsTool::loadDeadChamber(IOVSVC_CALLBACK_ARGS_P(I,keys))
{

  m_log.setLevel(msgLevel());
  m_debug = m_log.level() <= MSG::DEBUG;
  m_verbose = m_log.level() <= MSG::VERBOSE; 
  StatusCode sc=StatusCode::SUCCESS;
  m_log << MSG::INFO << "Load Dropped Chamber from DQ DB" << endmsg;
  
  // Print out callback information
  if( m_debug ) m_log << MSG::DEBUG << "Level " << I << " Keys: ";
  std::list<std::string>::const_iterator keyIt = keys.begin();
  for (; keyIt != keys.end(); ++ keyIt) if( m_debug ) m_log << MSG::DEBUG << *keyIt << " ";
  if( m_debug ) m_log << MSG::DEBUG << endmsg;
 
  
	
  const CondAttrListCollection * atrc;
  m_log << MSG::INFO << "Try to read from folder <"<<m_deadFolder<<">"<<endmsg;
  
  sc=detStore()->retrieve(atrc,m_deadFolder);
  if(sc.isFailure())  {
    m_log << MSG::ERROR
	<< "could not retreive the CondAttrListCollection from DB folder " 
	<< m_deadFolder << endmsg;
    return sc;
	  }
  
  else
    if( m_debug ) m_log<<MSG::DEBUG<<" CondAttrListCollection from DB folder have been obtained with size "<< atrc->size() <<endmsg;
  
   using namespace MuonCalib;
  CondAttrListCollection::const_iterator itr;
 
  for (itr = atrc->begin(); itr != atrc->end(); ++itr) {
  
    const coral::AttributeList& atr=itr->second;
    
    std::string chamber_name, list_mlayer, list_layer, list_tube;
    
    chamber_name=*(static_cast<const std::string*>((atr["Chambers_Name"]).addressOfData()));
    list_mlayer=*(static_cast<const std::string*>((atr["Dead_multilayer"]).addressOfData()));
    list_layer=*(static_cast<const std::string*>((atr["Dead_layer"]).addressOfData()));
    list_tube=*(static_cast<const std::string*>((atr["Dead_tube"]).addressOfData()));
    
    char delimiter = ' ';
    Identifier ChamberId= m_condMapTool->ConvertToOffline(chamber_name);
    const auto tokens = MuonCalib::MdtStringUtils::tokenize(list_tube,delimiter);
    const auto tokens_mlayer = MuonCalib::MdtStringUtils::tokenize(list_mlayer,delimiter);
    const auto tokens_layer = MuonCalib::MdtStringUtils::tokenize(list_layer,delimiter);
    
    for (unsigned int i=0; i<tokens.size(); i++) {
      if(tokens[i]!="0"){
        
	int ml = MdtStringUtils::atoi(tokens[i][0]);
	int layer= MdtStringUtils::atoi(tokens[i].substr(1,2));
	int tube= MdtStringUtils::atoi(tokens[i].substr(2));
	
	Identifier ChannelId =m_mdtIdHelper->channelID(ChamberId,ml,layer,tube); 
	m_cachedDeadTubesId.push_back(ChannelId);

      }
    }

    for (unsigned int i=0; i<tokens_mlayer.size(); i++) {
      if(tokens_mlayer[i]!="0"){
        
	int ml = MdtStringUtils::atoi(tokens_mlayer[i].substr(0));
	
	Identifier ChannelId =m_mdtIdHelper->channelID(ChamberId,ml,1,1); 
	m_cachedDeadMultiLayersId.push_back(ChannelId);

      }
    }

    for (unsigned int i=0; i<tokens_layer.size(); i++) {
      if(tokens_layer[i]!="0"){
        
	int ml = MdtStringUtils::atoi(tokens_layer[i].substr(0,1));
	int layer = MdtStringUtils::atoi(tokens_layer[i].substr(1));
		
	Identifier ChannelId =m_mdtIdHelper->channelID(ChamberId,ml,layer,1); 
	m_cachedDeadLayersId.push_back(ChannelId);

      }
    }
     
  }
  return StatusCode::SUCCESS;
}




	
StatusCode MDT_DQConditionsTool::loadNoisyChamber(IOVSVC_CALLBACK_ARGS_P(I,keys))
{
  using namespace MuonCalib;
  m_log.setLevel(msgLevel());
  m_debug = m_log.level() <= MSG::DEBUG;
  m_verbose = m_log.level() <= MSG::VERBOSE; 
  StatusCode sc=StatusCode::SUCCESS;
  m_log << MSG::INFO << "Load Noisy Chamber from DQ DB" << endmsg;
  
  // Print out callback information
  if( m_debug ) m_log << MSG::DEBUG << "Level " << I << " Keys: ";
  std::list<std::string>::const_iterator keyIt = keys.begin();
  for (; keyIt != keys.end(); ++ keyIt) if( m_debug ) m_log << MSG::DEBUG << *keyIt << " ";
  if( m_debug ) m_log << MSG::DEBUG << endmsg;
 
  
	
  const CondAttrListCollection * atrc;
  m_log << MSG::INFO << "Try to read from folder <"<<m_noisyFolder<<">"<<endmsg;
  
  sc=detStore()->retrieve(atrc,m_noisyFolder);
  if(sc.isFailure())  {
    m_log << MSG::ERROR
	<< "could not retreive the CondAttrListCollection from DB folder " 
	<< m_noisyFolder << endmsg;
    return sc;
	  }
  
  else
    if( m_debug ) m_log<<MSG::DEBUG<<" CondAttrListCollection from DB folder have been obtained with size "<< atrc->size() <<endmsg;
  
 
  CondAttrListCollection::const_iterator itr;
 
  for (itr = atrc->begin(); itr != atrc->end(); ++itr) {
  
    const coral::AttributeList& atr=itr->second;
    
    std::string chamber_name, list_mlayer, list_layer, list_tube;
    
    chamber_name=*(static_cast<const std::string*>((atr["Chambers_Name"]).addressOfData()));
    list_mlayer=*(static_cast<const std::string*>((atr["Noisy_multilayer"]).addressOfData()));
    list_layer=*(static_cast<const std::string*>((atr["Noisy_layer"]).addressOfData()));
    list_tube=*(static_cast<const std::string*>((atr["Noisy_tube"]).addressOfData()));
    
    char delimiter = ' ';
    Identifier ChamberId= m_condMapTool->ConvertToOffline(chamber_name);
    auto tokens = MuonCalib::MdtStringUtils::tokenize(list_tube,delimiter);
    auto tokens_mlayer = MuonCalib::MdtStringUtils::tokenize(list_mlayer,delimiter);
    auto tokens_layer = MuonCalib::MdtStringUtils::tokenize(list_layer,delimiter);
    
    for (unsigned int i=0; i<tokens.size(); i++) {
      if(tokens[i]!="0"){
        
	int ml = MdtStringUtils::atoi(tokens[i][0]);
	int layer= MdtStringUtils::atoi(tokens[i].substr(1,2));
	int tube= MdtStringUtils::atoi(tokens[i].substr(2));
	
	Identifier ChannelId =m_mdtIdHelper->channelID(ChamberId,ml,layer,tube); 
	m_cachedNoisyTubesId.push_back(ChannelId);

      }
    }

    for (unsigned int i=0; i<tokens_mlayer.size(); i++) {
      if(tokens_mlayer[i]!="0"){
        
	int ml = MdtStringUtils::atoi(tokens_mlayer[i].substr(0));
	
	Identifier ChannelId =m_mdtIdHelper->channelID(ChamberId,ml,1,1); 
	m_cachedNoisyMultiLayersId.push_back(ChannelId);

      }
    }

    for (unsigned int i=0; i<tokens_layer.size(); i++) {
      if(tokens_layer[i]!="0"){
        
	int ml = MdtStringUtils::atoi(tokens_layer[i].substr(0,1));
	int layer = MdtStringUtils::atoi(tokens_layer[i].substr(1));
		
	Identifier ChannelId =m_mdtIdHelper->channelID(ChamberId,ml,layer,1); 
	m_cachedNoisyLayersId.push_back(ChannelId);

      }
    }
     
  }
  
  return StatusCode::SUCCESS;
}



