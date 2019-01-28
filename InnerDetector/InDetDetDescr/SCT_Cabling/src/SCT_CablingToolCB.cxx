/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_CablingToolCB.cxx
 * Implementation file for SCT cabling tool using call back
 * @author Shaun Roe
 * @date 20 October, 2008
 **/
 
//this package
#include "SCT_CablingToolCB.h"
#include "SCT_Cabling/ISCT_FillCabling.h"
#include "SCT_CablingUtilities.h"

//Indet
#include "InDetIdentifier/SCT_ID.h"
#include "Identifier/Identifier.h"

//Gaudi includes
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/IIncidentSvc.h"

//constants in file scope
static const std::string coracool("CORACOOL");
static const std::string coolVectorPayload("COOLVECTOR");
static const std::string file("SCT_Sept08Cabling_svc.dat");
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
SCT_CablingToolCB::SCT_CablingToolCB(const std::string& type, const std::string& name, const IInterface* parent) :
  base_class(type, name, parent), m_idHelper(nullptr), m_usingDatabase(true) {
  declareProperty("DataSource", m_cablingDataSource=defaultSource);
}

//
void
SCT_CablingToolCB::handle(const Incident& runIncident) {
  ATH_MSG_INFO("Cabling event handler called.");
  if (runIncident.type()==IncidentType::BeginRun) {
    if (not empty()) {
      ATH_MSG_INFO("Cabling data was already filled");
    } else {
      if (not m_usingDatabase) {
        m_data = m_cablingFillerText->getMaps();
      } else {
        m_data = m_cablingFillerCoraCool->getMaps();
      }
    }
  } else if (runIncident.type()==IncidentType::EndRun) {
    m_data.invalidate();
    ATH_MSG_INFO("Cabling has been emptied");
  }
}

//
StatusCode
SCT_CablingToolCB::initialize() {
  ATH_MSG_INFO("Initialize SCT cabling " << PACKAGE_VERSION);
  const std::string cablingDataSource = m_cablingDataSource.value();
  m_usingDatabase=(cablingDataSource == coracool) or (cablingDataSource == coolVectorPayload) or (cablingDataSource == file);
  //select which service name to use
  bool filled{false};
  if (not m_usingDatabase) {
    m_cablingFillerCoraCool.disable();
    ATH_CHECK(m_cablingFillerText.retrieve());
    ATH_CHECK(m_cablingFillerText->setDataSource(m_cablingDataSource.value()));
    if (m_cablingFillerText->canFillDuringInitialize()) {
      m_data = m_cablingFillerText->getMaps();
      filled = true;
    }
  } else {
    m_cablingFillerText.disable();
    ATH_CHECK(m_cablingFillerCoraCool.retrieve());
    ATH_CHECK(m_cablingFillerCoraCool->setDataSource(m_cablingDataSource.value()));
    if (m_cablingFillerCoraCool->canFillDuringInitialize()) {
      m_data = m_cablingFillerCoraCool->getMaps();
      filled = true;
    }
  }

  if (not filled) { // register a handler to fill on the 'BeginRun' incident.
    ServiceHandle<IIncidentSvc> incidentSvc("IncidentSvc", name());
    ATH_CHECK(incidentSvc.retrieve());
    const long priority(1); // Default is 0. To be called before RegSelSvc, a value slightly higher than the default value is necessary.
    incidentSvc->addListener(this, IncidentType::BeginRun, priority);
    incidentSvc->addListener(this, IncidentType::EndRun);
  }
  //
  ATH_CHECK(detStore()->retrieve(m_idHelper, "SCT_ID"));
  return StatusCode::SUCCESS;
}

// 
StatusCode
SCT_CablingToolCB::finalize() {
  ATH_MSG_INFO("Thank-you for using the SCT_CablingToolCB, version "<<PACKAGE_VERSION);
  ATH_MSG_INFO("The cabling data source was "<< ((not m_usingDatabase) ? m_cablingFillerText->getDataSource() : m_cablingFillerCoraCool->getDataSource()));
  return StatusCode::SUCCESS;
}

//
unsigned int
SCT_CablingToolCB::size() const {
  return m_data.getHashEntries();
}

//
bool
SCT_CablingToolCB::empty() const {
  return (size()==0);
}

//
IdentifierHash 
SCT_CablingToolCB::getHashFromOnlineId(const SCT_OnlineId& onlineId, const bool withWarnings) const {
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

  return m_data.getHashFromOnlineId(onlineId);
}

//
SCT_OnlineId 
SCT_CablingToolCB::getOnlineIdFromHash(const IdentifierHash& hash) const {
  return m_data.getOnlineIdFromHash(hash);
}

//
SCT_OnlineId
SCT_CablingToolCB::getOnlineIdFromOfflineId(const Identifier& offlineId) const {
  if (not offlineId.is_valid()) return invalidId;
  IdentifierHash hash(m_idHelper->wafer_hash(offlineId));
  return getOnlineIdFromHash(hash);
}

//
IdentifierHash
SCT_CablingToolCB::getHashFromSerialNumber(const SCT_SerialNumber& sn) const {
  if (not sn.isWellFormed()) return invalidHash;
  return m_data.getHashFromSerialNumber(sn);
}

SCT_SerialNumber
SCT_CablingToolCB::getSerialNumberFromHash(const IdentifierHash& hash) const {
  if (not hash.is_valid()) return invalidSn;
  //hash must be even
  IdentifierHash evenHash{even(hash)};
  return m_data.getSerialNumberFromHash(evenHash);
}

void
SCT_CablingToolCB::getHashesForRod(std::vector<IdentifierHash>& usersVector, const std::uint32_t rodId) const {
  SCT_OnlineId firstPossibleId(rodId,SCT_OnlineId::FIRST_FIBRE);
  const bool withWarnings(false);
  for (SCT_OnlineId i(firstPossibleId);i!=SCT_OnlineId::INVALID_ONLINE_ID;++i) {
    IdentifierHash thisHash(getHashFromOnlineId(i, withWarnings));
    if (thisHash != invalidHash) {
      usersVector.push_back(thisHash);
    }
  }
}
