/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "CxxUtils/cPtrAccessSEGVHandler.h"

#include "PageAccessControlSvc.h"
PageAccessControlSvc::PageAccessControlSvc( const std::string& name, 
					    ISvcLocator* pSvcLocator ) : 
  AthService(name, pSvcLocator), m_saveSEGVaction(),
  m_accessControl(), m_SEGVHandler(m_accessControl)
{
  //pass m_SEGVHandler pointer to cPtrAccessSEGVHandler
  setPtrAccessSEGVHandler(&m_SEGVHandler);
  declareProperty("AutoMonitoring", m_autoMonitor=true, 
		  "start monitoring on initialize, stop on finalize");
}

bool PageAccessControlSvc::startMonitoring () {
  int rc = sigaction(SIGSEGV,nullptr, &m_saveSEGVaction);
  if (0==rc) {
    struct sigaction sa(m_saveSEGVaction);
    sa.sa_sigaction= cPtrAccessSEGVHandler; 
    sa.sa_flags=SA_SIGINFO;
    //off we go
    rc=sigaction(SIGSEGV,&sa,nullptr);
  }
  return (0==rc);
}

bool PageAccessControlSvc::stopMonitoring () {
  return (0 == sigaction(SIGSEGV,&m_saveSEGVaction, nullptr));
}

/// has this pointer been accessed (read/written)
bool PageAccessControlSvc::accessed(const void* address) const {
  return m_accessControl.accessed(address);
}

StatusCode PageAccessControlSvc::initialize() {
  StatusCode sc(StatusCode::SUCCESS);
  msg(MSG::INFO) << "Initializing " << name() << endmsg ;
  if (m_autoMonitor.value() && !this->startMonitoring()) sc = StatusCode::FAILURE;
  return sc;
}

StatusCode PageAccessControlSvc::finalize() {
  StatusCode sc(StatusCode::SUCCESS);
  if (m_autoMonitor.value()) {
    if (this->stopMonitoring()) this->report();
    else sc = StatusCode::FAILURE;
  }
  return sc;
}
// Query the interfaces.
//   Input: riid, Requested interface ID
//          ppvInterface, Pointer to requested interface
//   Return: StatusCode indicating SUCCESS or FAILURE.
// N.B. Don't forget to release the interface after use!!!
StatusCode
PageAccessControlSvc::queryInterface(const InterfaceID& riid, void** ppvInterface) 
{
    if ( IPageAccessControlSvc::interfaceID().versionMatch(riid) )    {
        *ppvInterface = (IPageAccessControlSvc*)this;
    }
    else  {
	// Interface is not directly available: try out a base class
	return Service::queryInterface(riid, ppvInterface);
    }
    this->addRef();
    return StatusCode::SUCCESS;
}


void PageAccessControlSvc::report() const {
  msg(MSG::INFO) << "Access monitoring report" << endmsg;
  PtrAccessSEGVHandler::const_iterator i(m_SEGVHandler.beginAccessedPtrs()),
    e(m_SEGVHandler.endAccessedPtrs());
  while (i != e) {
    msg(MSG::DEBUG) << "accessed pointer at @" << std::hex << *i++ << endmsg; 
  }
  PageAccessControl::const_iterator ia(m_accessControl.beginProtectedPtrs()),
    ea(m_accessControl.endProtectedPtrs());
  while (ia != ea) {
    msg(MSG::DEBUG) << "protected page at @" << std::hex << ia->addr
		    << " accessed "<< ia->restored << " times" << endmsg; 
    ++ia;
  }
}
