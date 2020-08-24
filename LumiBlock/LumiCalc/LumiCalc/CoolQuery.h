/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef COOLQUERY_H
#define COOLQUERY_H

// Cool access
#include "CoolKernel/Exception.h"
#include "CoolKernel/FolderSpecification.h"
#include "CoolKernel/IDatabaseSvc.h"
#include "CoolKernel/IDatabase.h"
#include "CoolKernel/IFolder.h"
#include "CoolKernel/IFolderSet.h"
#include "CoolKernel/IObject.h"
#include "CoolKernel/IObjectIterator.h"
#include "CoolKernel/Exception.h"
#include "CoolKernel/pointers.h"
#include "CoolKernel/IDatabaseSvc.h"
#include "CoolApplication/DatabaseSvcFactory.h"
#include "LumiCalc/ReplicaSorter.h"

#include "RelationalAccess/IConnectionServiceConfiguration.h"
#include "RelationalAccess/ConnectionService.h"
#include "RelationalAccess/ISessionProxy.h"
#include "RelationalAccess/ITransaction.h"
#include "RelationalAccess/ISchema.h"
#include "RelationalAccess/ITable.h"
#include "RelationalAccess/IQuery.h"
#include "RelationalAccess/ICursor.h"

// Need this to keep other packages from failing, should be fixed properly
#include "LumiBlockData/LumiBlockCollection.h"

// LumiBlock includes
#include "xAODLuminosity/LumiBlockRangeContainer.h"
#include "xAODLuminosity/LumiBlockRangeAuxContainer.h"
#include "AthenaKernel/IOVRange.h"
#include "AthenaKernel/IOVTime.h"

// stl includes
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
#include <map>

// boost includes
//#include <boost/unordered_map.hpp>

// logger
#include "GoodRunsLists/TMsgLogger.h"

// magic includes
//#include <magic.h>

//================================================
// Simple class to hold

// Simple class to hold IOV-based data efficiently
template <class T>
class IOVData {

 public:
  IOVData() {
    clear();
  }

  ~IOVData() {
    clear();
  }

  void clear() {
    data.clear();
    m_last = data.begin();
    m_lastData = false;
  }

  // Main access function, returns value at specific time
  T getValue(IOVTime time);

  // Useful for LAr calculation, returns list of IOV-data pairs which intersect given range
  typename std::list<std::pair<IOVRange, T > > getOverlap(const IOVRange& range);

  // Insert element
  void add(IOVRange range, T val);

  // Data list
  typename std::list<std::pair<IOVRange, T > > data;

 private:
  bool m_lastData;

  // Cant use a map as IOVRange objects have no order
  typename std::list<std::pair<IOVRange, T > >::iterator m_last;

};

template <class T> 
void IOVData<T>::add(IOVRange range, T val) {
  data.push_back(std::pair<IOVRange, T>(range, val));
  m_last = data.begin();
  m_lastData = false;
}

template <class T> 
T IOVData<T>::getValue(IOVTime time) {

  // bool verbose = false;
  // if (verbose) {
  //   cout << endl << "getValue(" << time << ")" << endl;
  //   cout << "Data Size: " << data.size() << endl;
  // }

  if (data.empty()) return T(-1);

  typename std::list<std::pair<IOVRange, T> >::iterator it;

  // if (verbose) {
  //   for (it = data.begin(); it != data.end(); it++)
  //     cout << "(" << it->first << ", " << it->second << ")" << endl;
  //   cout << "Last is valid - " << m_lastData << endl;
  // }

  // Check if previous value is still good
  if (m_lastData) {
    // if (verbose) {
    //   cout << "Check previous " << m_last->first << endl;
    // }

    if ((m_last->first).isInRange(time)) return m_last->second;

    // Previous not good, try next as best guess
    if (++(m_last) != data.end()) {
      // if (verbose) cout << "Check next " << m_last->first << endl;
      if ((m_last->first).isInRange(time)) return m_last->second;
    }
  } else {
    m_last = data.begin();
  }

  m_lastData = false;

  // OK, suck it up and find the best value by stepping through entire list
  // Make sure we start on a valid value
  if ( m_last == data.end() ) --m_last;

  // Step forward looking for a match
  if (m_last->first.stop() <= time) {

    // if (verbose) cout << "Search upwards" << endl;
    for (++m_last; m_last != data.end(); ++m_last) {
      // if (verbose) cout << m_last->first << endl;
      if ((m_lastData = (m_last->first).isInRange(time))) return m_last->second;
    }

  } else if ( m_last != data.begin() && m_last->first.start() > time) {

    // if (verbose) cout << "Search downwards" << endl;
    do {
      --m_last;
      // if (verbose) cout << m_last->first << endl;
      if ((m_lastData = (m_last->first).isInRange(time))) return m_last->second;
    } while (m_last != data.begin());

  } else {

    if ((m_lastData = m_last->first.isInRange(time))) return m_last->second;

  } 

  // Set to valid data if we didn't find anything
  m_last = data.begin();
  m_lastData = false;

  // If we got to here, we didn't find it
  return T(-1);
}

