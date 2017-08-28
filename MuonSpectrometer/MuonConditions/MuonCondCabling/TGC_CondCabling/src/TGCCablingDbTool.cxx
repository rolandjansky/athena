/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TGC_CondCabling/TGCCablingDbTool.h" 

#include "AthenaPoolUtilities/CondAttrListCollection.h"

#include "PathResolver/PathResolver.h" // needed for readASD2PP_DIFF_12FromText()
#include <fstream> // needed for readASD2PP_DIFF_12FromText()

//**********************************************************
//* Author Monica Verducci monica.verducci@cern.ch
//* Author Susumu Oda Susumu.Oda@cern.ch
//*
//* Tool to retrieve the TGC Cabling Map from COOL DB
//* one callback aganist the  class
//* retrieving of tables from DB
//*********************************************************

TGCCablingDbTool::TGCCablingDbTool(const std::string& type,
				   const std::string& name,
				   const IInterface* parent)
  : AthAlgTool(type, name, parent), m_ASD2PP_DIFF_12(0)
{
  declareInterface<ITGCCablingDbTool>(this);
  
  m_DataLocation="keyTGC";
  
  declareProperty("Folder", m_Folder="/TGC/CABLING/MAP_SCHEMA");

  // ASD2PP_diff_12.db is the text database for the TGCcabling12 package
  declareProperty("filename_ASD2PP_DIFF_12", m_filename="ASD2PP_diff_12.db");
  declareProperty("readASD2PP_DIFF_12FromText", m_readASD2PP_DIFF_12FromText=false);
}


