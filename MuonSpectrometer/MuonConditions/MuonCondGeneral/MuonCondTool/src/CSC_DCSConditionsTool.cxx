/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCondTool/CSC_DCSConditionsTool.h"

#include "SGTools/TransientAddress.h"
#include "CoralBase/Attribute.h"
#include "CoralBase/AttributeListSpecification.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "PathResolver/PathResolver.h"
#include "MuonCondSvc/MdtStringUtils.h"

#include <fstream>
#include <string>
#include <algorithm>
#include <stdio.h>
#include <map>

//**********************************************************
//* Author Monica Verducci monica.verducci@cern.ch
//*
//* Tool to retrieve the CSC DCS Info from COOL DB
//* retrieving of tables from DB
//*********************************************************

CSC_DCSConditionsTool::CSC_DCSConditionsTool (const std::string& type, const std::string& name, const IInterface* parent) :
    AthAlgTool(type, name, parent), 
    m_IOVSvc(nullptr),
    m_chronoSvc(nullptr) {
  declareInterface< ICSC_DCSConditionsTool >(this);
  m_DataLocation="keyCSCDCS";
  declareProperty("HVFolder",     m_hvFolder="/CSC/DCS/LAYERSTATE");
  declareProperty("ChamberFolder",     m_chamberFolder="/CSC/DCS/ENABLEDCHAMBERS");
}

StatusCode CSC_DCSConditionsTool::updateAddress(StoreID::type /*storeID*/,
                                                SG::TransientAddress* /*tad*/,
                                                const EventContext& /*ctx*/) {
  return StatusCode::FAILURE;
}

StatusCode CSC_DCSConditionsTool::initialize() {
  ATH_MSG_INFO("Initializing - folders names are: ChamberDropped "<<m_chamberFolder << " Hv " << m_hvFolder);
  // Get interface to IOVSvc
  bool CREATEIF(true);
  ATH_CHECK(service( "IOVSvc", m_IOVSvc, CREATEIF));
  // initialize the chrono service
  ATH_CHECK(service("ChronoStatSvc",m_chronoSvc));
  ATH_CHECK(m_idHelperSvc.retrieve());
  return StatusCode::SUCCESS;
}

StatusCode CSC_DCSConditionsTool::loadParameters(IOVSVC_CALLBACK_ARGS_P(I,keys)) {
  std::list<std::string>::const_iterator itr;
  for (itr=keys.begin(); itr!=keys.end(); ++itr) {
    ATH_MSG_INFO("LoadParameters "<< *itr << " I="<<I<<" ");
    if (*itr==m_hvFolder) {
      ATH_CHECK(loadHV(I,keys));
    }else if(*itr==m_chamberFolder) {
      ATH_CHECK(loadchamber(I,keys));
    }
  }
	return StatusCode::SUCCESS;
}

