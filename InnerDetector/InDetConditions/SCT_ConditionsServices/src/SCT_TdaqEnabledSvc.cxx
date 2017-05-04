/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_TdaqEnabledSvc.cxx
 * implementation file for service allowing one to declare rods as bad, or read the bad rods from the Tdaq entries in COOL db
 * @author shaun.roe@cern.ch
**/

#include "SCT_TdaqEnabledSvc.h"
//STL includes
#include <vector>
#include <list>
#include <algorithm>
#include <sstream>
#include <iterator>
#include "boost/array.hpp"
#include <iostream>
//Use Event info to determine whether folder is expetd to have valid data
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "SCT_Cabling/SCT_OnlineId.h"

//Gaudi includes
#include "GaudiKernel/StatusCode.h"

#include "Identifier/IdentifierHash.h"

#include "InDetIdentifier/SCT_ID.h"

//static const member initializer
const unsigned int SCT_TdaqEnabledSvc::NRODS=128;

static const std::string databaseSignature("database");
//The folder name changed between run 1 and run 2
static const std::string run1FolderName("/TDAQ/EnabledResources/ATLAS/SCT/Robins"); //a multi-channel, single version folder in the DB
static const std::string run2FolderName("/TDAQ/Resources/ATLAS/SCT/Robins");
static const unsigned int earliestRunForFolder(119253);
namespace{ //anonymous namespace to introduce file scope
/**
  //utility; is integer in given range
  bool
  inRange(const int var, const int lo, const int hi){
    return ((var>=lo) and (var<=hi));
  }
  **/
  //parse a rod channel name to a rod number, names are of the format 'ROL-SCT-BA-00-210000'
  //October 2014: names can now also be of format 'ROL-SCT-B-00-210100'
  unsigned int
  parseChannelName(const std::string & chanNameString){
    unsigned int result(0);
    const unsigned int length=chanNameString.size();
    if (length < 19) return result; //very rough check on sanity of string, should use regex
    //get the last six numbers, these are in hex  
    std::istringstream iss(chanNameString.substr(length-6, 6));
    iss.exceptions(std::ios_base::badbit|std::ios_base::failbit);
    try{
      iss>>std::hex>>result;
    } catch (std::ios_base::failure){ //bad conversion to int
      std::cerr<<"Bad conversion of last 6 digits of "<<chanNameString<<" to a hex number"<<std::endl;
      throw(std::ios_base::failure("stringToInt failure in SCT_TdaqEnabledSvc"));
    } 
    return result; 
  }
  
  //express a number of rods in words
  std::string
  inWords(const unsigned int aNumber){
    switch (aNumber){
      case 0:
        return std::string("No SCT rods were");
        break;
      case 1:
        return std::string("One SCT rod was");
        break;
      default:
        return std::to_string(aNumber) + " SCT rods were"; //C++11
        break;
    } 
  }
}
// Constructor
SCT_TdaqEnabledSvc::SCT_TdaqEnabledSvc( const std::string& name, ISvcLocator* pSvcLocator ) : AthService(name, pSvcLocator), 
m_filled(false), m_coolFolderName(""),m_pHelper(0),
m_useDatabase(true), m_detStore("DetectorStore",name),
m_storeGateSvc("StoreGateSvc",name),m_cablingSvc("SCT_CablingSvc", name), 
m_noneBad(true) {
  //declareProperty("BadRodIdentifiers",m_badElements);
}

//Initialize
StatusCode 
SCT_TdaqEnabledSvc::initialize(){
  //
  const std::string databaseUseString(m_useDatabase?"":"not ");
  ATH_MSG_INFO(" Database will "<<databaseUseString<<"be used.");
  //
  if (m_useDatabase) {
    m_coolFolderName=determineFolder(run1FolderName,run2FolderName);
    ATH_CHECK(m_detStore->regFcn(&SCT_TdaqEnabledSvc::fillData,this,m_dbList,m_coolFolderName));
  }
  ATH_CHECK(m_detStore->retrieve(m_pHelper,"SCT_ID"));
  // Retrieve cabling service
  ATH_CHECK(m_cablingSvc.retrieve());
  return StatusCode::SUCCESS;
}

//Finalize
StatusCode
SCT_TdaqEnabledSvc::finalize(){
  ATH_MSG_INFO(inWords(m_goodRods.size())<<" enabled in this run.");
  StatusCode sc(StatusCode::SUCCESS);
  //Code
  return sc;
}

