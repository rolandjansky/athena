///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// RootCnv.cxx
// Implementation file for class Athena::RootCnv
// Author: Peter van Gemmeren <gemmeren@anl.gov>
// Author: S.Binet<binet@cern.ch>
///////////////////////////////////////////////////////////////////

// AthenaRootComps includes
#include "RootCnv.h"

// AthenaRootKernel includes
#include "AthenaRootKernel/IAthenaRootCnvSvc.h"
#include "AthenaRootKernel/IRootSvc.h"

// stl includes
#include <string>

// fwk includes
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiKernel/IRegistry.h"
#include "AthenaKernel/ITPCnvBase.h"
#include "AthenaBaseComps/AthCheckMacros.h"
#include "PersistentDataModel/Placement.h"
#include "PersistentDataModel/Token.h"

// sgtools
#include "SGTools/StorableConversions.h"

namespace {
  std::string name_from_clid(const CLID clid) {
    std::ostringstream o;
    o << "AthenaRootCnv_" << clid;
    return o.str();
  }
}

namespace Athena {

RootCnv::RootCnv(const CLID& id, ISvcLocator* pSvcLocator) : ::Converter(ROOT_StorageType, id, pSvcLocator),
		::AthMessaging((pSvcLocator != 0 ? msgSvc() : 0), ::name_from_clid(id)),
	m_cnvSvc("Athena::RootCnvSvc/AthenaRootCnvSvc", ::name_from_clid(id)),
	m_ttype(),
	m_ptype(),
	m_tpcnv(0) {
}

RootCnv::RootCnv(ISvcLocator* pSvcLocator) : ::Converter(ROOT_StorageType, CLID_NULL, pSvcLocator),
		::AthMessaging( msgSvc(), ::name_from_clid(CLID_NULL)),
	m_cnvSvc("Athena::RootCnvSvc/AthenaRootCnvSvc", ::name_from_clid(CLID_NULL)),
	m_ttype(),
	m_ptype(),
	m_tpcnv(0) {
}

RootCnv::~RootCnv() {
}

StatusCode RootCnv::initialize() {
  if (!::Converter::initialize().isSuccess()) {
    ATH_MSG_FATAL("Cannot initialize Converter base class.");
    return StatusCode::FAILURE;
  }
  // Get the AthenaRootCnvSvc
  ATH_CHECK(m_cnvSvc.retrieve());
  CLID clid = this->objType();
  m_tpcnv = m_cnvSvc->getTPConverter(clid);
  if (!m_ttype) {
    m_ttype = m_cnvSvc->getType(clid);
  }
  if (!m_ptype) {
    if (m_tpcnv == 0) {
      // only warn if not a builtin
      if (!m_ttype.IsFundamental()) {
        ATH_MSG_INFO("Cannot get T/P converter, will write transient type [" << m_ttype.Name() << ", clid = " << clid << "]");
      }
      m_ptype = m_cnvSvc->getType(clid);
    } else {
      m_ptype = m_cnvSvc->getType(m_tpcnv->persistentTInfo());
    }
    if (!m_ptype) {
      ATH_MSG_ERROR("Cannot get RootType for DataObject.");
      return StatusCode::FAILURE;
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode RootCnv::finalize() {
  return ::Converter::finalize();
}

long RootCnv::repSvcType() const {
  return ROOT_StorageType;
}

long RootCnv::storageType() {
  return ROOT_StorageType;
}

StatusCode RootCnv::createObj(IOpaqueAddress* pAddr, DataObject*& pObj) {
  ATH_MSG_DEBUG("createObj(" << pAddr << ", " << pObj << ")");
  /*
  Athena::RootBranchAddress* rba = dynamic_cast<Athena::RootBranchAddress*>(pAddr);
  if (!rba) {
    ATH_MSG_DEBUG(*pAddr->par() << " is NOT a GenericAddress!");
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("loading branch: [" << rba->par()[0] << "/" << rba->par()[1] << "]...");
  rba->setBranchAddress(m_type);
  ATH_MSG_DEBUG("loading branch: [" << rba->par()[0] << "/" << rba->par()[1] << "]... [done]");
  Athena::DataBucketBranch* dbb = new DataBucketBranch(rba->clID(), rba->m_ptr);
  pObj = dbb;
  return StatusCode::SUCCESS;
  */
  return StatusCode::FAILURE;
}

StatusCode RootCnv::fillObjRefs(IOpaqueAddress* pAddr, DataObject* pObj) {
  ATH_MSG_VERBOSE("fillObjRefs(pAddr = " << pAddr << ", " << "pObj = " << pObj << ")...");
  return StatusCode::FAILURE;
}

StatusCode RootCnv::createRep(DataObject* pObj, IOpaqueAddress*& pAddr) {
  ATH_MSG_VERBOSE("createRep pObj = " << pObj);
  pAddr = 0;
  if (pObj == 0) {
    ATH_MSG_ERROR("createRep called without DataObject.");
    return StatusCode::FAILURE;
  }
  CLID clid = pObj->clID();
  IRegistry* pR = pObj->registry();
  if (pR == 0) {
    ATH_MSG_ERROR("createRep no registry for DataObject.");
    return StatusCode::FAILURE;
  }
  std::string p[2] = { "", pR->identifier() };
  unsigned long ip[2] = { 0, 0 };
  void* trans = SG::fromStorable(pObj, clid);
  ATH_MSG_DEBUG("writing [" << objType() << "/" << p[1] << "]");
  ATH_MSG_DEBUG("p-type: [" << m_ptype.Name() << "]");
  std::unique_ptr<const Token> token (m_cnvSvc->writeObject(p[1], m_ptype, trans, m_tpcnv));
  if (token == 0 || !m_cnvSvc->createAddress(this->repSvcType(), clid, p, ip, pAddr).isSuccess()) {
    ATH_MSG_ERROR("problem creating opaque address");
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("writing [" << objType() << "/" << p[1] << "] [done], entry = " << ip[0]);
  return StatusCode::SUCCESS;
}

StatusCode RootCnv::fillRepRefs(IOpaqueAddress* pAddr, DataObject* pObj) {
  ATH_MSG_VERBOSE("fillRepRefs(pAddr = " << pAddr << ", " << "pObj = " << pObj << ")...");
  return StatusCode::FAILURE;
}

} //< end namespace Athena
