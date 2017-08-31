/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TGC_CondCabling/TGCTriggerDbTool.h" 
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "CoolKernel/IObject.h"

TGCTriggerDbTool::TGCTriggerDbTool(const std::string& type,
				   const std::string& name,
				   const IInterface* parent)
  : AthAlgTool(type, name, parent)
{
  declareInterface<ITGCTriggerDbTool>(this);
  
  m_DataLocation = "keyTGC";
  
  declareProperty("BwFolder",   m_bwFolder="/TGC/TRIGGER/CW_BW");
  declareProperty("EifiFolder", m_eifiFolder="/TGC/TRIGGER/CW_EIFI");
  declareProperty("TileFolder", m_tileFolder="/TGC/TRIGGER/CW_TILE");
}


//StatusCode TGCCablingDbTool::updateAddress(SG::TransientAddress* tad) {
StatusCode TGCTriggerDbTool::updateAddress(StoreID::type /*storeID*/,
                                           SG::TransientAddress* tad,
                                           const EventContext& /*ctx*/) {
  CLID clid = tad->clID();
  std::string key = tad->name();
  // Need to add the CLID comparison 
  if(/* ==clid && */m_DataLocation==key) {
    ATH_MSG_DEBUG("updateAddress OK, clid = " << clid << " key = " << key);
    return StatusCode::SUCCESS;
  }
  
  ATH_MSG_FATAL("updateAddress failed, clid = " << clid << " key = " << key);
  return StatusCode::FAILURE;
}


StatusCode TGCTriggerDbTool::initialize() { 
  ATH_MSG_INFO("initialize");

  m_folders[CW_BW]   = m_bwFolder;
  m_folders[CW_EIFI] = m_eifiFolder;
  m_folders[CW_TILE] = m_tileFolder;

  for (int ii = 0; ii < CW_NUM; ++ii) {

    if ( !detStore()->contains<CondAttrListCollection>(m_folders[ii])) {
      ATH_MSG_FATAL("Could not found " << m_folders[ii] << " in DetectorStore.");
      return StatusCode::FAILURE;
    }
  }
  
  return StatusCode::SUCCESS;
}


StatusCode TGCTriggerDbTool::finalize() {
  ATH_MSG_INFO("finalize");

  return StatusCode::SUCCESS;
}


std::string TGCTriggerDbTool::getFolderName(int cwtype) const {
  return m_folders[cwtype];
}


std::string TGCTriggerDbTool::getData(int cwtype, std::string file) const {
  auto itr = m_datamap[cwtype].find(file);
  if (itr == m_datamap[cwtype].end()) {
    ATH_MSG_WARNING("File " << file << " is not found in m_datamap");
    return "";
  }
 
  return itr->second;
}


std::string TGCTriggerDbTool::getData(int cwtype, int channel) const {
  return m_data[cwtype][channel];
}


std::string TGCTriggerDbTool::getFile(int cwtype, int channel) const {
  return m_file[cwtype][channel];
}

std::string TGCTriggerDbTool::getVersion(int cwtype, int channel) const {
  return m_version[cwtype][channel];
}

std::string TGCTriggerDbTool::getType(int cwtype, int channel) const {
  return m_type[cwtype][channel];
}


bool TGCTriggerDbTool::isActive(int cwtype, int channel) const {
  return m_active[cwtype][channel];
}


StatusCode TGCTriggerDbTool::loadParameters(IOVSVC_CALLBACK_ARGS_P(I, keys)) {
  ATH_MSG_INFO("loadParameters from DB");

  for (int ii = 0; ii < CW_NUM; ++ii) {

    m_datamap[ii].clear();
    m_data[ii].clear();
    m_file[ii].clear();
    m_type[ii].clear();
    m_active[ii].clear();
    
    bool found = false;
    for(std::string key: keys) {
      ATH_MSG_INFO("loadParameters " << key << " I=" << I << " ");
      if (key == m_folders[ii]) {
        found = true;
        break;
      }
    }
    if (!found) continue;

    const CondAttrListCollection* calc;
    StatusCode sc = detStore()->retrieve(calc, m_folders[ii]);
    if(sc.isFailure()) {
      ATH_MSG_FATAL("could not retreive the CondAttrListCollection from DB folder " << m_folders[ii]);
     return sc;
 
    } else {
      ATH_MSG_INFO("CondAttrListCollection from DB folder have been obtained with size " << calc->size());
    }
  
    CondAttrListCollection::const_iterator itr = calc->begin();
    CondAttrListCollection::const_iterator itr_e = calc->end();
 
    for(; itr!=itr_e; ++itr) {
      const unsigned int channel = (*itr).first;
      const coral::AttributeList& atr = (*itr).second;
 
      ATH_MSG_DEBUG("channel: " << channel);
 
      std::string file    = atr["file"].data<cool::String255>();
      std::string data    = atr["data"].data<cool::String16M>();
      std::string version = atr["version"].data<cool::String255>();
      std::string type    = atr["type"].data<cool::String255>();
      bool active         = atr["active"].data<cool::Bool>();

      m_datamap[ii][file] = data;
      m_data[ii].push_back(data);
      m_file[ii].push_back(file);
      m_version[ii].push_back(version);
      m_type[ii].push_back(type);
      m_active[ii].push_back(active);

      ATH_MSG_INFO("active: " << active << " file: " << file);
      ATH_MSG_DEBUG("data: " << data);
    } 
  }

  return StatusCode::SUCCESS;
}