template <class T> 
std::list<std::pair<IOVRange, T> > IOVData<T>::getOverlap(const IOVRange& range) {

  std::list<std::pair<IOVRange, T> > mydata;
  mydata.clear();

  // Find first time
  IOVTime firsttime = range.start();
  IOVTime lasttime = range.stop();

  // Use this to efficiently set the 'm_last' pointer
  getValue(firsttime);

  // Pointer to found element
  typename std::list<std::pair<IOVRange, T> >::iterator elem;

  std::pair<IOVRange, T> val;

  // Loop over elements
  for (elem = m_last; elem != data.end(); ++elem) {

    val = *elem;

    // Truncate this if necessary
    if (val.first.start() < firsttime || val.first.stop() > lasttime) {
      IOVTime iovstart((val.first.start() < firsttime ? firsttime : val.first.start()));
      IOVTime iovstop((val.first.stop() > lasttime ? lasttime : val.first.stop()));
      val.first = IOVRange(iovstart, iovstop);
    }
    mydata.push_back(val);

    // Check if we are done
    if (elem->first.isInRange(lasttime)) break;
  }

  return mydata;
}

/***************************************
 * This class serves as a common engine
 * for Cool Query by fetching 
 * up basic values from COOL for a single
 * trigger chain, trigger db and lumi db
 ****************************************/

class CoolQuery{
 public:

  // constructor
  CoolQuery(const std::string& database, const std::string& triggerchain);

  // destructor
  ~CoolQuery();

  // Set trigger database
  void setDb(const std::string& database){m_database = database;}

  // Set trigger
  void setTrigger(const std::string& triggerchain){m_triggerchain = triggerchain;}


  // Opens database connection
  bool openDbConn();

  // set the "central" IOV range to this time (presumable this comes from some LumiBlockCollection's IOVRange
  // but one in general can create one such object via e.g.:
  //
  //  * iovc = new LumiBlockCollection();
  // iovc->push_back(new IOVRange(IOVTime(runnumber, LBstart), IOVTime(runnumber,LBend)));
  // LumiBlockCollection::const_iterator i = iovc->begin();
  //  
  // then use it via
  //
  // mycoolquery->setIOV(iovr->start().re_time(), iovr->stop().re_time());
  //
  void setIOV(const cool::ValidityKey start, const cool::ValidityKey stop);

  void setIOVForRun(unsigned int runnum);

  // returns HLT channelId of trigger "trigger" in folder "folder_name"
  cool::ChannelId getHLTChannelId(const std::string& trigger, const std::string& folder_name);

  // returns L1 channelId of trigger "trigger" in folder "folder_name"
  cool::ChannelId getL1ChannelId(const std::string& trigger, const std::string& folder_name);

  // returns Lumi channelId for lumimethod in folder_name
  cool::ChannelId getLumiChannelId(const std::string& lumimethod, const std::string& folder_name);

  // utility to indicate whether ChannelID returned by any of the above is valid
  bool channelIdValid();

  // returns "LowerChainName" of "trigger" in "folder_name" folder
  std::string getHLTLowerChainName(const std::string& trigger, const std::string& folder_name);

  // handy function  to get quickly prescale
  cool::Int32 getL1PrescaleFromChannelId(const std::string& folder_name, const cool::ChannelId& id );

  // handy function  to get quickly prescale
  cool::Float getHLTPrescaleFromChannelId(const std::string& folder_name, const cool::ChannelId& id );

  // Print list of triggers
  void printL1Triggers(const std::string& folder_name);
  void printHLTTriggers(const std::string& folder_name);

  // note this is mainly for trigger dependent variables, counters, prescales, etc.
  template <class T>
    std::map<cool::ValidityKey, T> getObjMapFromFolderAtChan(const std::string& obj_name, const std::string& folder_name, const cool::ChannelId& id);

