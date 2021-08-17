/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <limits.h>
#include "LumiCalc/CoolQuery.h"

//===========================================================================
CoolQuery::CoolQuery(const std::string& database, const std::string& triggerchain): 
  m_repsort(0),
  m_database(database), 
  m_triggerchain(triggerchain),
  m_VKstart(0),
  m_VKstop(0),
  m_logger( "CoolQuery" ),
  m_valid(false)
{

}

//===========================================================================
CoolQuery::~CoolQuery() {
  if ( m_sourceDbPtr.use_count()>0 && m_sourceDbPtr->isOpen() ) {
     m_logger << Root::kINFO << "Closing database '" << m_sourceDbPtr->databaseName() << Root::GEndl;
     m_sourceDbPtr->closeDatabase();
  }
}

//===========================================================================
bool CoolQuery::openDbConn() {

  m_logger << Root::kINFO << "Trying to connect to database " << m_database << "..." << Root::GEndl;

  //  CoraCoolDatabaseSvc& corasvc = CoraCoolDatabaseSvcFactory::databaseService();

  cool::IDatabaseSvc& databasesvc = cool::DatabaseSvcFactory::databaseService();
  //std::cout << "Done the CoraCool initialisation" << std::endl;
  //std::cout << "Opening CORAL database" << std::endl;
  try {
    m_repsort=new ReplicaSorter();
    coral::IConnectionServiceConfiguration& csconfig=m_coralsvc.configuration();
    csconfig.setReplicaSortingAlgorithm(*m_repsort);

    m_sourceDbPtr = databasesvc.openDatabase(m_database,true);// true --> readonly
    //    m_sourceCoraPtr=corasvc.openDatabase(m_database,true);// true --> readonly
    //std::cout << "....database connections open OK" << std::endl;
    return true;
   }
  catch (std::exception&e) {
    m_logger << Root::kERROR << "Problem opening CORAL database: " << e.what() << Root::GEndl;
    return false;
  }
  //  std::cout << "Done the database opening" << std::endl;
  
  // list the COOL folders found in the database

//    const std::vector<std::string>& folders=m_sourceDbPtr->listAllNodes();
//    std::cout << "COOL database has " << folders.size() << " folders defined"
//  	    << std::endl;
//    for (std::vector<std::string>::const_iterator itr=folders.begin();
//         itr!=folders.end();++itr) std::cout << *itr << std::endl;

  return false;

}

//===========================================================================
std::string CoolQuery::transConn(const std::string& inconn) {
  // translate simple connection string (no slash) to mycool.db with given                                                                                                              
  // instance name, all others are left alone                                                                                                                                           
  if (inconn.find('/')==std::string::npos) {
    return "sqlite://X;schema=mycool.db;dbname="+inconn;
  } else {
    return inconn;
  }
}

//===========================================================================
unsigned int CoolQuery::getTriggerLevel(std::string triggername){

  size_t found = triggername.find_first_of("_");
  if(found != std::string::npos){
    std::string s_lvl = triggername.substr(0,found);
    if(s_lvl == "EF") return 3;
    if(s_lvl == "L2") return 2;
    if(s_lvl == "L1") return 1;
    if(s_lvl == "HLT") return 2;
  }

  // Indicate no valid trigger name passed
  return 0;
    
}

//===========================================================================
void CoolQuery::setIOV(const cool::ValidityKey start, const cool::ValidityKey stop){
  m_VKstart = start;
  m_VKstop = stop;
}
  
void CoolQuery::setIOVForRun(unsigned int runnum) {
  cool::ValidityKey run = runnum;
  m_VKstart = (run << 32);
  m_VKstop = ((run+1) << 32) - 1;
}

cool::Int32 CoolQuery::getL1PrescaleFromChannelId(const std::string& folder_name, const cool::ChannelId& id ){

  cool::IFolderPtr folder_ptr = m_sourceDbPtr->getFolder(folder_name);
  cool::IObjectIteratorPtr itr = folder_ptr->browseObjects(m_VKstart, m_VKstop,id);

  // Need to iterate once to get to first valid record, do it this way to avoid Coverity warning
  if (itr->goToNext()) {
    const cool::IRecord& payload=itr->currentRef().payload();    
    return payload["Lvl1Prescale"].data<cool::Int32>();
  }

  // Nonsense value
  return UINT_MAX;

}

cool::Float CoolQuery::getHLTPrescaleFromChannelId(const std::string& folder_name, const cool::ChannelId& id ){

  cool::IFolderPtr folder_ptr = m_sourceDbPtr->getFolder(folder_name);
  cool::IObjectIteratorPtr itr = folder_ptr->browseObjects(m_VKstart, m_VKstop,id);

  // Need to iterate once to get to first valid record, do it this way to avoid Coverity warning
  if (itr->goToNext()) {
    const cool::IRecord& payload=itr->currentRef().payload();    
    return payload["Prescale"].data<cool::Float>();
  }

  // Nonsense value
  return -1.;

}

