/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_CablingSvc.cxx
 * Implementation file for SCT cabling service
 * @author Shaun Roe
 * @date 20 October, 2008
 **/
 
//this package
#include "SCT_CablingSvc.h"
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
namespace {
  //make a number even
  IdentifierHash even(const IdentifierHash& hash) {
    return (hash>>1) << 1;
  }
}

// Constructor
SCT_CablingSvc::SCT_CablingSvc(const std::string& name, ISvcLocator* pSvcLocator):
  AthService(name,pSvcLocator), m_cablingFiller("ISCT_FillCabling",name), m_idHelper(nullptr), m_usingDatabase(true) {
  declareProperty("DataSource", m_cablingDataSource=defaultSource);
}

//
void
SCT_CablingSvc::handle(const Incident& runIncident) {
  ATH_MSG_INFO("Cabling event handler called.");
  if (runIncident.type()==IncidentType::BeginRun) {
    if (not empty()) {
      ATH_MSG_INFO("Cabling data was already filled");
    } else {
      if (m_cablingFiller->fillMaps(this).isFailure()) {
        ATH_MSG_FATAL("Callback to fill the cabling FAILED");
      }
    }
  } else if (runIncident.type()==IncidentType::EndRun) {
    m_data.invalidate();
    ATH_MSG_INFO("Cabling has been emptied");
  }
}

//
StatusCode
SCT_CablingSvc::initialize() {
  StatusCode sc(StatusCode::FAILURE);
  ATH_MSG_INFO("Initialize SCT cabling " << PACKAGE_VERSION);
  const std::string cablingDataSource = m_cablingDataSource.value();
  m_usingDatabase=(cablingDataSource == coracool) or (cablingDataSource == coolVectorPayload);
  //select which service name to use
  std::string cablingFillerName=coracoolFiller; //default to coracool
  if (cablingDataSource == coolVectorPayload) cablingFillerName = coolVectorFiller;
  if (not m_usingDatabase) cablingFillerName = textFiller;
  //
  m_cablingFiller=ServiceHandle<ISCT_FillCabling>(cablingFillerName,name());
  if (m_cablingFiller.retrieve().isSuccess()) {
    if (m_cablingFiller->setDataSource(m_cablingDataSource.value()).isFailure()) {
      ATH_MSG_FATAL("Failed to find data");
      return sc;
    }
    // if the filler can do its job during initialisation, then do it...
    if (m_cablingFiller->canFillDuringInitialize()) {
      sc=m_cablingFiller->fillMaps(this);
    } else { //... otherwise, register a handler to fill on the 'BeginRun' incident.
      ServiceHandle<IIncidentSvc> incidentSvc("IncidentSvc",name());
      sc = incidentSvc.retrieve();
      if (sc.isFailure()) {
        ATH_MSG_FATAL("Failed to retrieve the IncidentSvc");
        return StatusCode::FAILURE;
      } else { 
        incidentSvc->addListener(this, IncidentType::BeginRun);
        incidentSvc->addListener(this, IncidentType::EndRun);
      }
    }
  }
  //
  ServiceHandle<StoreGateSvc> detStore("DetectorStore", name());
  ATH_CHECK(detStore.retrieve());
  ATH_CHECK(detStore->retrieve(m_idHelper, "SCT_ID"));
  return sc;
}

// 
StatusCode
SCT_CablingSvc::finalize() {
  ATH_MSG_INFO("Thank-you for using the SCT_CablingSvc, version "<<PACKAGE_VERSION);
  ATH_MSG_INFO("The cabling data source was "<<m_cablingFiller->getDataSource());
  return StatusCode::SUCCESS;
}

