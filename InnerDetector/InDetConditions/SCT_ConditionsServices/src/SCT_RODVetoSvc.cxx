/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_RODVetoSvc.cxx
 * implementation file for service allowing one to declare modules as bad
 * @author daiki.hayakawa@cern.ch
 **/

#include "SCT_RODVetoSvc.h"
//STL includes
#include <vector>
#include <list>
#include <algorithm>
#include <iterator>
#include <string>
#include <sstream>

//Gaudi includes
#include "GaudiKernel/StatusCode.h"

#include "Identifier/IdentifierHash.h"

#include "InDetIdentifier/SCT_ID.h"

#include "SCT_ConditionsServices/ISCT_ConfigurationConditionsSvc.h"


//StoreGate Includes
#include "StoreGate/DataHandle.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"
#include "CxxUtils/make_unique.h"


template <class T> 
static std::vector<T> 
string2Vector(const std::string & s){
  std::vector<T> v;
  std::istringstream inputStream(s);
  std::istream_iterator<T> vecRead(inputStream);
  std::istream_iterator<T> endOfString; //relies on default constructor to produce eof
  std::copy(vecRead,endOfString,std::back_inserter(v));// DOESN'T ALLOW NON-WHITESPACE DELIMITER !
  return v;
}

// Constructor
SCT_RODVetoSvc::SCT_RODVetoSvc( const std::string& name, ISvcLocator* pSvcLocator ) : 
  AthService(name, pSvcLocator), 
  m_cabling("SCT_CablingSvc",name),
  m_badRODElements("BadRODIdentifiers"),
  m_wFilled("isFilled"), 
  m_rFilled("isFilled"),
  m_pHelper{nullptr},
  m_detStore("DetectorStore", name)
  {
    declareProperty("BadRODIdentifiers",m_badRODElements );
    declareProperty("w_isFilled", m_wFilled );
    declareProperty("r_isFilled", m_rFilled );
  }

//Initialize
StatusCode 
SCT_RODVetoSvc::initialize(){
  ATH_CHECK(m_detStore.retrieve());
  ATH_CHECK(m_detStore->retrieve(m_pHelper, "SCT_ID"));
  ATH_CHECK(m_cabling.retrieve());
  ATH_CHECK(m_badRODElements.initialize());
  ATH_CHECK(m_wFilled.initialize());
  ATH_CHECK(m_rFilled.initialize());
 
  return  StatusCode::SUCCESS;
}

//Finalize
StatusCode
SCT_RODVetoSvc::finalize(){
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
SCT_RODVetoSvc::queryInterface(const InterfaceID& riid, void** ppvInterface) 
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
SCT_RODVetoSvc::canReportAbout(InDetConditions::Hierarchy h){
  return ((h==InDetConditions::DEFAULT) or (h==InDetConditions::SCT_SIDE));
}

bool 
SCT_RODVetoSvc::isGood(const Identifier & elementId, InDetConditions::Hierarchy h){
  if (not canReportAbout(h)) return true;
  if (not filled() and fillData().isFailure()) ATH_MSG_WARNING("Data structure could not be filled");
  bool result = (m_badIds.find(elementId) == m_badIds.end());
  return result;
}

bool 
SCT_RODVetoSvc::isGood(const IdentifierHash & hashId){
  if (not filled() and fillData().isFailure()) ATH_MSG_WARNING("Data structure could not be filled");
  Identifier elementId=m_pHelper->wafer_id(hashId);
  Identifier moduleId =m_pHelper->module_id(elementId);
  return isGood(moduleId);
}

StatusCode 
SCT_RODVetoSvc::fillData(){
  
  SG::ReadHandle<  std::vector<unsigned int> > badRODElements (m_badRODElements);
  if (!badRODElements.isValid()){
    ATH_MSG_ERROR("Can't find BadRODIdentifiers in StoreGate");
    return StatusCode::FAILURE;
  } else
    if ((*badRODElements).empty()){
      ATH_MSG_INFO("No bad RODs in job options.");
      return StatusCode::SUCCESS;
    } else
      if (filled() ) {
        ATH_MSG_INFO("RodVetoSvc has already been successfully filled.");
        return StatusCode::SUCCESS;
      }   

 
  ATH_MSG_INFO((*badRODElements).size() <<" RODs were declared bad");
  bool success(true);
  
  std::vector<unsigned int> allRods;
  m_cabling->getAllRods(allRods);
  
  for(unsigned int thisRod: (*badRODElements) ){
    //check whether rod exists
    if (std::find(allRods.begin(),allRods.end(),thisRod)==allRods.end()){
      ATH_MSG_WARNING("Your vetoed selection "<<std::hex<<"0x"<<thisRod<<" does not exist."<<std::dec);
      continue;
    }
    std::vector<IdentifierHash> listOfHashes;
    m_cabling->getHashesForRod(listOfHashes,thisRod);
    ATH_MSG_DEBUG("This rod is "<<std::hex<<"0x"<<thisRod<<std::dec);
    //Two consecutive hashes may produce the same module id, since they will be two sides
    //of the same module. We avoid invalid inserts by guarding against this.
    Identifier previousId; //constructor produces an invalid one
    for (const IdentifierHash &thisHash:listOfHashes){
      Identifier wafId = m_pHelper->wafer_id(thisHash);
      Identifier modId = m_pHelper->module_id(wafId);
      const bool alreadyInserted(modId==previousId);
      previousId=modId;
      if (alreadyInserted) continue;
      ATH_MSG_VERBOSE("This Id is "<<modId);
      const bool thisInsertionOk=m_badIds.insert(modId).second;
      if (not thisInsertionOk){
        ATH_MSG_WARNING("Insertion failed for rod "<<thisRod<<" and module (hash,id): "<<thisHash<<", "<<modId);
      }
      success &= thisInsertionOk;
    }
  }


  //At this point, as WriteHandles only work once and we cannot update them
  //we will only set isFilled if it is true. If false it will not be set
  if ( success ){
    SG::WriteHandle<bool> wFilled (m_wFilled);
    ATH_CHECK( wFilled.record( CxxUtils::make_unique<bool> (success))); 


    if ( !wFilled.isValid() ){
      ATH_MSG_INFO("isFilled not set yet");
      
    } 

    std::cout << "Recorded WH with key: " << wFilled.key() << " and value: " << *wFilled.cptr() << std::endl;   //wFilled->val() << std::endl; 
    //Need to add a checck if this succeeded
    if (*wFilled) ATH_MSG_INFO("Structure successfully filled with "<<m_badIds.size()<<" modules.");
  }


  return success?(StatusCode::SUCCESS):(StatusCode::FAILURE);

}

StatusCode 
SCT_RODVetoSvc::fillData(int& /*i*/ , std::list<std::string>& /*folderList*/){
  ATH_MSG_WARNING("A database fill callback was triggered for the SCT_RODVetoSvc, which does not use the database");
  return StatusCode::RECOVERABLE;
}

bool 
SCT_RODVetoSvc::canFillDuringInitialize(){
  return false;//uses the cabling, so cannot be filled in initialize
}
bool
SCT_RODVetoSvc::filled() const{
  //code
  SG::ReadHandle<bool> rFilled (m_rFilled);
  if (!rFilled.isValid()){
    ATH_MSG_INFO("No value found for isFilled in SG, assuming false");
    return false;
  }else return *rFilled;
}