cool::ChannelId CoolQuery::getL1ChannelId(const std::string& trigger, const std::string& folder_name){

  //  m_logger << Root::kINFO << "Getting channel id for L1 trigger [" << trigger << "] from folder [" << folder_name << "]" << Root::GEndl;

  m_valid = false;

  if (trigger == "") return UINT_MAX;
 
  cool::IFolderPtr folder_ptr = m_sourceDbPtr->getFolder(folder_name);
  cool::IObjectIteratorPtr obj_itr=folder_ptr->browseObjects(m_VKstart,m_VKstart, cool::ChannelSelection::all());
  // loop through all triggers
  while (obj_itr->goToNext()){
    const cool::IRecord& payload=obj_itr->currentRef().payload();
    // find the L1 trigger chain
    if(payload["ItemName"].data<std::string>() == trigger){
      m_valid = true;
      //      cout << "Channel id: " << obj_itr->currentRef().channelId() << endl;
      return obj_itr->currentRef().channelId();
    }
  }

  if(!m_valid){
    m_logger << Root::kERROR << "Couldn't find L1 trigger [" << trigger << "] in folder [" << folder_name << "]" << Root::GEndl;
  }


  // Nonsense value
  return UINT_MAX;
}

//===========================================================================
cool::ChannelId CoolQuery::getLumiChannelId(const std::string& lumimethod, const std::string& folder_name){

  m_valid = false;

  if (lumimethod == "") return UINT_MAX;
 
  // m_logger << Root::kINFO << "Getting channel id for Lumi method: " << lumimethod << " in folder " << folder_name << Root::GEndl;

  cool::IFolderPtr folder_ptr = m_sourceDbPtr->getFolder(folder_name);
  if(folder_ptr->existsChannel(lumimethod)){
    m_valid = true;
    return folder_ptr->channelId(lumimethod);
  }else{
    m_logger << Root::kWARNING << "Couldn't find lumimethod: " << lumimethod << " in COOL database!" << Root::GEndl;
  }

  // Nonsense value
  return UINT_MAX;
}

//===========================================================================
cool::ChannelId CoolQuery::getHLTChannelId(const std::string& trigger, const std::string& folder_name){

  // m_logger << Root::kINFO << "Getting channel id for HLT trigger [" << trigger << "] from folder [" << folder_name << "]" << Root::GEndl;

  m_valid = false;

  if (trigger == "") return UINT_MAX;

  cool::IFolderPtr folder_ptr = m_sourceDbPtr->getFolder(folder_name);
  cool::IObjectIteratorPtr obj_itr=folder_ptr->browseObjects(m_VKstart,m_VKstart, cool::ChannelSelection::all());
  // loop through all triggers
  // loop through all triggers
  while (obj_itr->goToNext()){
    const cool::IRecord& payload=obj_itr->currentRef().payload();
    if(payload["ChainName"].data<std::string>() == trigger){
      m_valid = true;
      // m_logger << Root::kINFO << "Found channel id: " << payload["ChainCounter"].data<cool::UInt32>() << Root::GEndl;
      return payload["ChainCounter"].data<cool::UInt32>();
    }
  }
  
  if(!m_valid){
    m_logger << Root::kERROR << "Couldn't find HLT trigger [" << trigger << "] in folder [" << folder_name << "]" << Root::GEndl;
  }

  // Nonsense value
  return UINT_MAX;
}

void
CoolQuery::printL1Triggers(const std::string& folder_name) {

  m_logger << Root::kINFO << "Listing available triggers [triggername(prescale, chanid)]: " << Root::GEndl;

  cool::IFolderPtr folder_ptr = m_sourceDbPtr->getFolder(folder_name);
  cool::IObjectIteratorPtr obj_itr=folder_ptr->browseObjects(m_VKstart,m_VKstart, cool::ChannelSelection::all());
  while (obj_itr->goToNext()){
    const cool::IRecord& payload=obj_itr->currentRef().payload();
    m_logger << Root::kINFO << payload["ItemName"].data<std::string>()  << "(" << this->getL1PrescaleFromChannelId("/TRIGGER/LVL1/Prescales",this->getL1ChannelId(payload["ItemName"].data<std::string>(), folder_name)) << ", " << obj_itr->currentRef().channelId() << "), ";
  }
  m_logger << Root::kINFO << Root::GEndl;
}

void
CoolQuery::printHLTTriggers(const std::string& folder_name) {

  m_logger << Root::kINFO << "Listing available triggers [triggername(prescale, chanid)]: " << Root::GEndl;

  cool::IFolderPtr folder_ptr = m_sourceDbPtr->getFolder(folder_name);
  cool::IObjectIteratorPtr obj_itr2=folder_ptr->browseObjects(m_VKstart,m_VKstart, cool::ChannelSelection::all());
  while (obj_itr2->goToNext()){
    const cool::IRecord& payload2=obj_itr2->currentRef().payload();
    //      m_logger << Root::kINFO << payload2["ChainName"].data<std::string>()  << ", ";
    m_logger << Root::kINFO << payload2["ChainName"].data<std::string>()  << "(" << payload2["Prescale"].data<cool::Float>() << ", " << payload2["ChainCounter"].data<cool::UInt32>() << "), ";
  }
  m_logger << Root::kINFO << Root::GEndl;
}

