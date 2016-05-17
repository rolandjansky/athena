///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// RootSvc.cxx
// Implementation file for class Athena::RootSvc
// Author: Peter van Gemmeren <gemmeren@anl.gov>
///////////////////////////////////////////////////////////////////

// AthenaRootComps includes
#include "RootSvc.h"
#include "RootConnection.h"

// POOL/APR includes for Catalog
#include "FileCatalog/IFileCatalog.h"
#include "FileCatalog/IFCAction.h"

// fwk includes
#include "AthenaKernel/IDictLoaderSvc.h"

#include "PersistentDataModel/Token.h"
#include "PersistentDataModel/Placement.h"

namespace Athena {

RootSvc::RootSvc(const std::string& name, ISvcLocator* pSvcLocator) :
	::AthService(name, pSvcLocator),
	m_catalog(0),
	m_conns(),
	m_wconn(0),
	m_dictSvc("AthDictLoaderSvc", name) {
}

RootSvc::~RootSvc() {
  for (ConnMap_t::iterator itr = m_conns.begin(), iend = m_conns.end(); itr != iend; ++itr) {
    delete itr->second; itr->second = 0;
  }
  m_conns.clear();
}

StatusCode RootSvc::initialize() {
  ATH_MSG_INFO("Initializing " << name() << " - package version " << PACKAGE_VERSION);
  if (!::AthService::initialize().isSuccess()) {
    ATH_MSG_FATAL("Cannot initialize ConversionSvc base class.");
    return StatusCode::FAILURE;
  }
  m_catalog = new pool::IFileCatalog;
  try {
    m_catalog->setWriteCatalog("xmlcatalog_file:RootFileCatalog.xml"); // FIXME: Make config
    m_catalog->connect();
    m_catalog->start();
  } catch (std::exception& e) {
    ATH_MSG_FATAL ("Set up Catalog - caught exception: " << e.what());
    return StatusCode::FAILURE;
  }
  ATH_CHECK(m_dictSvc.retrieve());
  return StatusCode::SUCCESS;
}

StatusCode RootSvc::finalize() {
  for (ConnMap_t::const_iterator itr = m_conns.begin(), iend = m_conns.end(); itr != iend; ++itr) {
    if (!itr->second->disconnect().isSuccess()) {
      ATH_MSG_WARNING("Cannot disconnect file = " << itr->first.toString());
    }
  }
  if (m_catalog != 0) {
    m_catalog->commit();
    delete m_catalog; m_catalog = 0;
  }
  return ::AthService::finalize();
}

StatusCode RootSvc::queryInterface(const InterfaceID& riid, void** ppvInterface) {
  if (IRootSvc::interfaceID().versionMatch(riid)) {
    *ppvInterface = dynamic_cast<IRootSvc*>(this);
  } else {
    // Interface is not directly available: try out a base class
    return ::AthService::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}

/// Load the type (dictionary) from Root.
RootType RootSvc::getType(const std::type_info& type) const {
  return m_dictSvc->load_type(type);
}

/// Read object from Root.
void* RootSvc::readObject(const Token& /*token*/, void*& /*pObj*/) {
  return 0;
}

/// Write object of a given class to Root.
const Token* RootSvc::writeObject(const Placement& placement, const RootType& type, const void* pObj) {
  ATH_MSG_VERBOSE("RootSvc::writeObject pObj = " << pObj);
  if (m_wconn == 0) {
    ATH_MSG_ERROR("Cannot write without RootConnection for placement " << placement.containerName());
    return 0;
  }
  if (!m_wconn->setContainer(placement.containerName(), type.Name()).isSuccess()) {
    ATH_MSG_ERROR("Cannot set container [" << placement.containerName() << "]");
    return 0;
  }
  unsigned long ientry = 0;
  if (!m_wconn->write(pObj, ientry).isSuccess()) {
    ATH_MSG_ERROR("Cannot write Object to placement [" << placement.containerName() << "]");
    return 0;
  }
  return new Token();
}

/// Create an object of a given `RootType`.
void* RootSvc::createObject(const RootType& type) const {
  void* pObj = type.Construct();
  return pObj;
}

/// Destruct a given object of type `RootType`.
void RootSvc::destructObject(const RootType& /*type*/, void* /*pObj*/) const {
}

/// Open the file `fname` with open mode `mode`
StatusCode RootSvc::open(const std::string& fname, const std::string& /*mode*/) {
// Catalog to get fid...
  Guid fid = Guid::null();
  if (m_catalog != 0) {
    std::string fidString, ftype;
    pool::IFCAction action;
    m_catalog->setAction(action);
    action.lookupFileByPFN(fname, fidString, ftype);
    if (!fidString.empty()) {
      fid.fromString(fidString);
    } else {
      Guid::create(fid);
      fidString = fid.toString();
      action.registerPFN(fname, "ROOT_All", fidString);
    }
  }
  Athena::RootConnection* conn = 0;
  ConnMap_t::const_iterator fitr = m_conns.find(fid);
  if (fitr == m_conns.end()) {
    conn = new Athena::RootConnection(this, fname);
    m_conns.insert(std::make_pair(fid, conn));
  } else {
    conn = fitr->second;
  }
  if (conn == 0) {
    ATH_MSG_ERROR("Cannot get RootConnection for file " << fid.toString());
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

/// Connect the file `fname` to the service.
StatusCode RootSvc::connect(const std::string& fname) {
  ATH_MSG_VERBOSE("connect(" << fname << ")...");
  Athena::RootConnection* conn = this->connection(fname);
  if (conn == 0) {
    ATH_MSG_ERROR("No open RootConnection for file " << fname);
    return StatusCode::FAILURE;
  }
  if (!conn->connectWrite("recreate").isSuccess()) {
    ATH_MSG_ERROR("Cannot connect to file " << fname);
    return StatusCode::FAILURE;
  }
  m_wconn = conn;
  return StatusCode::SUCCESS;
}

StatusCode RootSvc::commitOutput() {
  ATH_MSG_VERBOSE("RootSvc::commitOutput");
  if (m_wconn == 0) {
    ATH_MSG_ERROR("Cannot commit without RootConnection.");
    return StatusCode::FAILURE;
  }
  if (!m_wconn->commit().isSuccess()) {
    ATH_MSG_ERROR("Cannot commit RootConnection.");
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

/// Disconnect the file `fname` from the service.
StatusCode RootSvc::disconnect(const std::string& fname) {
  ATH_MSG_VERBOSE("disconnect(" << fname << ")...");
  Athena::RootConnection* conn = this->connection(fname);
  if (conn == 0) {
    ATH_MSG_ERROR("No open RootConnection for file " << fname);
    return StatusCode::FAILURE;
  }
  if (!conn->disconnect().isSuccess()) {
    ATH_MSG_ERROR("Cannot disconnect to file " << fname);
    return StatusCode::FAILURE;
  }
  if (m_wconn == conn) {
    m_wconn = 0;
  }
  return StatusCode::SUCCESS;
}

/// Get the RootConnection associated with file `fname`
/// @returns NULL if no such file is known to this service
Athena::RootConnection* RootSvc::connection(const std::string& fname) {
// Catalog to get fid...
  Guid fid = Guid::null();
  if (m_catalog != 0) {
    std::string fidString, ftype;
    pool::IFCAction action;
    m_catalog->setAction(action);
    action.lookupFileByPFN(fname, fidString, ftype);
    if (!fidString.empty()) {
      fid.fromString(fidString);
    } else {
      Guid::create(fid);
      fidString = fid.toString();
      action.registerPFN(fname, "ROOT_All", fidString);
    }
  }
  Athena::RootConnection* conn = 0;
  ConnMap_t::const_iterator fitr = m_conns.find(fid);
  if (fitr != m_conns.end()) {
    conn = fitr->second;
  }
  return conn;
}

} //> namespace Athena
