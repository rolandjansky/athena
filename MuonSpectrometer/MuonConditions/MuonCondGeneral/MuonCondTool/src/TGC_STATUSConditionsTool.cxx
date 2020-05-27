/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCondTool/TGC_STATUSConditionsTool.h"

#include "SGTools/TransientAddress.h"
#include "CoralBase/Attribute.h"
#include "CoralBase/AttributeListSpecification.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "PathResolver/PathResolver.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"

#include <fstream>
#include <string>
#include <stdio.h>
#include <map>

TGC_STATUSConditionsTool::TGC_STATUSConditionsTool (const std::string& type, const std::string& name, const IInterface* parent) :
    AthAlgTool(type, name, parent),
    m_IOVSvc(nullptr),
    m_chronoSvc(nullptr) {
  declareInterface< ITGC_STATUSConditionsTool >(this);
  m_tgcDqStatusDataLocation="TgcDqStatusKey";
  declareProperty("TgcDqFolder",m_FolderName="TGC/1/DetectorStatus");
}

StatusCode TGC_STATUSConditionsTool::updateAddress(StoreID::type /*storeID*/,
                                                   SG::TransientAddress* tad,
                                                   const EventContext& /*ctx*/) {
  std::string key  = tad->name();
  return StatusCode::FAILURE;
}

StatusCode TGC_STATUSConditionsTool::initialize() {
  ATH_MSG_INFO("Initializing - folders names are: Chamber Status "<<m_FolderName);
  // Get interface to IOVSvc
  bool CREATEIF(true);
  ATH_CHECK(service( "IOVSvc", m_IOVSvc, CREATEIF));
  // initialize the chrono service
  ATH_CHECK(service("ChronoStatSvc",m_chronoSvc));
  ATH_CHECK(m_idHelperSvc.retrieve());
  return StatusCode::SUCCESS;
}

StatusCode TGC_STATUSConditionsTool::loadParameterStatus(IOVSVC_CALLBACK_ARGS_P(I,keys)) {
  std::list<std::string>::const_iterator itr;
  for (itr=keys.begin(); itr!=keys.end(); ++itr) {
    ATH_MSG_INFO("LoadParameters "<< *itr << " I="<<I<<" ");
    if(*itr==m_FolderName) {
        ATH_CHECK(loadTgcDqStatus(I,keys));
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode TGC_STATUSConditionsTool::loadTgcDqStatus(IOVSVC_CALLBACK_ARGS_P(I,keys)) {
  ATH_MSG_INFO("Load Tgc Status flags  from DB");
  // Print out callback information
  if (msgLvl(MSG::DEBUG)) ATH_MSG_DEBUG("Level " << I << " Keys: ");
  std::list<std::string>::const_iterator keyIt = keys.begin();
  for (; keyIt != keys.end(); ++ keyIt) if (msgLvl(MSG::DEBUG)) ATH_MSG_DEBUG(*keyIt << " ");
  
  const CondAttrListCollection* atrc = nullptr;
  ATH_MSG_INFO("Try to read from folder <"<< m_FolderName <<">");

  ATH_CHECK(detStore()->retrieve(atrc,m_FolderName));
  ATH_MSG_INFO("CondAttrListCollection from DB folder have been obtained with size "<< atrc->size());

  CondAttrListCollection::const_iterator itr;
  for (itr = atrc->begin(); itr != atrc->end(); ++itr) {
     const coral::AttributeList& atr=itr->second;
     int detector_status;
 
     detector_status=*(static_cast<const int*>((atr["detector_status"]).addressOfData()));
     if (msgLvl(MSG::DEBUG)) ATH_MSG_DEBUG("TGC detector status is " << detector_status);

     if (detector_status!=0){
       int channum=itr->first;
       Identifier chamberId = m_idHelperSvc->tgcIdHelper().elementID(Identifier(channum));
       m_cachedDeadStationsId.push_back(chamberId);
     }
  }
  if (msgLvl(MSG::VERBOSE)) ATH_MSG_VERBOSE("Collection CondAttrListCollection CLID " << atrc->clID());
  return StatusCode::SUCCESS; 
}