  template <class T>
    std::map<cool::ValidityKey, T> getTrigObjMapFromFolderByName(const std::string& obj_name, const std::string& folder_name, const std::string& trigger);

  // same but giving an (unordered) map with IOVRange key
  //  template <class T>
  //  unordered_map<IOVRange, T> getTrigObjMapFromFolderByNameIOV(const std::string& obj_name, const std::string& folder_name, const std::string& trigger);

  template <class T>
    std::map<cool::ValidityKey, T> getLumiIterator(const std::string& luminame, const std::string& folder_name, const std::string& tag, const cool::ChannelId& id );

  // returns numerical 1/2/3 values for trigger "triggername" 
  unsigned int getTriggerLevel(std::string triggername);    

  // Retrieve all lumi data at once
  struct LumiFolderData {
    float LBAvInstLumi;
    float LBAvEvtsPerBX;
    cool::UInt32 Valid;
    LumiFolderData() : LBAvInstLumi(), LBAvEvtsPerBX(), Valid() {}
  };

  std::map<cool::ValidityKey, LumiFolderData> 
    getLumiFolderData(const std::string& folder_name, const std::string& tag, const cool::ChannelId& id );

  // Retrieve all L1 counter data at once
  struct L1CountFolderData {
    cool::UInt63 BeforePrescale;
    cool::UInt63 AfterPrescale;
    cool::UInt63 L1Accept;  // After L1 veto
  };

  std::map<cool::ValidityKey, L1CountFolderData> 
    getL1CountFolderData(const std::string& folder_name, const cool::ChannelId& id);

  // Retrieve prescale data into cached data object
  template <class T>
    IOVData<T> getIOVData(const std::string& name, const std::string& folder_name, const cool::ChannelId& id, const std::string& tag="");
 
 private:
  std::string transConn(const std::string& inconn);

  coral::ConnectionService m_coralsvc;  
  ReplicaSorter* m_repsort;
  cool::IDatabasePtr m_sourceDbPtr;
  std::string m_database;
  std::string m_triggerchain;
  cool::ValidityKey m_VKstart;
  cool::ValidityKey m_VKstop;
  
  Root::TMsgLogger m_logger;

  bool m_valid;
};

/* template <class T> */
/* unordered_map<IOVRange, T> CoolQuery::getTrigObjMapFromFolderByNameIOV(const std::string& obj_name, const std::string& folder_name, const std::string& trigger){ */
/*   unordered_map<IOVRange, T> test; */
/*   return test; */

/* } */



//===========================================================================
template <class T>
std::map<cool::ValidityKey, T> CoolQuery::getTrigObjMapFromFolderByName(const std::string& obj_name, const std::string& folder_name, const std::string& trigger){
  // m_logger << Root::kWARNING << "Getting object [" << obj_name << "] from folder [" << folder_name << "], in LB range: " << (m_VKstart >> 32) << ", " << (m_VKstart & 0xFFFFFFFF) << " - " << (m_VKstop >> 32) << ","<< (m_VKstop & 0xFFFFFFFF)  << Root::GEndl;
  std::map<cool::ValidityKey, T> mymap;
  bool found = false;
  cool::IFolderPtr folder_ptr = m_sourceDbPtr->getFolder(folder_name);
  cool::IObjectIteratorPtr obj_itr=folder_ptr->browseObjects(m_VKstart,m_VKstart, cool::ChannelSelection::all());
  // loop through all triggers
  while (obj_itr->goToNext()){

    const cool::IRecord& payload=obj_itr->currentRef().payload();
    //    std::cout << "ChainNames: " << payload["ChainName"].data<std::string>() << std::endl;

    if(payload["ChainName"].data<std::string>() == trigger){
      found = true;
      const cool::IRecord& payload=obj_itr->currentRef().payload();
      //      std::cout << "Inserting: " << (obj_itr->currentRef().since() >> 32) << " - " << (obj_itr->currentRef().since() & 0xFFFFFF) << ", " << payload[obj_name].data<T>() << std::endl;
      mymap.insert( std::pair<cool::ValidityKey, T>(obj_itr->currentRef().since() ,payload[obj_name].data<T>()));
    }
  }

  if(!found){
    m_logger << Root::kERROR << "Couldn't find HLT trigger [" << trigger << "] in folder [" << folder_name << "]" << Root::GEndl;
  }

  return  mymap;
}


