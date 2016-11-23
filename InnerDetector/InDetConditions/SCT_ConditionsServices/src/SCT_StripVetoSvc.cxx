/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_StripVetoSvc.cxx
 * implementation file for service allowing one to declare strips as bad
 * @author shaun.roe@cern.ch
**/

#include "SCT_StripVetoSvc.h"
//STL includes
#include <vector>

//Gaudi includes
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ServiceHandle.h"

#include "StoreGate/StoreGateSvc.h"
#include "Identifier/IdentifierHash.h"

#include "InDetIdentifier/SCT_ID.h"

// Constructor
SCT_StripVetoSvc::SCT_StripVetoSvc( const std::string& name, ISvcLocator* pSvcLocator ) : AthService(name, pSvcLocator), m_filled(false),m_pHelper(0){
  declareProperty("BadStripIdentifiers",m_badElements);
}

//Initialize
StatusCode 
SCT_StripVetoSvc::initialize(){
  static const StatusCode fail(StatusCode::FAILURE);
  StatusCode sc(StatusCode::SUCCESS);
  if (fillData().isFailure()) return msg(MSG:: ERROR)<<"Failed to fill data"<<endmsg, fail;
  //
  msg(MSG::INFO)<<"Initialized veto service with data, "<<m_badElements.value().size()<<" elements declared bad."<<endmsg;
  //
  ServiceHandle<StoreGateSvc>    detStore("DetectorStore",name());
  if (detStore.retrieve().isFailure()) return (msg(MSG:: FATAL) << "Detector service not found !" << endmsg), fail;
  if (detStore->retrieve(m_pHelper,"SCT_ID").isFailure()) return msg(MSG:: ERROR)<<"SCT helper failed to retrieve"<<endmsg, fail;
  return sc;
}

//Finalize
StatusCode
SCT_StripVetoSvc::finalize(){
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
SCT_StripVetoSvc::queryInterface(const InterfaceID& riid, void** ppvInterface) 
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
SCT_StripVetoSvc::canReportAbout(InDetConditions::Hierarchy h){
  return (h==InDetConditions::SCT_STRIP);//default case is the side, which we cant report on
}

bool 
SCT_StripVetoSvc::isGood(const Identifier & elementId, InDetConditions::Hierarchy h){
  if (not canReportAbout(h)) return true;
  return (m_badIds.find(elementId) == m_badIds.end());
}

bool 
SCT_StripVetoSvc::isGood(const IdentifierHash & /*hashId*/){//comment out unused parameter to prevent compiler warning
  return true; //cant answer questions about the module side
}

StatusCode 
SCT_StripVetoSvc::fillData(){
  StatusCode sc(StatusCode::SUCCESS);
  if (m_badElements.value().empty()) msg(MSG::INFO)<<"No bad strips."<<endmsg;
  std::vector<std::string>::const_iterator pId=m_badElements.value().begin();
  std::vector<std::string>::const_iterator last=m_badElements.value().end();
  bool success(true);
  for(;pId not_eq last;++pId){
    success &= m_badIds.insert(Identifier(static_cast<Identifier::value_type>(atoll(pId->c_str())))).second;
  }
  m_filled=true;
  return success?sc:(StatusCode::FAILURE);
}

bool
SCT_StripVetoSvc::filled() const{
  //code
  return m_filled;
}