//StatusCode TGCCablingDbTool::updateAddress(SG::TransientAddress* tad) {
StatusCode TGCCablingDbTool::updateAddress(StoreID::type /*storeID*/,
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

StatusCode TGCCablingDbTool::initialize() { 
  ATH_MSG_INFO("initialize");
  
  // Get interface to IOVSvc
  m_IOVSvc = 0;
  StatusCode sc = service("IOVSvc", m_IOVSvc, true);
  if(sc.isFailure() || !m_IOVSvc) {
    ATH_MSG_FATAL("Unable to get the IOVSvc");
    return sc;
  }
  
  // Initialize the chrono service
  sc = service("ChronoStatSvc", m_chronoSvc);
  if(!sc.isSuccess() || !m_chronoSvc) {
    ATH_MSG_FATAL("Could not find the ChronoSvc");
    return StatusCode::FAILURE;
  }
  
  return StatusCode::SUCCESS;
}

StatusCode TGCCablingDbTool::finalize() {
  ATH_MSG_INFO("finalize");

  // Database is deleted if exists
  delete m_ASD2PP_DIFF_12; 
  m_ASD2PP_DIFF_12 = 0;

  return StatusCode::SUCCESS;
}

std::string TGCCablingDbTool::getFolderName() const {
  return m_Folder;
}

std::vector<std::string>* TGCCablingDbTool::giveASD2PP_DIFF_12() {
  ATH_MSG_INFO("giveASD2PP_DIFF_12 (m_ASD2PP_DIFF_12 is " << (!m_ASD2PP_DIFF_12 ? "NULL" : "not NULL") << ")");

  // If no database found, null pointer is returned. 
  if(!m_ASD2PP_DIFF_12) {
    return 0;
  }

  // Copy the database, m_ASD2PP_DIFF_12, as new_ASD2PP_DIFF_12 and return new_ASD2PP_DIFF_12
  std::vector<std::string>* new_ASD2PP_DIFF_12 = new std::vector<std::string>;

  std::vector<std::string>::const_iterator it   = m_ASD2PP_DIFF_12->begin();
  std::vector<std::string>::const_iterator it_e = m_ASD2PP_DIFF_12->end();

  while(it!=it_e) {
    new_ASD2PP_DIFF_12->push_back(*it);
    it++;
  }

  return new_ASD2PP_DIFF_12;
}

StatusCode TGCCablingDbTool::loadParameters(IOVSVC_CALLBACK_ARGS_P(I, keys)) {
  ATH_MSG_INFO("loadParameters from DB");

  std::list<std::string>::const_iterator itr = keys.begin();
  std::list<std::string>::const_iterator itr_e = keys.end();
  
  for(; itr!=itr_e; ++itr) {
    ATH_MSG_INFO("loadParameters " << (*itr) << " I=" << I << " ");
    if((*itr)==m_Folder) {
      loadASD2PP_DIFF_12(I, keys);
    }
  }
  
  return StatusCode::SUCCESS;
}

StatusCode TGCCablingDbTool::loadASD2PP_DIFF_12(IOVSVC_CALLBACK_ARGS_P(/*I*/, /*keys*/)) {
  ATH_MSG_INFO("loadTGCMap from DB");
  
  if(m_readASD2PP_DIFF_12FromText) {
    ATH_MSG_INFO("m_readASD2PP_DIFF_12FromText is true. readASD2PP_DIFF_12FromText() will be executed with m_filename=" << m_filename.c_str());
    StatusCode sc = readASD2PP_DIFF_12FromText();
    if(sc.isFailure()) {
      ATH_MSG_FATAL("could not retreive the CondAttrListCollection from DB folder " << m_Folder);
      return sc;
    }
    return sc; 
  }

  // CondAttrListCollection is retrieved from COOL database 
  const CondAttrListCollection* atrc;  
  StatusCode sc = detStore()->retrieve(atrc, m_Folder);
  if(sc.isFailure()) {
    ATH_MSG_FATAL("could not retreive the CondAttrListCollection from DB folder " << m_Folder);
    return sc;
  } else {
    ATH_MSG_INFO("CondAttrListCollection from DB folder have been obtained with size " << atrc->size());
  }

  // Old database is deleted if exists
  delete m_ASD2PP_DIFF_12;
  // New database is created
  m_ASD2PP_DIFF_12 = new std::vector<std::string>;

  // Database is copied from CondAttrListCollection to m_ASD2PP_DIFF_12 
  CondAttrListCollection::const_iterator itr = atrc->begin();
  CondAttrListCollection::const_iterator itr_e = atrc->end();
  for(; itr!=itr_e; ++itr) {
    const coral::AttributeList& atr=itr->second;
    
    std::string string_ASD2PP_DIFF_12;
    string_ASD2PP_DIFF_12 = *(static_cast<const std::string*>((atr["data"]).addressOfData()));
    ATH_MSG_DEBUG("Sequence load is \n" << string_ASD2PP_DIFF_12);

    // string_ASD2PP_DIFF_12 has multiple newlines and will be separated into multiple lines (strings)
    unsigned int length = string_ASD2PP_DIFF_12.length();
    while(length>0) {
      unsigned int newLine = string_ASD2PP_DIFF_12.find("\n");
      if(length>newLine) {
	m_ASD2PP_DIFF_12->push_back(string_ASD2PP_DIFF_12.substr(0, newLine));
	string_ASD2PP_DIFF_12.erase(0, newLine+1); 
      }
      length = string_ASD2PP_DIFF_12.length();
    };
  }
  
  return StatusCode::SUCCESS; 
}

StatusCode TGCCablingDbTool::readASD2PP_DIFF_12FromText() {
  ATH_MSG_INFO("readTGCMap from text");

  // PathResolver finds the full path of the file (default file name is ASD2PP_diff_12.db) 
  std::string location = PathResolver::find_file(m_filename.c_str(), "DATAPATH");
  if(location=="") {
    ATH_MSG_FATAL("Could not find " << m_filename.c_str());
    return StatusCode::FAILURE; 
  }
  
  // ASD2PP_diff_12.db is opened as inASDToPP
  std::ifstream inASDToPP;
  inASDToPP.open(location.c_str());
  if(inASDToPP.bad()) { 
    ATH_MSG_FATAL("Could not open file " << location.c_str());
    return StatusCode::FAILURE; 
  } 
  
  ATH_MSG_INFO("readTGCMap found file " << location.c_str());
  
  // Old database is deleted if exists
  delete m_ASD2PP_DIFF_12;
  // New database is created
  m_ASD2PP_DIFF_12 = new std::vector<std::string>;

  unsigned int nLines = 0;
  std::string buf; 
  // Copy database into m_ASD2PP_DIFF_12
  while(getline(inASDToPP, buf)){ 
    m_ASD2PP_DIFF_12->push_back(buf);
    ATH_MSG_INFO(m_filename.c_str() << " L" << ++nLines << ": " << buf.c_str());
  }

  inASDToPP.close(); 
  return StatusCode::SUCCESS;
}
