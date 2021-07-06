///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// RootCnvSvc.cxx
// Implementation file for class Athena::RootCnvSvc
// Author: Peter van Gemmeren <gemmeren@anl.gov>
// Author: S.Binet<binet@cern.ch>
///////////////////////////////////////////////////////////////////

// AthenaRootComps includes
#include "RootCnvSvc.h"
#include "RootCnv.h"
#include "RootSvc.h"

// fwk includes
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/GenericAddress.h"
#include "AthenaKernel/IDictLoaderSvc.h"
#include "AthenaKernel/ITPCnvBase.h"
#include "AthenaKernel/ITPCnvSvc.h"
#include "PersistentDataModel/Placement.h"

// AthenaRootKernel includes
#include "AthenaRootKernel/IRootSvc.h"

namespace Athena {

RootCnvSvc::RootCnvSvc(const std::string& name, ISvcLocator* pSvcLocator) :
	::AthCnvSvc(name, pSvcLocator, ROOT_StorageType),
	m_dictSvc("AthDictLoaderSvc", name),
	m_tpCnvSvc("AthTPCnvSvc", name),
	m_rootSvc("Athena::RootSvc/AthenaRootSvc", name),
	m_treeName("CollectionTree") {
}

StatusCode RootCnvSvc::initialize() {
  ATH_MSG_INFO("Initializing " << name());
  if (!::AthCnvSvc::initialize().isSuccess()) {
    ATH_MSG_FATAL("Cannot initialize ConversionSvc base class.");
    return StatusCode::FAILURE;
  }
  // Get the Dictionary service
  ATH_CHECK(m_dictSvc.retrieve());
  // Get the T/P conversion service
  ATH_CHECK(m_tpCnvSvc.retrieve());
  // Get the Root service
  ATH_CHECK(m_rootSvc.retrieve());
  return StatusCode::SUCCESS;
}

StatusCode RootCnvSvc::finalize() {
  return ::AthCnvSvc::finalize();
}

StatusCode RootCnvSvc::queryInterface(const InterfaceID& riid, void** ppvInterface) {
  if (IAthenaRootCnvSvc::interfaceID().versionMatch(riid)) {
    *ppvInterface = dynamic_cast<IAthenaRootCnvSvc*>(this);
  } else {
    // Interface is not directly available: try out a base class
    return ::AthCnvSvc::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}

/// Connect the output file to the service with open mode.
StatusCode RootCnvSvc::connectOutput(const std::string& file, const std::string& mode) {
  std::string fileName = file;
  std::string::size_type inx1 = file.find('(');
  if (inx1 != std::string::npos) {
    std::string::size_type inx2 = file.find(')');
    if (inx2 == std::string::npos || inx2 != file.size() - 1) {
      return StatusCode::FAILURE;
    }
    m_treeName = fileName.substr(inx1 + 1, inx2 - inx1 - 1);
    fileName = fileName.substr(0, inx1);
  }
  if (!m_rootSvc->open(fileName, mode).isSuccess()) {
    ATH_MSG_ERROR("Could not open-recreate file [" << file << "]");
    return StatusCode::FAILURE;
  }
  return this->connectOutput(fileName);
}

/// Connect the output file to the service.
StatusCode RootCnvSvc::connectOutput(const std::string& file) {
  return m_rootSvc->connect(file);
}

/// Commit pending output.
StatusCode RootCnvSvc::commitOutput(const std::string& /*file*/, bool /*do_commit*/) {
  return m_rootSvc->commitOutput();
}

StatusCode RootCnvSvc::createAddress(long svc_type,
		const CLID& clid,
		const std::string* par,
		const unsigned long* ip,
		IOpaqueAddress*& refpAddress) {
  ATH_MSG_VERBOSE("RootCnvSvc::createAddress("
                  << "svc_type=" << svc_type << ", "
                  << "clid=" << clid << ", "
                  << "par=" << par[0] << ", " << par[1] << " "
                  << "ip=" << ip[0] << ", " << ip[1] << " "
                  << "refpaddr=" << refpAddress << ")");
  refpAddress = new GenericAddress(ROOT_StorageType, clid, par[0], par[1], ip[0], ip[1]);
  return StatusCode::SUCCESS;
}

StatusCode RootCnvSvc::convertAddress(const IOpaqueAddress* pAddress, std::string& refAddress) {
  ATH_MSG_VERBOSE("::convertAddress(pAddr = " << pAddress << ", refPaddr = " << refAddress << ")");
  return StatusCode::FAILURE;
}

StatusCode RootCnvSvc::createAddress(long svc_type,
		const CLID& clid,
		const std::string& refAddress,
		IOpaqueAddress*& refpAddress) {
  ATH_MSG_VERBOSE("RootCnvSvc::createAddress("
                  << "svc_type=" << svc_type << ", "
                  << "clid=" << clid << ", "
                  << "refaddr=" << refAddress << ", "
                  << "refpaddr=" << refpAddress << ")");

  return StatusCode::FAILURE;
}

StatusCode RootCnvSvc::updateServiceState(IOpaqueAddress* pAddress) {
  ATH_MSG_VERBOSE("RootCnvSvc::updateServiceState(paddr = " << pAddress << ")");
  return StatusCode::FAILURE;
}

///@{ RootType-based API
/// Load the class (dictionary) from Root.
RootType RootCnvSvc::getType(const CLID& clid) const {
  return m_dictSvc->load_type(clid);
}

RootType RootCnvSvc::getType(const std::type_info& type) const {
  return m_dictSvc->load_type(type);
}

/// Write object of a given class to Root, using optional T/P converter.
const Token* RootCnvSvc::writeObject(const std::string& key,
		const RootType& typeDesc,
		const void* pObj,
		ITPCnvBase* tpConverter) {
  const Token* token = 0;
  Placement placement;
  placement.setContainerName(m_treeName + "(" + key + ")");
  placement.setTechnology(ROOT_StorageType);
  if (tpConverter == 0) {
    token = m_rootSvc->writeObject(placement, typeDesc, pObj);
  } else {
    void* pers = m_rootSvc->createObject(typeDesc);
    tpConverter->transToPersUntyped(pObj, pers, this->msg());
    token = m_rootSvc->writeObject(placement, typeDesc, pers);
    m_rootSvc->destructObject(typeDesc, pers); pers = 0;
  }
  return token;
}

ITPCnvBase* RootCnvSvc::getTPConverter(const CLID& clid) const {
  ATH_MSG_VERBOSE("RootCnvSvc::getTPConverter clid = " << clid);
  return m_tpCnvSvc->t2p_cnv(clid);
}
///@}

IConverter* RootCnvSvc::createConverter(long typ, const CLID& clid, const ICnvFactory* fac) {
  ATH_MSG_VERBOSE("createConverter typ = " << typ << ", clid = " << clid);
  if (typ == ROOT_StorageType) {
    return new Athena::RootCnv(clid, serviceLocator().get());
  }
  return ::AthCnvSvc::createConverter(typ, clid, fac);
}

} //> end namespace Athena
