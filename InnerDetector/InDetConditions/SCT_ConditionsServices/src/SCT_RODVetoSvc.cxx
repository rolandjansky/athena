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

//Athena includes

#include "Identifier/IdentifierHash.h"
#include "InDetIdentifier/SCT_ID.h"
#include "SCT_ConditionsServices/ISCT_ConfigurationConditionsSvc.h"

//StoreGate Includes
#include "StoreGate/DataHandle.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"
#include "CxxUtils/make_unique.h"

// Constructor
SCT_RODVetoSvc::SCT_RODVetoSvc( const std::string& name, ISvcLocator* pSvcLocator ) : 
  AthService(name, pSvcLocator), 
  m_cabling("SCT_CablingSvc",name),
  m_badModuleIds{"BadSCTModuleIds_RODVeto"},
  m_pHelper{nullptr},
  m_detStore("DetectorStore", name)
  {
    declareProperty("BadModuleIds", m_badModuleIds, "Read key for bad module identifiers");
  }

//Initialize
StatusCode 
SCT_RODVetoSvc::initialize(){
  ATH_CHECK(m_detStore.retrieve());
  ATH_CHECK(m_detStore->retrieve(m_pHelper, "SCT_ID"));
  ATH_CHECK(m_cabling.retrieve());
  ATH_CHECK(m_badModuleIds.initialize());
 
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
  const IdentifierSet* badIds{getCondData()};
  if (badIds==nullptr) {
    ATH_MSG_ERROR("IdentifierSet cannot be retrieved in isGood. true is returned.");
    return true;
  }
  bool result = (badIds->find(elementId) == badIds->end());
  return result;
}

bool 
SCT_RODVetoSvc::isGood(const IdentifierHash & hashId){
  Identifier elementId=m_pHelper->wafer_id(hashId);
  Identifier moduleId =m_pHelper->module_id(elementId);
  return isGood(moduleId);
}

StatusCode 
SCT_RODVetoSvc::fillData(){
  ATH_MSG_ERROR("THIS SCT_RODVetoSvc::fillData() METHOD IS OBSOLETE.");
  return StatusCode::RECOVERABLE;
}

StatusCode 
SCT_RODVetoSvc::fillData(int& /*i*/ , std::list<std::string>& /*folderList*/){
  ATH_MSG_ERROR("A database fill callback was triggered for the SCT_RODVetoSvc, which does not use the database");
  return StatusCode::RECOVERABLE;
}

bool 
SCT_RODVetoSvc::canFillDuringInitialize(){
  return false;//uses the cabling, so cannot be filled in initialize
}
bool
SCT_RODVetoSvc::filled() const{
  return static_cast<bool>(getCondData());
}

const IdentifierSet*
SCT_RODVetoSvc::getCondData() const {
  SG::ReadHandle<IdentifierSet> condData{m_badModuleIds};
  if (not condData.isValid()) {
    ATH_MSG_ERROR("Failed to get " << m_badModuleIds.key());
    return nullptr;
  }
  return condData.cptr();
}
