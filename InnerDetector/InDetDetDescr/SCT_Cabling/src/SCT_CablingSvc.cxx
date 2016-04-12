/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_CablingSvc.cxx
 * Implementation file for  SCT cabling service
 * @author Shaun Roe
 * @date 20 October, 2008
 **/
 
//this package
#include "SCT_Cabling/SCT_CablingSvc.h"
#include "SCT_Cabling/ISCT_FillCabling.h"
#include "SCT_CablingUtilities.h"

//Indet
#include "InDetIdentifier/SCT_ID.h"
#include "Identifier/Identifier.h"

//Gaudi includes
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "EventInfo/EventIncident.h"
#include "StoreGate/StoreGateSvc.h"

//constants in file scope
static const std::string coracool("CORACOOL");
static const std::string coolVectorPayload("COOLVECTOR");
static const std::string coracoolFiller("SCT_FillCablingFromCoraCool");
static const std::string coolVectorFiller("SCT_FillCablingFromCoraCool");
static const std::string file("SCT_Sept08Cabling_svc.dat");
static const std::string textFiller("SCT_FillCablingFromText");
static const std::string defaultSource(coracool);
//invalid identifiers to return in case of error
static const SCT_OnlineId invalidId;
static const IdentifierHash invalidHash;
static const SCT_SerialNumber invalidSn;

//Utility functions at file scope
namespace{
  //make a number even
  IdentifierHash even(const IdentifierHash &hash){
    return (hash>>1) << 1;
  }
 
}

// Constructor
SCT_CablingSvc::SCT_CablingSvc( const std::string & name, ISvcLocator * pSvcLocator):
AthService(name,pSvcLocator), m_cablingFiller("ISCT_FillCabling",name), m_hashEntries(0),m_idHelper(0),m_usingDatabase(true){
     declareProperty("DataSource", m_cablingDataSource=defaultSource);
  m_onlineId2HashArray.fill(invalidHash);
  m_hash2OnlineIdArray.fill(invalidId);
}

//
void 
SCT_CablingSvc::handle(const Incident& runIncident) {
  ATH_MSG_INFO("Cabling event handler called.");
  if (runIncident.type()==IncidentType::BeginRun){
    if (not empty()) {
      ATH_MSG_INFO("Cabling data was already filled");
    } else {
      if (m_cablingFiller->fillMaps(this).isFailure()) msg(MSG::FATAL)<<"Callback to fill the cabling FAILED"<<endreq;
    }
  } else if (runIncident.type()==IncidentType::EndRun){
    m_onlineId2HashArray.fill(invalidHash);
    m_hash2OnlineIdArray.fill(invalidId);
    m_sn2HashMap.clear();
    m_rodIdSet.clear();
    m_hashEntries=0;
    msg(MSG::INFO)<<"Cabling has been emptied"<<endreq;
  }
}


//
StatusCode
SCT_CablingSvc::initialize(){
  StatusCode sc(StatusCode::FAILURE);
  ATH_MSG_INFO("Initialize SCT cabling "<<PACKAGE_VERSION);
  const std::string cablingDataSource = m_cablingDataSource.value();
  m_usingDatabase=(cablingDataSource == coracool) or (cablingDataSource == coolVectorPayload);
  //select which service name to use
  std::string cablingFillerName=coracoolFiller; //default to coracool
  if (cablingDataSource == coolVectorPayload) cablingFillerName = coolVectorFiller;
  if (not m_usingDatabase) cablingFillerName = textFiller;
  //
  m_cablingFiller=ServiceHandle<ISCT_FillCabling>(cablingFillerName,name());
  if (m_cablingFiller.retrieve().isSuccess()){
    if (m_cablingFiller->setDataSource(m_cablingDataSource.value()).isFailure()) return msg(MSG::FATAL)<<"Failed to find data"<<endreq,sc;
    // if the filler can do its job during initialisation, then do it...
    if (m_cablingFiller->canFillDuringInitialize()){
      sc=m_cablingFiller->fillMaps(this);
    } else { //... otherwise, register a handler to fill on the 'BeginRun' incident.
      ServiceHandle<IIncidentSvc> incidentSvc("IncidentSvc",name());
      sc = incidentSvc.retrieve();
      if (sc.isFailure() ) {
        ATH_MSG_ERROR("Failed to retrieve the IncidentSvc");
        return StatusCode::FAILURE;
      } else { 
        incidentSvc->addListener( this , IncidentType::BeginRun);
        incidentSvc->addListener( this , IncidentType::EndRun);
      }
    }
  }
  //
  ServiceHandle<StoreGateSvc> detStore("DetectorStore",name());
  ATH_CHECK(detStore.retrieve());
  ATH_CHECK(detStore->retrieve(m_idHelper,"SCT_ID"));
  return sc;
}

