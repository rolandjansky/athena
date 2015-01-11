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
#include "GaudiKernel/System.h"
#include "AthenaKernel/IDictLoaderSvc.h"
#include "AthenaKernel/ITPCnvBase.h"
#include "AthenaKernel/ITPCnvSvc.h"

// AthenaRootKernel includes
#include "AthenaRootKernel/IRootSvc.h"

namespace Athena {

RootCnvSvc::RootCnvSvc(const std::string& name, ISvcLocator* pSvcLocator) :
	::AthCnvSvc(name, pSvcLocator, ROOT_StorageType),
	m_dictsvc("AthDictLoaderSvc", name),
	m_tpcnvsvc("AthTPCnvSvc", name),
	m_rootsvc("Athena::RootSvc/AthenaRootSvc", name),
	m_iosvc  ("AthIoSvc", name)
{}

RootCnvSvc::~RootCnvSvc()
{}

StatusCode
RootCnvSvc::initialize()
{
  ATH_MSG_INFO("Initializing " << name() << " - package version " << PACKAGE_VERSION);
  if (!::AthCnvSvc::initialize().isSuccess()) {
    ATH_MSG_FATAL("Cannot initialize ConversionSvc base class.");
    return StatusCode::FAILURE;
  }
  // Get the Dictionary service
  ATH_CHECK(m_dictsvc.retrieve());
  // Get the T/P conversion service
  ATH_CHECK(m_tpcnvsvc.retrieve());
  // Get the Root service
  ATH_CHECK(m_rootsvc.retrieve());
  // Get the I/O service
  ATH_CHECK(m_iosvc.retrieve());
#if ROOT_VERSION_CODE < ROOT_VERSION(5,99,0)
  // Initialize AthenaRootStreamerSvc
  ServiceHandle<IService> arssvc("AthenaRootStreamerSvc", this->name());
  ATH_CHECK(arssvc.retrieve());
#endif
  return StatusCode::SUCCESS;
}

StatusCode
RootCnvSvc::finalize()
{
  return ::AthCnvSvc::finalize();
}

StatusCode
RootCnvSvc::queryInterface(const InterfaceID& riid, void** ppvInterface)
{
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
StatusCode
RootCnvSvc::connectOutput(const std::string& file,
                          const std::string& mode)
{
  IIoSvc::Fd fd = m_iosvc->fd(file);
  if (fd==-1) {
    ATH_MSG_ERROR("no file [" << file << "] known to the i/o svc");
    return StatusCode::FAILURE;
  }
  return connectOutput(fd, IIoSvc::IoTypeFromName(mode));
}

/// Connect the output file to the service.
StatusCode
RootCnvSvc::connectOutput(const std::string& file)
{
  IIoSvc::Fd fd = m_iosvc->fd(file);
  if (fd==-1) {
    ATH_MSG_ERROR("no file [" << file << "] known to the i/o svc");
    return StatusCode::FAILURE;
  }
  return connectOutput(fd, IIoSvc::UPDATE);
}

/// Commit pending output.
StatusCode
RootCnvSvc::commitOutput(const std::string& file, bool do_commit)
{
  IIoSvc::Fd fd = m_iosvc->fd(file);
  if (fd==-1) {
    ATH_MSG_ERROR("no file [" << file << "] known to the i/o svc");
    return StatusCode::FAILURE;
  }
  return commitOutput(fd, do_commit);
}

StatusCode
RootCnvSvc::connectOutput(IIoSvc::Fd out, IIoSvc::IoType mode)
{
  ATH_MSG_VERBOSE("RootCnvSvc::connectOutput outputFile = "
                  << out << ", openMode = " << IIoSvc::IoTypeName(mode));
  return this->connectOutput(out);
}

StatusCode
RootCnvSvc::connectOutput(IIoSvc::Fd out)
{
  ATH_MSG_VERBOSE("RootCnvSvc::connectOutput output = " << out);
  if (!m_rootsvc->connect(out)) {
    ATH_MSG_ERROR("Unable to open an UPDATE transaction.");
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

StatusCode
RootCnvSvc::commitOutput(IIoSvc::Fd out, bool do_commit)
{
  ATH_MSG_VERBOSE("RootCnvSvc::commitOutput output = " << out
                  << " do-commit=" << do_commit);
  return m_rootsvc->commitOutput();
}

StatusCode
RootCnvSvc::createAddress(long svc_type,
                          const CLID& clid,
                          const std::string* par,
                          const unsigned long* ip,
                          IOpaqueAddress*& refpAddress)
{
  ATH_MSG_VERBOSE("RootCnvSvc::createAddress("
                  << "svc_type=" << svc_type << ", "
                  << "clid=" << clid << ", "
                  << "par=" << par[0] << ", " << par[1] << " "
                  << "ip=" << ip[0] << ", " << ip[1] << " "
                  << "refpaddr=" << refpAddress << ")...");
  refpAddress = new GenericAddress(ROOT_StorageType, clid,
                                   par[0], par[1], ip[0], ip[1]);
  return StatusCode::SUCCESS;
}

StatusCode
RootCnvSvc::convertAddress(const IOpaqueAddress* pAddress,
                           std::string& refAddress)
{
  ATH_MSG_VERBOSE("::convertAddress(paddr=" << pAddress
                  << ", refpaddr=" << refAddress << ")...");
  return StatusCode::FAILURE;
}

StatusCode
RootCnvSvc::createAddress(long svc_type,
                          const CLID& clid,
                          const std::string& refAddress,
                          IOpaqueAddress*& refpAddress)
{
  ATH_MSG_VERBOSE("RootCnvSvc::createAddress("
                  << "svc_type=" << svc_type << ", "
                  << "clid=" << clid << ", "
                  << "refaddr=" << refAddress << ", "
                  << "refpaddr=" << refpAddress << ")...");

  return StatusCode::FAILURE;
}

StatusCode
RootCnvSvc::updateServiceState(IOpaqueAddress* pAddress)
{
  ATH_MSG_VERBOSE("RootCnvSvc::updateServiceState(paddr=" << pAddress
                  << ")...");
  return StatusCode::FAILURE;
}

TClass*
RootCnvSvc::getClass(const CLID& clid) const
{
  return m_rootsvc->getClass(m_dictsvc->load_type(clid).TypeInfo());
}

TClass*
RootCnvSvc::getClass(const std::type_info& type) const
{
  return m_rootsvc->getClass(m_dictsvc->load_type(type).TypeInfo());
}

ITPCnvBase*
RootCnvSvc::getTPConverter(const CLID& clid) const
{
  ATH_MSG_VERBOSE("RootCnvSvc::getTPConverter clid = " << clid);
  return m_tpcnvsvc->t2p_cnv(clid);
}

unsigned long
RootCnvSvc::writeObject(const std::string& placement,
                        TClass* classDesc,
                        const void* pObj,
                        ITPCnvBase* tpConverter)
{
  unsigned long len = 0;
  if (tpConverter == 0) {
    len = m_rootsvc->writeObject(placement, classDesc, pObj);
  } else {
    void* pers = m_rootsvc->createObject(classDesc);
    tpConverter->transToPersUntyped(pObj, pers, this->msg());
    len = m_rootsvc->writeObject(placement, classDesc, pers);
    m_rootsvc->destructObject(classDesc, pers); pers = 0;
  }
  return len;
}

///@{ RootType-based API
/// Load the class (dictionary) from Root.
RootType
RootCnvSvc::getType(const CLID& clid) const
{
  return m_dictsvc->load_type(clid);
}

RootType
RootCnvSvc::getType(const std::type_info& type) const
{
  return m_dictsvc->load_type(type);
}

/// Write object of a given class to Root, using optional T/P converter.
unsigned long
RootCnvSvc::writeObject(const std::string& placement,
                        RootType typeDesc,
                        const void* pObj,
                        ITPCnvBase* tpConverter)
{
  unsigned long len = 0;
  if (tpConverter == 0) {
    len = m_rootsvc->writeObject(placement, typeDesc, pObj);
  } else {
    void* pers = m_rootsvc->createObject(typeDesc);
    tpConverter->transToPersUntyped(pObj, pers, this->msg());
    len = m_rootsvc->writeObject(placement, typeDesc, pers);
    m_rootsvc->destructObject(typeDesc, pers);
    pers = 0;
  }
  return len;
}
///@}

IConverter*
RootCnvSvc::createConverter(long typ,
                            const CLID& clid,
                            const ICnvFactory* fac)
{
  ATH_MSG_VERBOSE("createConverter typ = " << typ << ", clid = " << clid);
  if (typ == ROOT_StorageType) {
    return new Athena::RootCnv(clid, serviceLocator().get());
  }
  return ::AthCnvSvc::createConverter(typ, clid, fac);
}

} //> end namespace Athena
