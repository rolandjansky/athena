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
SCT_RODVetoSvc::SCT_RODVetoSvc( const std::string& name, ISvcLocator* pSvcLocator ) : 
  AthService(name, pSvcLocator), 
  m_sct_id(0),
  m_cabling("SCT_CablingSvc",name),
  m_badElements(0),
  m_filled(false), 
  m_pHelper(0), 
  m_useDatabase(false),  
  m_detStore("DetectorStore",name){
  declareProperty("BadRODIdentifiers",m_badRODElements);
  declareProperty("CablingService",m_cabling);
}

//Initialize
StatusCode 
SCT_RODVetoSvc::initialize(){
  StatusCode sc(StatusCode::SUCCESS);
  //
  sc = m_cabling.retrieve() ;
  if (sc.isFailure()) {
    msg(MSG::FATAL) << "Cannot retrieve cabling!"  << endreq;
    return StatusCode::SUCCESS;
  } 
  
  msg(MSG::INFO)<<"Start getting list of RODs."<<endreq;

  std::vector<boost::uint32_t> listOfRODs;
  m_cabling->getAllRods(listOfRODs);
  std::vector<boost::uint32_t>::iterator rodIter = listOfRODs.begin();
  std::vector<boost::uint32_t>::iterator rodEnd = listOfRODs.end();

  msg(MSG::INFO) << "listofRODS size: " << listOfRODs.size() << endreq;
  int i_rod=0;
  for (; rodIter != rodEnd; ++rodIter) {
    std::vector<IdentifierHash> listOfHashes;
    msg(MSG::INFO) << "rodIter number: " << (*rodIter)<<endreq;
    if (std::find(m_badRODElements.value().begin(), m_badRODElements.value().end(), i_rod) != m_badRODElements.value().end()) {
      msg(MSG::INFO)<<"Mask ROD:"<< (*rodIter)<<endreq;
      m_cabling->getHashesForRod(listOfHashes,(*rodIter));
      std::vector<IdentifierHash>::iterator hashIt = listOfHashes.begin();
      std::vector<IdentifierHash>::iterator hashEnd = listOfHashes.end();
      for (; hashIt != hashEnd; ++hashIt) {
        msg(MSG::INFO)<<"In for hash."<<endreq;
        Identifier wafId = m_sct_id->wafer_id(*hashIt);
        msg(MSG::INFO)<<"a"<<endreq;
        Identifier modId = m_sct_id->module_id(wafId);
        msg(MSG::INFO)<<"b"<<endreq;
        std::string modSt = modId.getString();
        msg(MSG::INFO)<<"c"<<endreq;
        m_badElements.push_back(modSt);
        msg(MSG::INFO)<<"d"<<endreq;
      }
    }
  i_rod++;
  }

  //
  m_useDatabase=(std::find(m_badElements.begin(),m_badElements.end(),databaseSignature) != m_badElements.end());
  if (not m_useDatabase){
    if (fillData().isFailure()) return msg(MSG::ERROR)<<"Failed to fill data"<<endreq, StatusCode::FAILURE;
  } else {
    if (m_detStore->regFcn(&SCT_RODVetoSvc::fillData,this,m_dbList,coolFolderName).isFailure()) return msg(MSG::ERROR)<<"Failed to register callback"<<endreq, StatusCode::FAILURE;
  }
  //
  const std::string databaseUseString(m_useDatabase?"":"not ");
  msg(MSG::INFO)<<"Initialized veto service with data, "
  <<(m_badElements.size() - int(m_useDatabase))
  <<" elements declared bad. Database will "<<databaseUseString<<"be used."<<endreq;
  //
  if (m_detStore->retrieve(m_pHelper,"SCT_ID").isFailure()) return msg(MSG::ERROR)<<"SCT helper failed to retrieve"<<endreq, StatusCode::FAILURE;
  return sc;
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
  return ((h==InDetConditions::DEFAULT) or (h==InDetConditions::SCT_SIDE) );
}

bool 
SCT_RODVetoSvc::isGood(const Identifier & elementId, InDetConditions::Hierarchy h){
  if (not canReportAbout(h)) return true;
  bool result = (m_badIds.find(elementId) == m_badIds.end());
  return result;
}

bool 
SCT_RODVetoSvc::isGood(const IdentifierHash & hashId){
  Identifier elementId=m_pHelper->wafer_id(hashId);
  return isGood(elementId);
}

StatusCode 
SCT_RODVetoSvc::fillData(){
  StatusCode sc(StatusCode::SUCCESS);
  if ((m_badElements.size() - int(m_useDatabase)) == 0){
    msg(MSG::INFO)<<"No bad RODs in job options."<<endreq;
    return sc;
  } 
  bool success(true);
  std::vector<std::string>::const_iterator pId=m_badElements.begin();
  std::vector<std::string>::const_iterator last=m_badElements.end();
  for(;pId not_eq last;++pId){
    if (*pId != databaseSignature) success &= m_badIds.insert(Identifier(atoi(pId->c_str()))).second;
  }
  m_filled=true;
  return success?sc:(StatusCode::FAILURE);
}

StatusCode 
SCT_RODVetoSvc::fillData(int& /*i*/ , std::list<std::string>& /*folderList*/){
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
SCT_RODVetoSvc::canFillDuringInitialize(){
  return (not m_useDatabase);// can only fill during intialize if we don't use the database
}
bool
SCT_RODVetoSvc::filled() const{
  //code
  return m_filled;
}