//
StatusCode
SCT_CablingSvc::queryInterface(const InterfaceID& riid, void** ppvInterface) {
  if (IIncidentListener::interfaceID().versionMatch(riid)) {
    *ppvInterface = dynamic_cast<IIncidentListener*>(this);
  } else if (ISCT_CablingSvc::interfaceID().versionMatch(riid)) {
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
SCT_CablingSvc::size() const {
  return m_data.getHashEntries();
}

//
bool
SCT_CablingSvc::empty() const {
  return (size()==0);
}

//
IdentifierHash 
SCT_CablingSvc::getHashFromOnlineId(const SCT_OnlineId& onlineId, const bool withWarnings) {
  //is it valid at all?
  if (not onlineId.is_valid()) {
    if (withWarnings) ATH_MSG_WARNING("Invalid online id ("<<std::hex<<onlineId<<") "<<std::dec);
    return invalidHash;
  }
  //is it specifically valid for the given datasource?
  if (not onlineId.is_valid(m_usingDatabase)) {
    const std::string alternative=m_usingDatabase?"text file cabling":"cabling from database";
    if (withWarnings) ATH_MSG_WARNING("Invalid online id ("<<std::hex<<onlineId<<") try using the "<<alternative<<std::dec);
    return invalidHash;
  }
  if (empty() and m_cablingFiller->fillMaps(this).isFailure()) {
    ATH_MSG_FATAL("Filling the cabling FAILED");
    return invalidHash;
  }

  return m_data.getHashFromOnlineId(onlineId);
}

//
SCT_OnlineId 
SCT_CablingSvc::getOnlineIdFromHash(const IdentifierHash& hash) {
  if (empty() and m_cablingFiller->fillMaps(this).isFailure()) {
    ATH_MSG_FATAL("Filling the cabling FAILED");
    return invalidId;
  }
  return m_data.getOnlineIdFromHash(hash);
}

//
SCT_OnlineId
SCT_CablingSvc::getOnlineIdFromOfflineId(const Identifier& offlineId) {
  if (empty() and m_cablingFiller->fillMaps(this).isFailure()) {
    ATH_MSG_FATAL("Filling the cabling FAILED");
    return invalidId;
  }
  if (not offlineId.is_valid()) return invalidId;
  IdentifierHash hash(m_idHelper->wafer_hash(offlineId));
  return getOnlineIdFromHash(hash);
}

//
IdentifierHash
SCT_CablingSvc::getHashFromSerialNumber(const SCT_SerialNumber& sn) {
  if (empty() and m_cablingFiller->fillMaps(this).isFailure()) {
    ATH_MSG_FATAL("Filling the cabling FAILED");
    return invalidHash;
  }
  if (not sn.isWellFormed()) return invalidHash;
  return m_data.getHashFromSerialNumber(sn);
}

SCT_SerialNumber
SCT_CablingSvc::getSerialNumberFromHash(const IdentifierHash& hash)  {
  if (empty() and m_cablingFiller->fillMaps(this).isFailure()) {
    ATH_MSG_FATAL("Filling the cabling FAILED");
    return invalidSn;
  }
  if (not hash.is_valid()) return invalidSn;
  //hash must be even
  IdentifierHash evenHash{even(hash)};
  return m_data.getSerialNumberFromHash(hash);
}

void
SCT_CablingSvc::getHashesForRod(std::vector<IdentifierHash>& usersVector, const std::uint32_t rodId) {
  if (not SCT_OnlineId::rodIdInRange(rodId)) {
    ATH_MSG_WARNING("Invalid rod id: "<<std::hex<<"0x"<<rodId<<std::dec<<" asked for associated hashes");
    return; //users vector remains unfilled
  }
  SCT_OnlineId firstPossibleId(rodId,SCT_OnlineId::FIRST_FIBRE);
  const bool withWarnings(false);
  for (SCT_OnlineId i(firstPossibleId);i!=SCT_OnlineId::INVALID_ONLINE_ID;++i) {
    IdentifierHash thisHash(getHashFromOnlineId(i, withWarnings));
    if (thisHash != invalidHash) {
      usersVector.push_back(thisHash);
    }
  }
}

//
bool
SCT_CablingSvc::insert(const IdentifierHash& hash, const SCT_OnlineId& onlineId, const SCT_SerialNumber& sn) {
  if (not sn.isWellFormed()) {
    ATH_MSG_FATAL("Serial number is not in correct format");
    return false;
  }
  if (not hash.is_valid()) {
    ATH_MSG_FATAL("Invalid hash: "<<hash);
    return false;
  }
  
  if (not m_data.setHashForOnlineId(hash, onlineId)) return false;
  if (not m_data.setOnlineIdForHash(onlineId, hash)) return false;

  //only insert even hashes for serial numbers
  IdentifierHash evenHash{even(hash)};
  bool successfulInsert{m_data.setHashForSerialNumber(evenHash, sn)};
  successfulInsert &= m_data.setSerialNumberForHash(sn, evenHash);
  // in this form, the m_data.getHashEntries() will be half the number of hashes
  if (successfulInsert) {
    m_data.setRod(onlineId.rod()); //move this here so insertion only happens for valid onlineId, hash
  }
  return true;
}