bool
CoolQuery::channelIdValid() {
  // m_logger << Root::kINFO << "channelIdValid = " << m_valid << Root::GEndl;
  return m_valid;
}

//===========================================================================
std::string CoolQuery::getHLTLowerChainName(const std::string& trigger, const std::string& folder_name){

  //  cout << "Getting lower chain name for trigger [" << trigger << "] from folder [" << folder_name << "] " << endl;
  bool found = false;
  cool::IFolderPtr folder_ptr = m_sourceDbPtr->getFolder(folder_name);
  cool::IObjectIteratorPtr obj_itr=folder_ptr->browseObjects(m_VKstart,m_VKstart, cool::ChannelSelection::all());
  // loop through all triggers
  while (obj_itr->goToNext()){
    const cool::IRecord& payload=obj_itr->currentRef().payload();
    if(payload["ChainName"].data<std::string>() == trigger){
      found = true;
      return payload["LowerChainName"].data<std::string>();

    }
  }

  if (!found) {
    m_logger << Root::kERROR << "Couldn't find HLT trigger [" << trigger << "] in folder [" << folder_name << "]" << Root::GEndl;

    //    m_logger << Root::kINFO << "List of triggers, hth: " << Root::GEndl;
    //    cool::IObjectIteratorPtr obj_itr2=folder_ptr->browseObjects(m_VKstart,m_VKstart, cool::ChannelSelection::all());
    //    while (obj_itr2->goToNext()){
    //      const cool::IRecord& payload2=obj_itr2->currentRef().payload();
    //      m_logger << Root::kINFO << payload2["ChainName"].data<std::string>()  << ", ";
    //    }
    //    m_logger << Root::kINFO << Root::GEndl;
  }

  return "";

}

//===========================================================================
std::map<cool::ValidityKey, CoolQuery::LumiFolderData> 
CoolQuery::getLumiFolderData(const std::string& folder_name, const std::string& tag, const cool::ChannelId& id ){

  std::map<cool::ValidityKey, LumiFolderData> mymap;
  LumiFolderData folderData;

  cool::IFolderPtr folder_ptr = m_sourceDbPtr->getFolder(folder_name);
  //  m_logger << Root::kWARNING << "Getting from database " << m_database << " tag " << tag << Root::GEndl; 
  if (!folder_ptr->existsChannel(id)) {
    m_logger << Root::kWARNING << "Lumi channel id " << id << " does not exist in database " << folder_name << "!" << Root::GEndl;
    return mymap;
  }

  cool::IObjectIteratorPtr itr;
  if(folder_ptr->existsUserTag(tag)) {
    itr = folder_ptr->browseObjects(m_VKstart, m_VKstop, id, tag);
  } else {
    // Try without specifying tag
    itr = folder_ptr->browseObjects(m_VKstart, m_VKstop, id);
  }

  while (itr->goToNext()) {
    const cool::IRecord& payload=itr->currentRef().payload();
    folderData.LBAvInstLumi = payload["LBAvInstLumi"].data<float>();
    folderData.LBAvEvtsPerBX = payload["LBAvEvtsPerBX"].data<float>();
    folderData.Valid = payload["Valid"].data<cool::UInt32>();
    mymap.insert( std::pair<cool::ValidityKey, LumiFolderData>(itr->currentRef().since(), folderData));
  }
  
  return mymap;

}

//===========================================================================
std::map<cool::ValidityKey, CoolQuery::L1CountFolderData> 
CoolQuery::getL1CountFolderData(const std::string& folder_name, const cool::ChannelId& id ){

  std::map<cool::ValidityKey, L1CountFolderData> mymap;
  L1CountFolderData folderData;

  cool::IFolderPtr folder_ptr = m_sourceDbPtr->getFolder(folder_name);
  //  m_logger << Root::kWARNING << "Getting from database " << m_database << " tag " << tag << Root::GEndl; 
  if (!folder_ptr->existsChannel(id)) {
    m_logger << Root::kWARNING << "Lumi channel id " << id << " does not exist in database " << folder_name << "!" << Root::GEndl;
    return mymap;
  }

  cool::IObjectIteratorPtr itr;

  itr = folder_ptr->browseObjects(m_VKstart, m_VKstop, id);

  while (itr->goToNext()) {
    const cool::IRecord& payload=itr->currentRef().payload();
    folderData.BeforePrescale = payload["BeforePrescale"].data<cool::UInt63>();
    folderData.AfterPrescale = payload["AfterPrescale"].data<cool::UInt63>();
    folderData.L1Accept = payload["L1Accept"].data<cool::UInt63>();
    mymap.insert( std::pair<cool::ValidityKey, L1CountFolderData>(itr->currentRef().since(), folderData));
  }
  
  return mymap;

}



