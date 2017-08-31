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
#include "MuonIdHelpers/CscIdHelper.h"

#include "PathResolver/PathResolver.h"
#include <fstream>
#include <string>
#include <algorithm>
#include <stdio.h>
#include <map>

#include "MuonCondTool/CSC_DCSConditionsTool.h"
#include "MuonCondSvc/MdtStringUtils.h"

//**********************************************************
//* Author Monica Verducci monica.verducci@cern.ch
//*
//* Tool to retrieve the CSC DCS Info from COOL DB
//* retrieving of tables from DB
//*********************************************************


CSC_DCSConditionsTool::CSC_DCSConditionsTool (const std::string& type,
				    const std::string& name,
				    const IInterface* parent)
	  : AthAlgTool(type, name, parent), 
	    m_detStore(0),
	    m_IOVSvc(0),
	    m_cscIdHelper(0),
	    m_chronoSvc(0),
	    m_log( msgSvc(), name ),
	    m_debug(true),
	    m_verbose(false) 
{
  
  declareInterface< ICSC_DCSConditionsTool >(this);
  
  
  m_DataLocation="keyCSCDCS";
  declareProperty("HVFolder",     m_hvFolder="/CSC/DCS/LAYERSTATE");
  declareProperty("ChamberFolder",     m_chamberFolder="/CSC/DCS/ENABLEDCHAMBERS");
}


//StatusCode CSC_DCSConditionsTool::updateAddress(SG::TransientAddress* /*tad*/)
StatusCode CSC_DCSConditionsTool::updateAddress(StoreID::type /*storeID*/,
                                                SG::TransientAddress* /*tad*/,
                                                const EventContext& /*ctx*/)
{
  return StatusCode::FAILURE;
}
	


