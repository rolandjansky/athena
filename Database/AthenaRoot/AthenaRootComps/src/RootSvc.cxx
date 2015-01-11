///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// RootSvc.cxx
// Implementation file for class Athena::RootSvc
// Author: Peter van Gemmeren <gemmeren@anl.gov>
// Author: S.Binet<binet@cern.ch>
///////////////////////////////////////////////////////////////////

// AthenaRootComps includes
#include "RootSvc.h"
#include "RootConnection.h"

// fwk includes
#include "AthenaKernel/IDictLoaderSvc.h"

// ROOT includes
#include "TROOT.h"
#include "TClass.h"
#include "RVersion.h"

namespace Athena {

RootSvc::RootSvc(const std::string& name, ISvcLocator* pSvcLocator) :
  ::AthService(name, pSvcLocator),
  m_conns(),
  m_wconn(NULL),
  m_dictsvc("AthDictLoaderSvc", name),
  m_iosvc  ("IoSvc/AthIoSvc", name)
{}

RootSvc::~RootSvc()
{
  for (ConnMap_t::iterator
         itr  = m_conns.begin(),
         iend = m_conns.end();
       itr != iend;
       ++itr) {
    delete itr->second; itr->second = 0;
  }
  m_conns.clear();
}

StatusCode
RootSvc::initialize()
{
  ATH_MSG_INFO("Initializing " << name() << " - package version " << PACKAGE_VERSION);
  if (!::AthService::initialize().isSuccess()) {
    ATH_MSG_FATAL("Cannot initialize ConversionSvc base class.");
    return StatusCode::FAILURE;
  }
  ATH_CHECK(m_dictsvc.retrieve());
  ATH_CHECK(m_iosvc.retrieve());

  return StatusCode::SUCCESS;
}

StatusCode
RootSvc::finalize()
{
  for (ConnMap_t::const_iterator
         itr = m_conns.begin(),
         iend= m_conns.end();
       itr != iend;
       ++itr) {
    IIoSvc::Fd fd = itr->first;
    if (!itr->second->disconnect().isSuccess()) {
      ATH_MSG_WARNING("Cannot disconnect file = " << fd);
    } else if (!m_iosvc->close(fd).isSuccess()) {
      ATH_MSG_WARNING("cannot close file = " << fd);
    }
  }
  return ::AthService::finalize();
}

StatusCode
RootSvc::queryInterface(const InterfaceID& riid, void** ppvInterface)
{
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
RootType
RootSvc::getType(const std::type_info& type) const
{
  return m_dictsvc->load_type(type);
}

/// Create an object of a given `RootType`.
void*
RootSvc::createObject(const RootType type) const
{
#if ROOT_VERSION_CODE < ROOT_VERSION(5,99,0)
  void *pObj = type.Allocate();
#else
  void *pObj = type.Construct();
#endif
  return pObj;
}

/// Write object of a given class to Root.
unsigned long
RootSvc::writeObject(const std::string& placement,
                     const RootType type,
                     const void* pObj)
{
  ATH_MSG_VERBOSE("RootSvc::writeObject placement = "
                  << placement << ", pObj = " << pObj);
  if (m_wconn == 0) {
    ATH_MSG_ERROR("Cannot write without RootConnection for placement "
                  << placement);
    return 0;
  }
  if (!m_wconn->setContainer(placement,
			     type.Name(),
			     pObj).isSuccess()) {
    ATH_MSG_ERROR("Cannot set container [" << placement << "]");
    return 0;
  }
  unsigned long ientry = 0;
  if (!m_wconn->write(pObj, ientry).isSuccess()) {
    ATH_MSG_ERROR("Cannot write Object to placement [" << placement << "]");
    return 0;
  }
  return ientry;
}

/// Destruct a given object of type `RootType`.
void
#if ROOT_VERSION_CODE < ROOT_VERSION(5,99,0)
RootSvc::destructObject(RootType type, void* pObj) const
#else
RootSvc::destructObject(RootType /*type*/, void* /*pObj*/) const
#endif
{
#if ROOT_VERSION_CODE < ROOT_VERSION(5,99,0)
  type.Destruct(pObj);
#endif
}

TClass*
RootSvc::getClass(const std::type_info& type) const
{
  ATH_MSG_VERBOSE("RootSvc::getClass type = " << System::typeinfoName(type));
  return gROOT->GetClass(type);
}

void*
RootSvc::createObject(const TClass* classDesc) const
{
  ATH_MSG_VERBOSE("RootSvc::createObject type = " << classDesc->GetName());
  return classDesc->New();
}

unsigned long
RootSvc::writeObject(const std::string& placement,
                     const TClass* classDesc,
                     const void* pObj)
{
  ATH_MSG_VERBOSE("RootSvc::writeObject placement = " << placement << ", pObj = " << pObj);
  if (m_wconn == 0) {
    ATH_MSG_ERROR("Cannot write without RootConnection for placement [" 
                  << placement << "]");
    return 0;
  }
  if (!m_wconn->setContainer(placement,
                             classDesc->GetName(),
                             pObj).isSuccess()) {
    ATH_MSG_ERROR("Cannot set container [" << placement << "]");
    return 0;
  }
  unsigned long ientry = 0;
  if (!m_wconn->write(pObj, ientry).isSuccess()) {
    ATH_MSG_ERROR("Cannot write Object to placement [" << placement << "]");
    return 0;
  }
  return ientry;
}

void
RootSvc::destructObject(TClass* classDesc, void* pObj) const
{
  return classDesc->Destructor(pObj);
}

/// open the file `fname` with open mode `mode`
/// @returns the file descriptor or -1 if failure
IIoSvc::Fd
RootSvc::open(const std::string& fname, IIoSvc::IoType mode)
{
  IIoSvc::Fd fd = m_iosvc->open(fname, mode);
  if (fd == -1) {
    ATH_MSG_ERROR("Unable to open " << fname);
    return fd;
  }
  ConnMap_t::iterator ifd = m_conns.find(fd);
  if (ifd != m_conns.end()) {
    m_iosvc->close(fd);
    return -1;
  }
  return fd;
}

/// Connect the file descriptor `fd` to the service.
StatusCode
RootSvc::connect(IIoSvc::Fd fd)
{
  ATH_MSG_VERBOSE("::connect(" << fd << ")...");
  Athena::RootConnection *conn = NULL;
  ConnMap_t::const_iterator fitr = m_conns.find(fd);
  if (fitr == m_conns.end()) {
    conn = new Athena::RootConnection(this, fd);
    m_conns.insert(std::make_pair(fd, conn));
  } else {
    conn = fitr->second;
  }
  if (0 == conn) {
    ATH_MSG_ERROR("cannot get RootConnection for file ["
                  << m_iosvc->fname(fd) << "]"
                  << " (fd=" << fd << ")");
    return StatusCode::FAILURE;
  }
  if (!conn->connectWrite(m_iosvc->mode(fd)).isSuccess()) {
    ATH_MSG_ERROR("cannot connect to file ["
                  << m_iosvc->fname(fd) << "] mode=["
                  << m_iosvc->mode(fd) << "]");
    return StatusCode::FAILURE;
  }
  m_wconn = conn;
  return StatusCode::SUCCESS;
}

/// get the RootConnection associated with file descriptor `fd`
/// @returns NULL if no such file is known to this service
Athena::RootConnection*
RootSvc::connection(IIoSvc::Fd fd)
{
  Athena::RootConnection *conn = NULL;
  ConnMap_t::const_iterator fitr = m_conns.find(fd);
  if (fitr != m_conns.end()) {
    conn = fitr->second;
  }
  return conn;
}

/// get the connection (or create it) for file descriptor `fd`
Athena::RootConnection*
RootSvc::new_connection(IIoSvc::Fd fd)
{
  Athena::RootConnection *conn = connection(fd);
  if (NULL == conn) {
    conn = new Athena::RootConnection(this, fd);
    m_conns.insert(std::make_pair(fd, conn));
  }
  return conn;
}

StatusCode
RootSvc::commitOutput() {
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

/// Disconnect the file from the service.
StatusCode
RootSvc::disconnect(IIoSvc::Fd fd)
{
  const std::string& file = m_iosvc->fname(fd);
  ATH_MSG_VERBOSE("RootSvc::disconnect file = " << file);
  ConnMap_t::const_iterator entry = m_conns.find(fd);
  if (entry == m_conns.end()) {
    ATH_MSG_ERROR("Cannot get RootConnection for file " << file);
    return StatusCode::FAILURE;
  }
  if (!entry->second->disconnect().isSuccess()) {
    ATH_MSG_ERROR("Cannot disconnect to file " << file);
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

} //> namespace Athena