StatusCode CSC_DCSConditionsTool::loadHV(IOVSVC_CALLBACK_ARGS_P(I,keys)) {
  ATH_MSG_INFO("Load HV from DCS DB");
  const CondAttrListCollection* atrc=nullptr;
  ATH_MSG_INFO("Try to read from folder <"<<m_hvFolder<<">");
  // Print out callback information
  if (msgLvl(MSG::DEBUG)) ATH_MSG_DEBUG("Level " << I << " Keys: ");
  std::list<std::string>::const_iterator keyIt = keys.begin();
  for (; keyIt != keys.end(); ++ keyIt)  if (msgLvl(MSG::DEBUG)) ATH_MSG_DEBUG(*keyIt << " ");  

  ATH_CHECK(detStore()->retrieve(atrc,m_hvFolder));
  ATH_MSG_INFO("CondAttrListCollection from DB folder have been obtained with size "<< atrc->size());
  
  CondAttrListCollection::const_iterator itr;
  Identifier ChamberId;
  unsigned int layer_index=0;
  unsigned int chan_index=0;
  std::map<Identifier,int>::const_iterator it;
  std::pair<std::map<Identifier,int>::const_iterator,bool> ret;
  
  int hv_state, lv_state, hv_setpoint0, hv_setpoint1;
  for (itr = atrc->begin(); itr != atrc->end(); ++itr){
    
    if (msgLvl(MSG::DEBUG)) ATH_MSG_DEBUG("index "<<chan_index<< "  chanNum :" <<atrc->chanNum(chan_index));
    unsigned int chanNum=atrc->chanNum(chan_index);
    
    
    std::string csc_chan_name=atrc->chanName(chanNum);
    itr=atrc-> chanAttrListPair(chanNum);
    const coral::AttributeList& atr=itr->second;
    if (msgLvl(MSG::DEBUG)) ATH_MSG_DEBUG(" CondAttrListCollection ChanNum : "<<chanNum<<" AttributeList  size : " << atr.size()<< " Channel Name = "<< csc_chan_name);
    
    if(atr.size()){
    
    hv_state=*(static_cast<const int*>((atr["HVState"]).addressOfData()));
    lv_state=*(static_cast<const int*>((atr["LVState"]).addressOfData()));
    hv_setpoint0=*(static_cast<const int*>((atr["HVSetpoint0"]).addressOfData()));
    hv_setpoint1=*(static_cast<const int*>((atr["HVSetpoint1"]).addressOfData()));
   
    
    
    std::string delimiter = "_";
    std::vector<std::string> tokens;
    MuonCalib::MdtStringUtils::tokenize(csc_chan_name,tokens,delimiter);

    for (unsigned int i=0; i<tokens.size(); i++) {
      
      if(tokens[i]!="0"){
	 if (msgLvl(MSG::DEBUG)) ATH_MSG_DEBUG("Sequence for name string load is \n" << tokens[i]); 
      }
      
    }

    

    if((hv_state!=1 or lv_state!=1 or hv_setpoint0 <1000 or hv_setpoint1 <1000) && tokens.size()!=0){
      
      if (msgLvl(MSG::DEBUG)) ATH_MSG_DEBUG("NOT 0 HV : " << hv_state << " ChamberName : "<<tokens[0] << "wirelayer" << tokens[1]);
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
      
      ChamberId = m_idHelperSvc->cscIdHelper().elementID(chamber_name, eta, phi);
      Identifier WireLayerId = m_idHelperSvc->cscIdHelper().channelID(ChamberId, 1, wirelayer,1,1);
      if (msgLvl(MSG::DEBUG)) ATH_MSG_DEBUG("chamber Name = " <<chamber_name);
      std::string WireLayerstring = chamber_name+"_"+eta_side+"_"+sector_side+"_"+layer;  
      m_cachedDeadWireLayers.push_back(WireLayerstring);
       if (msgLvl(MSG::DEBUG)) ATH_MSG_DEBUG("Layers Off = " <<WireLayerstring);
      m_cachedDeadWireLayersId.push_back(WireLayerId);

      if(m_CSC_LayerMap.count(ChamberId))
	{
	  if (msgLvl(MSG::DEBUG)) {
      ATH_MSG_DEBUG("element 'ChamberId' already existed");
      ATH_MSG_DEBUG(" with a value of " << m_CSC_LayerMap[ChamberId]);
    }
	  layer_index++;
	}
      m_CSC_LayerMap[ChamberId]=wirelayer;
      
    }
    
    
    }
    if(layer_index==3) {
      m_cachedDeadStations.push_back(ChamberId);
      
      if (msgLvl(MSG::DEBUG)) ATH_MSG_DEBUG("layers " << layer_index << " ChamberId : "<<ChamberId);	
      layer_index=0;
    }
  
    chan_index++;
    
  }
  return StatusCode::SUCCESS;

}




StatusCode CSC_DCSConditionsTool::loadchamber(IOVSVC_CALLBACK_ARGS_P(I,keys)) {
  ATH_MSG_INFO("Load chamber from DCS DB");
  // Print out callback information
  if (msgLvl(MSG::DEBUG)) ATH_MSG_DEBUG("Level " << I << " Keys: ");
  std::list<std::string>::const_iterator keyIt = keys.begin();
  for (; keyIt != keys.end(); ++ keyIt)  if (msgLvl(MSG::DEBUG)) ATH_MSG_DEBUG(*keyIt << " ");
 
  const CondAttrListCollection* atrc=nullptr;
  ATH_MSG_INFO("Try to read from folder <"<<m_chamberFolder<<">");

  ATH_CHECK(detStore()->retrieve(atrc,m_chamberFolder));
  ATH_MSG_INFO(" CondAttrListCollection from DB folder have been obtained with size "<< atrc->size());
  
  CondAttrListCollection::const_iterator itr;
  
  std::map<Identifier,int>::const_iterator it;

  std::pair<std::map<Identifier,int>::const_iterator,bool> ret;

  std::vector<std::string> chamber_good;

  for (itr = atrc->begin(); itr != atrc->end(); ++itr){
    
    const coral::AttributeList& atr=itr->second;
     if (msgLvl(MSG::DEBUG)) ATH_MSG_DEBUG("AttributeList  size : " << atr.size());
    
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
      if (msgLvl(MSG::DEBUG)) ATH_MSG_DEBUG("found chamber good!\n" <<chamber_v[count]); 
    }else {
      if (msgLvl(MSG::DEBUG)) ATH_MSG_DEBUG(" not found = " << chamber_good[count]);
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

    
    Identifier ChamberId = m_idHelperSvc->cscIdHelper().elementID(chamber_name, eta, phi);
    m_cachedDeadStationsId_chamber.push_back(ChamberId);
    
        
  }
  
  //merge deadStationsId with deadWireStationsId, then sort the vector elements and
  //finally remove duplicates
  if (msgLvl(MSG::VERBOSE)) ATH_MSG_VERBOSE("Now merging the  DeadStationsId with DeadWireStationsId");
  m_cachedDeadStationsId.insert( m_cachedDeadStationsId.end(),
				 m_cachedDeadStationsId_chamber.begin(),m_cachedDeadStationsId_chamber.end());
  std::sort(m_cachedDeadStationsId.begin(),m_cachedDeadStationsId.end(),compareId);  
  std::vector<Identifier>::const_iterator itId;
  itId= std::unique(m_cachedDeadStationsId.begin(),m_cachedDeadStationsId.end());
  m_cachedDeadStationsId.resize(itId -m_cachedDeadStationsId.begin());

  return StatusCode::SUCCESS;
}