// From s.binet
// Query the interfaces.
//   Input: riid, Requested interface ID
//          ppvInterface, Pointer to requested interface
//   Return: StatusCode indicating SUCCESS or FAILURE.
// N.B. Don't forget to release the interface after use!!!
StatusCode 
SCT_TdaqEnabledSvc::queryInterface(const InterfaceID& riid, void** ppvInterface) 
{
  if ( ISCT_ConditionsSvc::interfaceID().versionMatch(riid) ) {
    *ppvInterface = dynamic_cast<ISCT_ConditionsSvc*>(this);
  } else {
    // Interface is not directly available : try out a base class
    return AthService::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}

bool 
SCT_TdaqEnabledSvc::canReportAbout(InDetConditions::Hierarchy h){
  return (h==InDetConditions::DEFAULT or h==InDetConditions::SCT_SIDE or h==InDetConditions::SCT_MODULE); 
}

bool 
SCT_TdaqEnabledSvc::isGood(const Identifier & elementId, InDetConditions::Hierarchy h){
  if (not canReportAbout(h)) return true;
  //turn to hash, given the identifier
  const IdentifierHash hashId=m_pHelper->wafer_hash(elementId);
  return isGood(hashId);
}

bool 
SCT_TdaqEnabledSvc::isGood(const IdentifierHash & hashId){
  if (m_noneBad) return true;
  bool result = (m_goodIds.find(hashId) != m_goodIds.end());
  return result;
}

StatusCode 
SCT_TdaqEnabledSvc::fillData(){
  StatusCode sc(StatusCode::FAILURE);
  return sc;
  //remove possibility to fill from job options
}

StatusCode 
SCT_TdaqEnabledSvc::fillData(int& /*i*/ , std::list<std::string>& /*folderList*/){
  //check whether we expect valid data at this time
  if (unfilledRun()){
    m_noneBad=true;
    m_filled=true;
    return StatusCode::SUCCESS;
  }
  //clear structures before filling
  m_goodRods.clear();
  m_goodIds.clear();
  const int modulesPerRod(48);
  if (m_detStore->retrieve(m_dbList,m_coolFolderName).isFailure())  return StatusCode::FAILURE;
  CondAttrListCollection::const_iterator attrList( m_dbList->begin());
  CondAttrListCollection::const_iterator end( m_dbList->end());
  //CondAttrListCollection doesnt support C++11 type loops, no generic 'begin'
  for (;attrList!=end;++attrList) {
    //A CondAttrListCollection is a map of ChanNum and AttributeList
    CondAttrListCollection::ChanNum  channelNumber=attrList->first;
    CondAttrListCollection::AttributeList   payload=attrList->second;
    std::string enabled=payload["class"].data<std::string>();
    std::string chanName=m_dbList->chanName(channelNumber);
    unsigned int rodNumber=parseChannelName(chanName);
    //range check on the rod channel number has been removed, since it refers both to existing channel names
    //which can be rods in slots 1-128 but also historical names which have since been removed
    if (SCT_OnlineId::rodIdInRange(rodNumber)){
      if (!enabled.empty() and !m_goodRods.insert(rodNumber).second) msg(MSG::WARNING)<<"Set insertion failed for rod "<<rodNumber<<endmsg;
    } else {
      msg(MSG::WARNING)<<"Names in "<<m_coolFolderName<<" should be of the form ROL-SCT-BA-00-210000 this channel, number "<<channelNumber<<", is: "<<chanName<<endmsg;
    }
  }
  if (m_goodRods.size()>NRODS){
    msg(MSG::ERROR)<<"The number of rods declared as good appears to be greater than the permissible number of rods ("<<NRODS<<")"<<endmsg;
    m_filled=false;
    return StatusCode::FAILURE;
  }
  unsigned int nBad = NRODS-m_goodRods.size();
  std::string howMany=inWords(nBad);
  ATH_MSG_DEBUG(howMany << " declared bad in the database.");
  //provide short-cut for no rods bad (hopefully the most common case)
  m_noneBad=(nBad==0);
  if (m_noneBad){
    m_filled=true;
    return StatusCode::SUCCESS;
  }
 //now find the modules which belong to those rods...
  std::vector<IdentifierHash> tmpIdVec(0);
  tmpIdVec.reserve(modulesPerRod);
  for (const auto & thisRod : m_goodRods){
    tmpIdVec.clear();
    m_cablingSvc->getHashesForRod(tmpIdVec, thisRod);
    copy(tmpIdVec.begin(), tmpIdVec.end(), inserter(m_goodIds, m_goodIds.end()));
  }
  m_filled=true;
  return StatusCode::SUCCESS;
}

bool 
SCT_TdaqEnabledSvc::canFillDuringInitialize(){
  return (not m_useDatabase);// can only fill during intialize if we don't use the database
}
bool
SCT_TdaqEnabledSvc::filled() const{
  //code
  return m_filled;
}

bool
SCT_TdaqEnabledSvc::unfilledRun() const{
  const EventInfo* event;
  if (m_storeGateSvc->retrieve(event).isSuccess()) {
    const unsigned int runNumber=event->event_ID()->run_number();
    const bool noDataExpected=(runNumber < earliestRunForFolder);
    if (noDataExpected) ATH_MSG_INFO("This run occurred before the /TDAQ/EnabledResources/ATLAS/SCT/Robins folder was filled in COOL; assuming the SCT is all ok.");
    return noDataExpected;
  }
  ATH_MSG_ERROR("The event information could not be retrieved in this service");
  return false; //this means the service will attempt to access the folder and fill it, even though the run number is unknown
}

std::string 
SCT_TdaqEnabledSvc::determineFolder(const std::string& option1, const std::string& option2) const{
std::string result("");
 const bool option1Exists = m_detStore->contains<CondAttrListCollection>(option1);
 const bool option2Exists = m_detStore->contains<CondAttrListCollection>(option2);
 //its only sensible if either of these exists (but not both)
 const bool nonsense = (option1Exists == option2Exists);
 if (nonsense) {
   ATH_MSG_ERROR("The folder names "<<option1<<" or "<<option2<<" could not be found");
 } else {
 	 result = option1Exists ? option1 : option2;
 	 ATH_MSG_INFO("Sct_TdaqEnabledSvc will use the folder "<<result);
 }
 
 return result;
}
 