//===========================================================================
template <class T>
std::map<cool::ValidityKey, T> CoolQuery::getObjMapFromFolderAtChan(const std::string& obj_name, const std::string& folder_name, const cool::ChannelId& id){
  //  m_logger << Root::kWARNING << "Getting object [" << obj_name << "] from folder [" << folder_name << "], in channel: " << id << ", in LB range: " << (m_VKstart >> 32) << ", " << (m_VKstart & 0xFFFFFFFF) << " - " << (m_VKstop >> 32) << ","<< (m_VKstop & 0xFFFFFFFF)  << Root::GEndl;
  std::map<cool::ValidityKey, T> mymap;
  cool::IFolderPtr folder_ptr = m_sourceDbPtr->getFolder(folder_name);
  cool::ChannelSelection chsel(id);
  if(folder_ptr->existsChannel(id)){
    cool::IObjectIteratorPtr itr = folder_ptr->browseObjects(m_VKstart, m_VKstop,chsel);
    while (itr->goToNext()) {
      const cool::IRecord& payload=itr->currentRef().payload();
      //     std::cout << "Inserting: " << (itr->currentRef().since() >> 32) << " - " << (itr->currentRef().since() & 0xFFFFFF) << ", " << payload[obj_name].data<T>() << std::endl;
      mymap.insert( std::pair<cool::ValidityKey, T>(itr->currentRef().since() ,payload[obj_name].data<T>()));
    }
  }else{
    m_logger << Root::kWARNING << "Channel " << id << " does not exist in database." << Root::GEndl; 
/*     const std::map<cool::ChannelId,std::string> list = folder_ptr->listChannelsWithNames(); */
  }
  return  mymap;
}


//===========================================================================
template <class T>
std::map<cool::ValidityKey, T> CoolQuery::getLumiIterator(const std::string& luminame, const std::string& folder_name, const std::string& tag, const cool::ChannelId& id ){

  std::map<cool::ValidityKey, T> mymap;
  cool::IFolderPtr folder_ptr = m_sourceDbPtr->getFolder(folder_name);
  //  m_logger << Root::kWARNING << "Getting from database " << m_database << " tag " << tag << Root::GEndl; 
  if(folder_ptr->existsChannel(id) && folder_ptr->existsUserTag(tag)){
    cool::IObjectIteratorPtr itr = folder_ptr->browseObjects(m_VKstart, m_VKstop, id, tag);
    while (itr->goToNext()) {
      const cool::IRecord& payload=itr->currentRef().payload();
      mymap.insert( std::pair<cool::ValidityKey, T>(itr->currentRef().since() ,payload[luminame].data<T>()));
    }
  }else if(folder_ptr->existsChannel(id) && !folder_ptr->existsUserTag(tag)){
    // ok, tag doesn't exist, still try to use an untagged way of accessing data
    mymap = CoolQuery::getObjMapFromFolderAtChan<T>(luminame, folder_name,id);
  }else{
    m_logger << Root::kWARNING << "Lumi tag " << tag << " does not exist, or Lumi channel id " << id << " does not exist in database." << Root::GEndl;
  }
  return mymap;

}

//===========================================================================
template <class T>
IOVData<T>
CoolQuery::getIOVData(const std::string& obj_name, const std::string& folder_name, const cool::ChannelId& id, const std::string& tag) {

  IOVData<T> mydata;

  cool::IFolderPtr folder_ptr = m_sourceDbPtr->getFolder(folder_name);
  cool::ChannelSelection chsel(id);
  if(!folder_ptr->existsChannel(id)){
    m_logger << Root::kWARNING << "Channel " << id << " does not exist in database " << folder_name << "!" << Root::GEndl; 
    return mydata;
  }

  // Try with or without tag
  cool::IObjectIteratorPtr itr;
  if (folder_ptr->existsUserTag(tag)) {
    itr = folder_ptr->browseObjects(m_VKstart, m_VKstop, id, tag);
  } else {
    itr = folder_ptr->browseObjects(m_VKstart, m_VKstop, id);
  }

  while (itr->goToNext()) {
    IOVTime since, until;
    since.setRETime(itr->currentRef().since());
    until.setRETime(itr->currentRef().until());

    IOVRange range(since, until);

    const cool::IRecord& payload=itr->currentRef().payload();

    mydata.add( range, payload[obj_name].data<T>() );
  }

  return  mydata;
}



#endif //> COOLQUERY_H