// 
StatusCode
SCT_CablingSvc::finalize(){
  msg(MSG::INFO)<<"Thank-you for using the SCT_CablingSvc, version "<<PACKAGE_VERSION<<endreq;
  msg(MSG::INFO)<<"The cabling data source was "<<m_cablingFiller->getDataSource()<<endreq;
  return StatusCode::SUCCESS;
}

//
StatusCode
SCT_CablingSvc::queryInterface(const InterfaceID & riid, void** ppvInterface ){
  if ( IIncidentListener::interfaceID().versionMatch(riid) ){
    *ppvInterface = dynamic_cast<IIncidentListener*>(this);
  } else if ( ISCT_CablingSvc::interfaceID().versionMatch(riid) ) {
      *ppvInterface = dynamic_cast<ISCT_CablingSvc*>(this);
  } else {
      // Interface is not directly available : try out a base class
      return AthService::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}

//
unsigned int 
SCT_CablingSvc::size() const{
  return m_hashEntries;
}

//
bool
SCT_CablingSvc::empty() const{
  return (m_hashEntries==0 );
}

//
IdentifierHash 
SCT_CablingSvc::getHashFromOnlineId(const SCT_OnlineId & onlineId, const bool withWarnings) {
  //is it valid at all?
  if (not onlineId.is_valid()){
    if (withWarnings) msg(MSG::WARNING)<<"Invalid online id ("<<std::hex<<onlineId<<") "<<std::dec<<endreq;
    return invalidHash;
  }
  //is it specifically valid for the given datasource?
  if (not onlineId.is_valid(m_usingDatabase)){
    const std::string alternative=m_usingDatabase?"text file cabling":"cabling from database";
    if (withWarnings) msg(MSG::WARNING)<<"Invalid online id ("<<std::hex<<onlineId<<") try using the "<<alternative<<std::dec<<endreq;
    return invalidHash;
  } 
  if (empty() and m_cablingFiller->fillMaps(this).isFailure() ) return msg(MSG::FATAL)<<"Filling the cabling FAILED"<<endreq, invalidHash;
  const unsigned int indx(onlineId.index());
  if (indx==SCT_OnlineId::INVALID_INDEX){
    ATH_MSG_WARNING("An invalid index was returned for the onlineId, so an InvalidHash is generated");
    return invalidHash;
  }
  return m_onlineId2HashArray[indx];
}

//
SCT_OnlineId 
SCT_CablingSvc::getOnlineIdFromHash(const IdentifierHash & hash) {
  if (empty() and m_cablingFiller->fillMaps(this).isFailure() ) return msg(MSG::FATAL)<<"Filling the cabling FAILED"<<endreq, invalidId;
  return hash.is_valid()? m_hash2OnlineIdArray[hash] : invalidId;
}

//
SCT_OnlineId
SCT_CablingSvc::getOnlineIdFromOfflineId(const Identifier & offlineId) {
  if (empty() and m_cablingFiller->fillMaps(this).isFailure() ) return msg(MSG::FATAL)<<"Filling the cabling FAILED"<<endreq, invalidId;
  if (not offlineId.is_valid()) return invalidId;
  IdentifierHash hash(m_idHelper->wafer_hash(offlineId));
  return getOnlineIdFromHash(hash);
}

//
IdentifierHash
SCT_CablingSvc::getHashFromSerialNumber(const SCT_SerialNumber & sn)  {
  if (empty() and m_cablingFiller->fillMaps(this).isFailure() ) return msg(MSG::FATAL)<<"Filling the cabling FAILED"<<endreq, invalidHash;
  if (not sn.isWellFormed()) return invalidHash;
  std::map<SCT_SerialNumber, IdentifierHash>::const_iterator it(m_sn2HashMap.find(sn));
  return (it == m_sn2HashMap.end()) ? invalidHash : it->second;
}


//This is an O(N) operation, but should only be used on insertion (or for debugging)
SCT_SerialNumber
SCT_CablingSvc::getSerialNumberFromHash(const IdentifierHash & hash)  {
  if (empty() and m_cablingFiller->fillMaps(this).isFailure() ) return msg(MSG::FATAL)<<"Filling the cabling FAILED"<<endreq, invalidSn;
  if (not hash.is_valid()) return invalidSn;
  //hash must be even
  IdentifierHash evenHash= even(hash);
  std::map<SCT_SerialNumber, IdentifierHash>::const_iterator it(m_sn2HashMap.begin());
  const std::map<SCT_SerialNumber, IdentifierHash>::const_iterator end(m_sn2HashMap.end());
  while ((it->second not_eq evenHash) and (it not_eq end)){ it++;}
  return (it == end)?invalidSn:it->first;
}

void
SCT_CablingSvc::getHashesForRod(std::vector<IdentifierHash> & usersVector, const std::uint32_t rodId){
  if (not SCT_OnlineId::rodIdInRange(rodId)){
    msg(MSG::WARNING)<<"Invalid rod id: "<<std::hex<<"0x"<<rodId<<std::dec<<" asked for associated hashes"<<endreq;
    return; //users vector remains unfilled
  }
  SCT_OnlineId firstPossibleId(rodId,SCT_OnlineId::FIRST_FIBRE);
  const bool withWarnings(false);
  for (SCT_OnlineId i(firstPossibleId);i!=SCT_OnlineId::INVALID_ONLINE_ID;++i){
    IdentifierHash thisHash(getHashFromOnlineId(i, withWarnings));
    if (thisHash != invalidHash){
      usersVector.push_back(thisHash);
    }
  }
}

//
bool 
SCT_CablingSvc::insert(const IdentifierHash & hash, const SCT_OnlineId & onlineId, const SCT_SerialNumber & sn){
  if (not sn.isWellFormed()) {
    msg(MSG::FATAL)<<"Serial number is not in correct format"<<endreq;
    return false;
  }
  if (not hash.is_valid()) return  msg(MSG::FATAL)<<"Invalid hash: "<<hash<<endreq, false;
  
  //compute the index for insertion to the onlineId/Hash array
  unsigned int indx(onlineId.index());
  if (m_onlineId2HashArray[indx].is_valid()) return false; //don't insert if theres already a valid hash there
  m_onlineId2HashArray[indx] = hash;
  if (m_hash2OnlineIdArray[hash].is_valid()) return false; //don't insert if there's already a valid online id there
  //insert to hash/onlineId array
  m_hash2OnlineIdArray[hash] = onlineId;
  //only insert even hashes for serial numbers
  IdentifierHash evenHash=even(hash);
  bool successfulInsert(m_sn2HashMap.insert(std::make_pair(sn, evenHash)).second);
  // in this form, the m_hashEntries will be half the number of hashes
  if ( successfulInsert) {
    m_rodIdSet.insert(onlineId.rod()); //move this here so insertion only happens for valid onlineId, hash
    ++m_hashEntries;
  }
  return true;
}


