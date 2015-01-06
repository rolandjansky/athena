///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// VarHandleBase.cxx 
// Implementation file for class VarHandleBase
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 

#undef DEBUG_VHB  /* define to get verbose debug printouts */

#include "StoreGate/VarHandleBase.h"

#include "SGTools/DataBucketBase.h"
#include "SGTools/IProxyDictWithPool.h"
#include "SGTools/StorableConversions.h"

#include <algorithm> //> for std::swap

// fwk includes
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaKernel/getMessageSvc.h"

// #ifdef NDEBUG
// #undef NDEBUG
// #endif

namespace SG {

/// helper functions to bind ourselves to a DataProxy
bool 
VarHandleBase::bindToProxy(SG::VarHandleBase* v)
{
  // we need to set v's proxy to null in order for the 
  // DataProxy::bindHandle call to succeed: it checks if the to-be-bound
  // object is already set/bound...
  if (v->m_proxy) {
    SG::DataProxy *p = NULL;
    std::swap(v->m_proxy, p);
    bool o = p->bindHandle(v);
    std::swap(v->m_proxy, p);
    return o;
  }
  return false;
}

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////

/// Default constructor: 
VarHandleBase::VarHandleBase() :
  IResetable(),
  m_ptr(NULL),
  m_proxy(NULL),
  m_sgkey(""),
  m_store("")
{}

/// Copy constructor: 
VarHandleBase::VarHandleBase( const VarHandleBase& rhs ) :
  IResetable(),
  m_ptr(rhs.m_ptr),
  m_proxy(rhs.m_proxy),
  m_sgkey(rhs.m_sgkey),
  m_store(rhs.m_store)
{
  if (m_proxy) {
    m_proxy->addRef();
    bindToProxy(this);
  }
}


/// Assignment operator: 
VarHandleBase& 
VarHandleBase::operator=( const VarHandleBase& rhs )
{
  if (this != &rhs) {
    if (m_proxy) {
      m_proxy->release();
    }

    m_ptr =    rhs.m_ptr;
    m_proxy =  rhs.m_proxy;
    m_sgkey =  rhs.m_sgkey;
    m_store =  rhs.m_store;

    if (m_proxy) {
      m_proxy->addRef();
      bindToProxy(this);
    }

  }
  return *this;
}

VarHandleBase::VarHandleBase(const std::string& sgkey,
                             const std::string& storename) :
  IResetable(),
  m_ptr(NULL),
  m_proxy(NULL),
  m_sgkey(sgkey),
  m_store(storename)
{}

/// Destructor: 
VarHandleBase::~VarHandleBase()
{
  if (m_proxy != 0) {
    m_proxy->unbindHandle(this);
    m_proxy->release();
  }
  m_proxy = 0;
  m_ptr = 0;
}

// Destructor
///////////////

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

StatusCode 
VarHandleBase::setState() const
{
  //std::cerr << "::VHB::setState() const...\n"; 
  if (m_sgkey.empty() || m_store.empty()) {
    return StatusCode::FAILURE;
  }
  ServiceHandle<IProxyDictWithPool> hStore(m_store, "VarHandleBase");
  return (hStore.retrieve().isSuccess()) ? 
    this->setState(hStore->proxy(this->clid(), m_sgkey)):
    StatusCode::FAILURE;
}

StatusCode 
VarHandleBase::setState(SG::DataProxy* proxy) const
{
#ifdef DEBUG_HVB
   std::cerr << "::VHB::setState(" 
             << proxy 
             << " -- isValid: " << proxy->isValid()
             << " -- isConst: " << proxy->isConst()
             << ") const\n";
#endif
  if (0 == proxy || !proxy->isValid()) {
    return StatusCode::FAILURE;
  }

  SG::VarHandleBase *self = const_cast<SG::VarHandleBase*>(this);

  if (m_proxy != proxy) {
    if (m_proxy) m_proxy->release();
    m_proxy = proxy;
    m_proxy->addRef();
    bindToProxy(self);
  }

  self->reset();

  return StatusCode::SUCCESS;
}

StatusCode 
VarHandleBase::setState(IProxyDict* store, const std::string& key) const 
{
  if (0 == store) {
    return StatusCode::FAILURE;
  }
#ifdef DEBUG_VHB
  std::cerr << "::VHB::setState(" 
             << store->name() << ", "
             << key
             << ") const\n";
#endif
  CLID cid = this->clid();
  SG::DataProxy* proxy = store->proxy(cid, key);
  // std::cerr << "::VHB:: -- clid=[" << cid << "] proxy=[" << proxy << "]\n";
  return this->setState(proxy);
}

bool 
VarHandleBase::isConst() const
{
  return 0 != m_proxy 
    ? m_proxy->isConst() 
    : false;
}


// A weaker test that
// *does not* retrieve the GaudiObject to check validity if not already done
bool 
VarHandleBase::isInitialized() const
{
  return (0 != m_proxy);
}

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

StatusCode 
VarHandleBase::setState()
{
  // std::cerr << "::VHB::setState()...\n"; 
  if (m_sgkey.empty() || m_store.empty()) {
    return StatusCode::FAILURE;
  }

  typedef ServiceHandle<StoreGateSvc> Store_t;
  Store_t store(m_store, "VarHandle");
  if (!store.retrieve().isSuccess()) {
    return StatusCode::FAILURE;
  }

  SG::DataProxy * proxy = store->proxy(this->clid(), m_sgkey);
  return this->setState(proxy);
}

StatusCode 
VarHandleBase::setState(SG::DataProxy* proxy)
{
#ifdef DEBUG_VHB
  std::cerr << "::VHB::setState(" 
             << proxy 
             << " -- isValid: " << proxy->isValid()
             << " -- isConst: " << proxy->isConst()
             << ")\n";
#endif
  if (0 == proxy || !proxy->isValid() || proxy->isConst()) {
    return StatusCode::FAILURE;
  }

  if (m_proxy != proxy) {
    if (m_proxy) m_proxy->release();
    m_proxy = NULL;
    m_proxy = proxy;
    m_proxy->addRef();
    bindToProxy(this);
  }
  this->reset();
  return StatusCode::SUCCESS;
}

StatusCode 
VarHandleBase::setState(IProxyDict* store, const std::string& key)
{
  if (0 == store) {
    return StatusCode::FAILURE;
  }
#ifdef DEBUG_VHB
  std::cerr << "::VHB::setState(" 
           << store->name() << ", "
             << key
             << ")\n";
#endif
  CLID cid = this->clid();
  SG::DataProxy* proxy = store->proxy(cid, key);
  // std::cerr << "::VHB:: -- clid=[" << cid << "] proxy=[" << proxy << "]\n";
  return this->setState(proxy);
}

/////////////////////////////////////////////////////////////////// 
// Protected methods: 
/////////////////////////////////////////////////////////////////// 

#define SG_VARHANDLE_NAME "SG::VarHandleBase::typeless_dataPointer"
void* 
VarHandleBase::typeless_dataPointer_impl(bool
#ifndef NDEBUG
   quiet
#endif
) const
{
#ifdef DEBUG_VHB
   std::cerr << "::VHB::typeless_dataPointer("
             << "ptr=" << this->m_ptr << ", "
             << "proxy=" << this->m_proxy << ", "
             << "key=" <<this->m_sgkey
             << ")...\n";
#endif
  if (m_ptr) {
    return m_ptr;
  }

  m_ptr = 0;
  if (0 == m_proxy) {
    setState().ignore();
  }
  if (!m_proxy || !m_proxy->isValid()) {
    // invalid proxy
#ifndef NDEBUG
    if (!quiet) {
      MsgStream msg(Athena::getMessageSvc(), SG_VARHANDLE_NAME);
      if (msg.level() <= MSG::WARNING) {
	msg << MSG::WARNING 
	    << "this proxy " << MSG::hex << m_proxy
	    << MSG::dec 
	    << " [" << (m_proxy != 0 ? m_proxy->clID() : 0)
	    << "/" << (m_proxy != 0 
		       ? m_proxy->name() 
		       : std::string("<N/A>"))
	    << "] is in an invalid state" << endreq;
      }
    }
#endif
    return m_ptr;
  }

  DataObject* dobj = m_proxy->accessData();
  if (!dobj) {
    // invalid dobj
#ifndef NDEBUG
    if (!quiet) {
      MsgStream msg(Athena::getMessageSvc(), SG_VARHANDLE_NAME);
      if (msg.level() <= MSG::WARNING) {
	msg << MSG::WARNING 
	    << "this proxy " << MSG::hex << m_proxy
	    << MSG::dec << " has a NULL data object ptr" 
	    << endreq;
      }
    }
#endif
    return m_ptr;
  }

  const CLID clid = this->clid();
  m_ptr = SG::Storable_cast(dobj, clid, m_proxy);
  if (m_ptr) {
    return m_ptr;
  }

  // if m_ptr is null, probably the clid we gave wasn't the clid
  // the object was stored with, nor it inherits from it.
  // before giving up, let's check its transient CLIDs
  DataBucketBase *dbb = 0;
  if (m_proxy->transientAddress()->transientID(clid) &&
      0 != (dbb = dynamic_cast<DataBucketBase*>(dobj))) {
    // it is a symlink after all.
    // Let's hard cast (and keep our fingers Xed)
    m_ptr = static_cast<void*>(dbb->object());
  } else {
#ifndef NDEBUG
    if (!quiet) {
      MsgStream msg(Athena::getMessageSvc(), SG_VARHANDLE_NAME);
      if (msg.level() <= MSG::WARNING) {
	msg << MSG::WARNING 
	    << "Request for an invalid object; requested CLID = " 
	    << clid 
	    << ", proxy primary ID is " << m_proxy->clID() 
	    << endreq;
      }
    }
#endif
  } // try symlink -- endif
  return m_ptr;
}
#undef SG_VARHANDLE_NAME

// SG::DataProxy*
// VarHandleBase::retrieveProxyFromStore(CLID clid,
//                                       const std::string& name,
//                                       const std::string& store_name)
// {
//   ServiceHandle<IProxyDict> store(store_name, "SG::VarHandleBase");
//   return store->proxy(clid, name);
// }

} /* namespace SG */