StatusCode CSC_DCSConditionsTool::initialize()
{

  m_log.setLevel(msgLevel());
  m_debug = m_log.level() <= MSG::DEBUG;
  m_verbose = m_log.level() <= MSG::VERBOSE;
  
  m_log << MSG::INFO << "Initializing - folders names are: ChamberDropped "<<m_chamberFolder << " Hv " << m_hvFolder<< endmsg;
   
  StatusCode sc = serviceLocator()->service("DetectorStore", m_detStore);
  if ( sc.isSuccess() ) {
     if( m_debug ) m_log << MSG::DEBUG << "Retrieved DetectorStore" << endmsg;
  }else{
    m_log << MSG::ERROR << "Failed to retrieve DetectorStore" << endmsg;
    return sc;
  }
  


  sc = m_detStore->retrieve(m_cscIdHelper, "CSCIDHELPER" );
  if (sc.isFailure())
    {
      m_log << MSG::FATAL << " Cannot retrieve CscIdHelper " << endmsg;
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


StatusCode CSC_DCSConditionsTool::loadParameters(IOVSVC_CALLBACK_ARGS_P(I,keys))
{
 
  m_log.setLevel(msgLevel());
  m_debug = m_log.level() <= MSG::DEBUG;
  m_verbose = m_log.level() <= MSG::VERBOSE;	 
 
  std::list<std::string>::const_iterator itr;
  for (itr=keys.begin(); itr!=keys.end(); ++itr) {
    m_log << MSG::INFO <<"LoadParameters "<< *itr << " I="<<I<<" "<<endmsg;
    if (*itr==m_hvFolder) {
      StatusCode sc = loadHV(I,keys);
      if (sc.isFailure())
	{
	  return sc;
	}
    }else if(*itr==m_chamberFolder) {
      StatusCode sc = loadchamber(I,keys);
      if (sc.isFailure())
	{
	  return sc;
	}
    }
    
  }
  
	  return StatusCode::SUCCESS;
}



StatusCode CSC_DCSConditionsTool::loadHV(IOVSVC_CALLBACK_ARGS_P(I,keys))
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
  for (; keyIt != keys.end(); ++ keyIt)  if( m_debug ) m_log << MSG::DEBUG << *keyIt << " ";
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
  
  CondAttrListCollection::const_iterator itr;
  Identifier ChamberId;
  unsigned int layer_index=0; 
  unsigned int chan_index=0; 
  //Identifier name_control;
  
  std::map<Identifier,int>::const_iterator it;
  std::pair<std::map<Identifier,int>::const_iterator,bool> ret;
  
  int hv_state, lv_state, hv_setpoint0, hv_setpoint1;
  for (itr = atrc->begin(); itr != atrc->end(); ++itr){
    
    if( m_debug ) m_log<<MSG::DEBUG<<"index "<<chan_index<< "  chanNum :" <<atrc->chanNum(chan_index)<< endmsg;
    unsigned int chanNum=atrc->chanNum(chan_index);
    
    
    std::string csc_chan_name=atrc->chanName(chanNum);
    itr=atrc-> chanAttrListPair(chanNum);
    const coral::AttributeList& atr=itr->second;
    if( m_debug ) m_log<<MSG::DEBUG<<" CondAttrListCollection ChanNum : "<<chanNum<<" AttributeList  size : " << atr.size()<< " Channel Name = "<< csc_chan_name <<endmsg;
    
    //if(atr.size()==1){
    if(atr.size()){
    
    hv_state=*(static_cast<const int*>((atr["HVState"]).addressOfData()));
    lv_state=*(static_cast<const int*>((atr["LVState"]).addressOfData()));
    hv_setpoint0=*(static_cast<const int*>((atr["HVSetpoint0"]).addressOfData()));
    hv_setpoint1=*(static_cast<const int*>((atr["HVSetpoint1"]).addressOfData()));
   
    
    
    std::string delimiter = "_";
    std::vector<std::string> tokens;
    MuonCalib::MdtStringUtils::tokenize(csc_chan_name,tokens,delimiter);
    //  if( m_debug ) m_log<<MSG::DEBUG<<" CondAttrListCollection ChanNum : "<<chanNum<<" ChanName : " << atrc->chanName(chanNum) << " tokens[0] "<<tokens[0] <<endmsg;

    for (unsigned int i=0; i<tokens.size(); i++) {
      
      if(tokens[i]!="0"){
	 if( m_debug ) m_log << MSG::DEBUG << "Sequence for name string load is \n" << tokens[i]<< endmsg; 
      }
      
    }

    

    if((hv_state!=1 or lv_state!=1 or hv_setpoint0 <1000 or hv_setpoint1 <1000) && tokens.size()!=0){
      
      if( m_debug ) m_log << MSG::DEBUG << "NOT 0 HV : " << hv_state << " ChamberName : "<<tokens[0] << "wirelayer" << tokens[1]<<endmsg;	
      int eta=0; int phi=0;
      //std::string chamber_name;
      std::string layer = tokens[1];
      std::string number_layer =tokens[1].substr(1,2);   
      int wirelayer = atoi(const_cast<char*>(number_layer.c_str()));
      //std::string chamber_name = tokens[0];
      std::string chamber_name;
      std::string eta_side = tokens[0].substr(0,1);
      if (eta_side == "A") eta = +1;
      if (eta_side == "C") eta = -1;
      std::string size_side = tokens[0].substr(1,1);
      
      if (size_side == "L") chamber_name = "CSL";
      if (size_side == "S") chamber_name = "CSS";
      std::string sector_side = tokens[0].substr(2,4);
      if (sector_side == "01" || sector_side == "02") phi=1;
      if (sector_side == "03" || sector_side == "04") phi=2;
      if (sector_side == "05" || sector_side == "06") phi=3;
      if (sector_side == "07" || sector_side == "08") phi=4;
      if (sector_side == "09" || sector_side == "10") phi=5;
      if (sector_side == "11" || sector_side == "12") phi=6;
      if (sector_side == "13" || sector_side == "14") phi=7;
      if (sector_side == "15" || sector_side == "16") phi=8;
      
      ChamberId = m_cscIdHelper->elementID(chamber_name, eta, phi);
      Identifier WireLayerId = m_cscIdHelper->channelID(ChamberId, 1, wirelayer,1,1);
      if( m_debug ) m_log<<MSG::DEBUG<< "chamber Name = " <<chamber_name<< endmsg;
      std::string WireLayerstring = chamber_name+"_"+eta_side+"_"+sector_side+"_"+layer;  
      m_cachedDeadWireLayers.push_back(WireLayerstring);
       if( m_debug ) m_log<<MSG::DEBUG<< "Layers Off = " <<WireLayerstring<< endmsg;
      m_cachedDeadWireLayersId.push_back(WireLayerId);
      
      //m_CSC_LayerMap.insert(std::make_pair(ChamberId,wirelayer));
      //ret= m_CSC_LayerMap.insert(std::make_pair(ChamberId,wirelayer));
      //if (ret.second==false)
      if(m_CSC_LayerMap.count(ChamberId))
	{
	  if( m_debug ) m_log<<MSG::DEBUG<< "element 'ChamberId' already existed";
	  //if( m_debug ) m_log<<MSG::DEBUG<< " with a value of " << ret.first->second << endmsg;
	  if( m_debug ) m_log<<MSG::DEBUG<< " with a value of " << m_CSC_LayerMap[ChamberId] << endmsg;
	  layer_index++;
	}
      m_CSC_LayerMap[ChamberId]=wirelayer;
      
    }
    
    
    }
    if(layer_index==3) {
      m_cachedDeadStations.push_back(ChamberId);
      
      if( m_debug ) m_log << MSG::DEBUG << "layers " << layer_index << " ChamberId : "<<ChamberId <<endmsg;	
      layer_index=0;
    }
  
    chan_index++;
    
  }
  

  
  
  return StatusCode::SUCCESS;
  
  }




StatusCode CSC_DCSConditionsTool::loadchamber(IOVSVC_CALLBACK_ARGS_P(I,keys))
{
 

  m_log.setLevel(msgLevel());
  m_debug = m_log.level() <= MSG::DEBUG;
  m_verbose = m_log.level() <= MSG::VERBOSE;

  StatusCode sc=StatusCode::SUCCESS;
  m_log << MSG::INFO << "Load chamber from DCS DB" << endmsg;

  // Print out callback information
   if( m_debug ) m_log << MSG::DEBUG << "Level " << I << " Keys: ";
  std::list<std::string>::const_iterator keyIt = keys.begin();
  for (; keyIt != keys.end(); ++ keyIt)  if( m_debug ) m_log << MSG::DEBUG << *keyIt << " ";
  if( m_debug ) m_log << MSG::DEBUG << endmsg;
  
 
  const CondAttrListCollection * atrc;
  m_log << MSG::INFO << "Try to read from folder <"<<m_chamberFolder<<">"<<endmsg;

  sc=m_detStore->retrieve(atrc,m_chamberFolder);
  
  if(sc.isFailure())  {
    m_log << MSG::ERROR
	<< "could not retreive the CondAttrListCollection from DB folder " 
	<<  m_chamberFolder << endmsg;
    return sc;
  }
  
  else
    m_log<<MSG::INFO<<" CondAttrListCollection from DB folder have been obtained with size "<< atrc->size() <<endmsg;
  
  CondAttrListCollection::const_iterator itr;
  Identifier ChamberId;
  
  std::map<Identifier,int>::const_iterator it;

  std::pair<std::map<Identifier,int>::const_iterator,bool> ret;

  std::vector<std::string> chamber_good;

  for (itr = atrc->begin(); itr != atrc->end(); ++itr){
    
    const coral::AttributeList& atr=itr->second;
     if( m_debug ) m_log<<MSG::DEBUG<<"AttributeList  size : " << atr.size() <<endmsg;
    
    std::string chamber_enabled=*(static_cast<const std::string*>((atr["enabledChambers"]).addressOfData()));
    
    std::string delimiter = " ";
    std::vector<std::string> tokens;
    MuonCalib::MdtStringUtils::tokenize(chamber_enabled,tokens,delimiter);
    
    for (unsigned int i=0; i<tokens.size(); i++) {
      chamber_good.push_back(tokens[i]);
    }
    
  }
  
  std::string chamber_all[] = {"A01","A02","A03","A04","A05","A06","A07","A08","A09","A10","A11","A12","A13","A14","A15","A16","C01","C02","C03","C04","C05","C06","C07","C08","C09","C10","C11","C12","C13","C14","C15","C16"};
  std::vector<std::string> chamber_v(chamber_all,chamber_all+32);      
  sort(chamber_v.begin(), chamber_v.end());
  
  for(unsigned int count=0; count<chamber_good.size(); count++){
   
    if (binary_search (chamber_v.begin(), chamber_v.end(),chamber_good[count])){
      if( m_debug ) m_log<<MSG::DEBUG<< "found chamber good!\n" 
		       <<chamber_v[count] <<endmsg; 
    }else {
      if( m_debug ) m_log<<MSG::DEBUG << " not found = " << chamber_good[count] << endmsg;
      m_cachedDeadStationsStr.push_back(chamber_good[count]);
    }
    
  }

  for(unsigned int tot=0; tot<m_cachedDeadStationsStr.size(); tot++){
    int eta=0; int phi=0;

    std::string chamber_name;
    std::string eta_side = m_cachedDeadStationsStr[tot].substr(0,1);
    if (eta_side == "A") eta = +1;
    if (eta_side == "C") eta = -1;
    std::string sector_side = m_cachedDeadStationsStr[tot].substr(2,4);
    if (sector_side == "01" || sector_side == "02") phi=1;
    if (sector_side == "03" || sector_side == "04") phi=2;
    if (sector_side == "05" || sector_side == "06") phi=3;
    if (sector_side == "07" || sector_side == "08") phi=4;
    if (sector_side == "09" || sector_side == "10") phi=5;
    if (sector_side == "11" || sector_side == "12") phi=6;
    if (sector_side == "13" || sector_side == "14") phi=7;
    if (sector_side == "15" || sector_side == "16") phi=8;

    if (sector_side == "01" || sector_side == "03" ||sector_side == "05" ||sector_side == "07" || sector_side == "09" || sector_side == "11" || sector_side == "13" || sector_side == "15" ) chamber_name = "CSL";
    if (sector_side == "02" || sector_side == "04" || sector_side == "06"|| sector_side == "08" || sector_side == "10"|| sector_side == "12"|| sector_side == "14"|| sector_side == "16") chamber_name = "CSS";

    
    Identifier ChamberId = m_cscIdHelper->elementID(chamber_name, eta, phi);
    m_cachedDeadStationsId_chamber.push_back(ChamberId);
    
        
  }
  
  //merge deadStationsId with deadWireStationsId, then sort the vector elements and
  //finally remove duplicates

  if( m_verbose ) m_log << MSG::VERBOSE << "Now merging the  DeadStationsId with DeadWireStationsId" <<  endmsg;
  m_cachedDeadStationsId.insert( m_cachedDeadStationsId.end(),
				 m_cachedDeadStationsId_chamber.begin(),m_cachedDeadStationsId_chamber.end());
  std::sort(m_cachedDeadStationsId.begin(),m_cachedDeadStationsId.end(),compareId);  
  std::vector<Identifier>::const_iterator itId;
  itId= std::unique(m_cachedDeadStationsId.begin(),m_cachedDeadStationsId.end());
  m_cachedDeadStationsId.resize(itId -m_cachedDeadStationsId.begin());
  
      

  
   
  return StatusCode::SUCCESS;
 

 
}
