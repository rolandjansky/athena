/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_ModuleVetoSvc.cxx
 * implementation file for service allowing one to declare modules as bad
 * @author shaun.roe@cern.ch
**/

#include "SCT_ModuleVetoSvc.h"
//STL includes
#include <vector>
#include <list>
#include <algorithm>
#include <sstream>
#include <iterator>

//Gaudi includes
#include "GaudiKernel/StatusCode.h"

#include "Identifier/IdentifierHash.h"

#include "InDetIdentifier/SCT_ID.h"

static const std::string databaseSignature("database");
static const std::string coolFolderName("/SCT/Manual/BadModules"); //a single-channel folder in the DB

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
SCT_ModuleVetoSvc::SCT_ModuleVetoSvc( const std::string& name, ISvcLocator* pSvcLocator ) : AthService(name, pSvcLocator), m_filled(false), m_pHelper(0),
m_useDatabase(false), m_detStore("DetectorStore",name){
  declareProperty("BadModuleIdentifiers",m_badElements);
}

//Initialize
StatusCode 
SCT_ModuleVetoSvc::initialize(){
  StatusCode sc(StatusCode::SUCCESS);
 //
  m_useDatabase=(std::find(m_badElements.value().begin(),m_badElements.value().end(),databaseSignature) != m_badElements.value().end());
  if (not m_useDatabase){
    if (fillData().isFailure()) return msg(MSG::ERROR)<<"Failed to fill data"<<endreq, StatusCode::FAILURE;
  } else {
     if (m_detStore->regFcn(&SCT_ModuleVetoSvc::fillData,this,m_dbList,coolFolderName).isFailure()) return msg(MSG::ERROR)<<"Failed to register callback"<<endreq, StatusCode::FAILURE;
  }
  //
  const std::string databaseUseString(m_useDatabase?"":"not ");
  msg(MSG::INFO)<<"Initialized veto service with data, "
  <<(m_badElements.value().size() - int(m_useDatabase))
  <<" elements declared bad. Database will "<<databaseUseString<<"be used."<<endreq;
  //
  if (m_detStore->retrieve(m_pHelper,"SCT_ID").isFailure()) return msg(MSG::ERROR)<<"SCT helper failed to retrieve"<<endreq, StatusCode::FAILURE;
  return sc;
}

//Finalize
StatusCode
SCT_ModuleVetoSvc::finalize(){
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
SCT_ModuleVetoSvc::queryInterface(const InterfaceID& riid, void** ppvInterface) 
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
SCT_ModuleVetoSvc::canReportAbout(InDetConditions::Hierarchy h){
  return ((h==InDetConditions::DEFAULT) or (h==InDetConditions::SCT_SIDE) );
}

bool 
SCT_ModuleVetoSvc::isGood(const Identifier & elementId, InDetConditions::Hierarchy h){
  if (not canReportAbout(h)) return true;
  bool result = (m_badIds.find(elementId) == m_badIds.end());
  return result;
}

bool 
SCT_ModuleVetoSvc::isGood(const IdentifierHash & hashId){
  Identifier elementId=m_pHelper->wafer_id(hashId);
  return isGood(elementId);
}

StatusCode 
SCT_ModuleVetoSvc::fillData(){
  StatusCode sc(StatusCode::SUCCESS);
  if ((m_badElements.value().size() - int(m_useDatabase)) == 0){
    msg(MSG::INFO)<<"No bad modules in job options."<<endreq;
    return sc;
  } 
  bool success(true);
  std::vector<std::string>::const_iterator pId=m_badElements.value().begin();
  std::vector<std::string>::const_iterator last=m_badElements.value().end();
  for(;pId not_eq last;++pId){
    if (*pId != databaseSignature) success &= m_badIds.insert(Identifier(atoi(pId->c_str()))).second;
  }
  m_filled=true;
  return success?sc:(StatusCode::FAILURE);
}

StatusCode 
SCT_ModuleVetoSvc::fillData(int& /*i*/ , std::list<std::string>& /*folderList*/){
  StatusCode sc(fillData());
  const StatusCode fail(StatusCode::FAILURE);
  if (sc.isFailure() ) return fail;
  if (m_detStore->retrieve(m_dbList,coolFolderName).isFailure())  return fail;
  std::string badModuleString=m_dbList[0]["ModuleList"].data<std::string>();
  std::vector<int> v=string2Vector<int>(badModuleString);
  int numberInDb=v.size();
  msg(MSG::INFO)<<numberInDb<<" elements were declared bad in the database."<<endreq;
  for (std::vector<int>::const_iterator i(v.begin());i!=v.end();++i){
    m_badIds.insert(Identifier(*i));
  }
  return sc;
}

bool 
SCT_ModuleVetoSvc::canFillDuringInitialize(){
  return (not m_useDatabase);// can only fill during intialize if we don't use the database
}
bool
SCT_ModuleVetoSvc::filled() const{
  //code
  return m_filled;
}

