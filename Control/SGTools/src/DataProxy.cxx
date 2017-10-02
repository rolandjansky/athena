/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <algorithm> 

#include <cassert>
#include <stdexcept>

#include "AthenaKernel/IResetable.h"
#include "AthenaKernel/getMessageSvc.h"

#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/IConverter.h"
#include "GaudiKernel/GenericAddress.h"
#include "GaudiKernel/MsgStream.h"

#include "SGTools/TransientAddress.h"
#include "SGTools/T2pMap.h"
#include "SGTools/DataBucketBase.h"
#include "SGTools/CurrentEventStore.h"
#include "AthenaKernel/IProxyDict.h"

#include "SGTools/DataProxy.h"
using SG::DataProxy;
using SG::TransientAddress;
using std::find;


namespace SG {
  typedef IProxyDict** getDataSourcePointerFunc_t (const std::string&);
  extern getDataSourcePointerFunc_t* getDataSourcePointerFunc;

  class DataProxyHolder
  {
  public:
    static void resetCachedSource();
  };
}

namespace {

class PushStore
{
public:
  PushStore (IProxyDict* store)
  {
    static std::string storeName = "StoreGateSvc";
    m_storePtr = (*SG::getDataSourcePointerFunc) (storeName);
    m_store = *m_storePtr;
    if (store && store != m_store) {
      *m_storePtr = store;
      SG::DataProxyHolder::resetCachedSource();
    }
  }

  ~PushStore()
  {
    if (*m_storePtr != m_store) {
      *m_storePtr  = m_store;
      SG::DataProxyHolder::resetCachedSource();
    }
  }

private:
  IProxyDict** m_storePtr;
  IProxyDict* m_store;
};

}


namespace {
  ///sets pMember to pgref (resetting it if pgref is 0). Handles Gaudi refcount
  template <class GAUDIREF>
  void setGaudiRef(GAUDIREF* pgref, GAUDIREF*& pMember) {
    if (0 != pgref) pgref->addRef();
    if (0 != pMember) pMember->release();
    pMember = pgref;
  }
  
  ///resets pMember. Handles Gaudi refcount
  template <class GAUDIREF>
  void resetGaudiRef(GAUDIREF*& pMember) { setGaudiRef((GAUDIREF*)0, pMember); }
  
} //end of unnamed namespace

// Default Constructor
DataProxy::DataProxy():
  m_tAddress(new TransientAddress()),
  m_refCount(0),
  m_dObject(0), 
  m_dataLoader(0),
  m_const(false),
  m_origConst(false),
  m_storageType(0),
  m_resetFlag(true),
  m_t2p(0),
  m_errno(ALLOK),
  m_store(0)
{ 
}

// DataProxy constructor with Transient Address
// (typically called from Proxy Provider)
DataProxy::DataProxy(TransientAddress* tAddr, 
		     IConverter* svc,
		     bool constFlag, bool resetOnly)
  : DataProxy (std::unique_ptr<TransientAddress> (tAddr),
               svc, constFlag, resetOnly)
{
}

// DataProxy constructor with Transient Address
// (typically called from Proxy Provider)
DataProxy::DataProxy(std::unique_ptr<TransientAddress> tAddr, 
		     IConverter* svc,
		     bool constFlag, bool resetOnly):
  m_tAddress(std::move(tAddr)),
  m_refCount(0),
  m_dObject(0), 
  m_dataLoader(svc),
  m_const(constFlag),
  m_origConst(constFlag),
  m_storageType(0),
  m_resetFlag(resetOnly),
  m_t2p(0),
  m_errno(ALLOK),
  m_store(0)
{
  //assert( tAddr->clID() != 0 );
  if (svc) svc->addRef();
}

// with Data Object:
// (typically called from a StoreGate record
DataProxy::DataProxy(DataObject* dObject, 
		     TransientAddress* tAddr,
		     bool constFlag, bool resetOnly):
  m_tAddress(tAddr),
  m_refCount(0),
  m_dObject(0), 
  m_dataLoader(0),
  m_const(constFlag),
  m_origConst(constFlag),
  m_storageType(0),
  m_resetFlag(resetOnly),
  m_t2p(0),
  m_errno(ALLOK),
  m_store(0)
{
  setObject(dObject);
}

// Destructor
DataProxy::~DataProxy()
{  
  finalReset();
}

void DataProxy::setT2p(T2pMap* t2p)
{
  lock_t lock (m_mutex);
  m_t2p = t2p;
}


/**
 * @brief Mark this object as const.  (Lock the object.)
 *
 * If the object held that derives from @c ILockable, then we also
 * call @c lock on the object.
 */
void DataProxy::setConst()
{
  objLock_t objLock (m_objMutex);
  lock_t lock (m_mutex);
  m_const = true;
  this->lock (objLock);
}

bool DataProxy::bindHandle(IResetable* ir) {
  assert(ir);
  lock_t lock (m_mutex);
  if (ir->isSet()) {
    return false;
  } else {
    m_handles.push_back(ir);
    if (m_store)
      m_store->boundHandle(ir);
    return true;
  }
}



void DataProxy::reset (bool hard /*= false*/)
{
  resetBoundHandles (hard);

  objLock_t objLock (m_objMutex);
  lock_t lock (m_mutex);
  DataObject* dobj = m_dObject;
  resetGaudiRef(dobj);
  m_dObject = dobj;
  m_tAddress->reset();
  m_const = m_origConst;
}

void DataProxy::finalReset()
{
  handleList_t handles;
  {
    objLock_t objLock (m_objMutex);
    lock_t lock (m_mutex);
    m_const=false; //hack to force the resetting of proxy ptr in VarHandleBase

    handles = m_handles;

    DataObject* dobj = m_dObject;
    resetGaudiRef(dobj);
    m_dObject = dobj;
    resetGaudiRef(m_dataLoader);
  }

  for (auto ih: handles) {
    if (0 != ih) ih->finalReset();
  }
}

/// don't need no comment
void DataProxy::resetBoundHandles (bool hard) {
  handleList_t handles;
  {
    lock_t lock (m_mutex);
    // Early exit if the list is empty.
    if (m_handles.empty()) return;

    // Remove empty entries.
    handleList_t::iterator it =
      std::remove (m_handles.begin(), m_handles.end(), nullptr);
    m_handles.erase (it, m_handles.end());
    if (m_handles.empty()) return;

    // Make a copy and drop the lock, so we're not holding the lock
    // during the callback.
    handles = m_handles;
  }

  for (IResetable* h : handles) {
    h->reset(hard);
  }
}

void DataProxy::unbindHandle(IResetable *ir) {
  assert(ir);
  lock_t lock (m_mutex);
  //  std::cout << "unbindHandle " << ir << std::endl;
  auto ifr = find(m_handles.begin(), m_handles.end(), ir );
  //reset the entry for ir instead of deleting it, so this can be called
  //within a m_handles loop
  if (ifr != m_handles.end()) {
    *ifr=0; 
    if (m_store)
      m_store->unboundHandle(ir);
  }
}
  
/// return refCount
unsigned long DataProxy::refCount() const
{
  lock_t lock (m_mutex);
  return m_refCount;
}

/// Add reference to object
unsigned long DataProxy::addRef()
{ 
  lock_t lock (m_mutex);
  return ++m_refCount;
}

/// release reference to object
unsigned long DataProxy::release()
{
  unsigned long count;
  {
    lock_t lock (m_mutex);
    count = --m_refCount;
  }
  if ( 0 == count ) delete this;
  return count;
}


///request to release the instance (may just reset it)
bool DataProxy::requestRelease(bool force, bool hard) {

  //this needs to happen no matter what
  resetBoundHandles(hard);

  bool canRelease = force;
  {
    lock_t lock (m_mutex);
    if (!m_resetFlag) canRelease = true;
  }

#ifndef NDEBUG
  MsgStream gLog(m_ims, "DataProxy");
  if (gLog.level() <= MSG::VERBOSE) {
    gLog << MSG::VERBOSE << "requestRelease(): "
	 << (canRelease ? " release " : " reset")
	 <<" object " 
	 << name() << " CLID " << clID() << " address " << MSG::hex
	 << object() << MSG::dec << endmsg;
  }
#endif
  if (canRelease)  release();
  else             reset(hard);
  return canRelease;
}

/// set a DataObject address
void DataProxy::setObject(objLock_t& objLock, DataObject* dObject)
{
  DataObject* dobj = m_dObject;
  setGaudiRef(dObject, dobj);
  m_dObject = dobj;
  if (0 != dobj) {
    dobj->setRegistry(this);
    if (m_const) this->lock (objLock);
  }
}


/// set a DataObject address
void DataProxy::setObject(DataObject* dObject)
{
  objLock_t objLock (m_objMutex);
  setObject (objLock, dObject);
}


// set IOpaqueAddress
void DataProxy::setAddress(IOpaqueAddress* address)
{
  lock_t lock (m_mutex);
  m_tAddress->setAddress(address);
}
//////////////////////////////////////////////////////////////


/**
 * @brief Read in a new copy of the object referenced by this proxy.
 * @param errNo If non-null, set to the resulting error code.
 *
 * If this proxy has an associated loader and address, then load
 * a new copy of the object and return it.  Any existing copy
 * held by the proxy is unaffected.
 *
 * This will fail if the proxy does not refer to an object read from an
 * input file.
 */
std::unique_ptr<DataObject> DataProxy::readData (objLock_t&, ErrNo* errNo) const
{
  if (errNo) *errNo = ALLOK;

  IConverter* dataLoader;
  IProxyDict* store;
  IOpaqueAddress* address;
  {
    lock_t lock (m_mutex);
    if (0 == m_dataLoader) {
      //MsgStream gLog(m_ims, "DataProxy");
      //gLog << MSG::WARNING
      //	  << "accessData:  IConverter ptr not set" <<endmsg;
      if (errNo) *errNo=NOCNVSVC;
      return nullptr;
    }
    if (!isValidAddress (lock)) {
      //MsgStream gLog(m_ims, "DataProxy");
      //gLog << MSG::WARNING
      //	 << "accessData:  IOA pointer not set" <<endmsg;
      if (errNo) *errNo=NOIOA;
      return nullptr;
    }

    dataLoader = m_dataLoader;
    store = m_store;
    address = m_tAddress->address();
  }

  SG::CurrentEventStore::Push push (m_store);

  DataObject* obj = nullptr;
  StatusCode sc;
  if (store)
    sc = store->createObj (dataLoader, address, obj);
  else
    sc = dataLoader->createObj (address, obj);
  if (sc.isSuccess())
    return std::unique_ptr<DataObject>(obj);
  if (errNo) *errNo = CNVFAILED;
  return nullptr;
}


/// Access DataObject on-demand using conversion service
DataObject* DataProxy::accessData()
{
  if (0 != m_dObject) return m_dObject;  // cached object

  objLock_t objLock (m_objMutex);

  if (isValidAddress()) {
    // An address provider called by isValidAddress may have set the object
    // pointer directly, rather than filling in the address.  So check
    // the cached object pointer again.
    if (0 != m_dObject) return m_dObject;  // cached object
  }

  std::unique_ptr<DataObject> obju = readData (objLock, &m_errno);
  if (!obju) {
    if (m_errno == NOIOA) {
      MsgStream gLog(m_ims, "DataProxy");
      gLog << MSG::WARNING
           << "accessData:  IOA pointer not set" <<endmsg;
    }
    else if (m_errno == CNVFAILED) {
      MsgStream gLog(m_ims, "DataProxy");
      gLog << MSG::WARNING 
           << "accessData: conversion failed for data object " 
           <<m_tAddress->clID() << '/' << m_tAddress->name() << '\n'
           <<" Returning NULL DataObject pointer  " << endmsg;
    }
    setObject(objLock, 0);
    return 0;   
  }

  DataObject* obj = obju.release();
  setObject(objLock, obj);
  DataBucketBase* bucket = dynamic_cast<DataBucketBase*>(obj);
  if (m_t2p) {
    if (bucket) {
      void* payload = bucket->object();
      m_t2p->t2pRegister(payload, this);
      m_errno=ALLOK;

      // Register bases as well.
      const SG::BaseInfoBase* bi = SG::BaseInfoBase::find (m_tAddress->clID());
      if (bi) {
        std::vector<CLID> base_clids = bi->get_bases();
        for (unsigned i=0; i < base_clids.size(); ++i) {
          void* bobj = SG::DataProxy_cast (this, base_clids[i]);
          if (bobj && bobj != payload)
            m_t2p->t2pRegister (bobj, this);
        }
      }
    }
    else {
      MsgStream gLog(m_ims, "DataProxy");
      gLog << MSG::ERROR
           << "accessData: ERROR registering object in t2p map" 
           <<m_tAddress->clID() << '/' << m_tAddress->name() << '\n'
           <<" Returning NULL DataObject pointer  " << endmsg;
      obj=0; 
      setObject(objLock, 0);
      m_errno=T2PREGFAILED;
    }
  }

  return obj;
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
bool DataProxy::isValidAddress (lock_t&) const
{
  return (0 != m_tAddress && m_tAddress->isValid(m_store));
}

bool DataProxy::isValidAddress() const
{
  lock_t lock (m_mutex);
  return isValidAddress (lock);
}

bool DataProxy::isValidObject() const
{
  // FIXME: should we try to chase?
  return (0!= m_dObject);
}
  
bool DataProxy::isValid() const
{
  return (isValidObject() || isValidAddress());
}


bool DataProxy::updateAddress()
{
  lock_t lock (m_mutex);
  return m_tAddress->isValid(m_store, true);
}

/**
 * @brief Try to get the pointer back from a @a DataProxy,
 *        converted to be of type @a clid.
 * @param proxy The @a DataProxy.
 * @param clid The ID of the class to which to convert.
 *
 * Only works if the held object is a @a DataBucket.
 * Returns 0 on failure,
 */
void* SG::DataProxy_cast (SG::DataProxy* proxy, CLID clid)
{
  if (0 == proxy || !proxy->isValid())
    return 0;
  DataObject* pObject = proxy->accessData();
  if (0 == pObject)
    return 0;
  return SG::Storable_cast (pObject, clid, proxy, proxy->isConst());
}

  
/**
 * @brief Register a transient object in a t2p map.
 * @param trans The object to register.
 */
void DataProxy::registerTransient (void* p)
{
  lock_t lock (m_mutex);
  if (m_t2p)
    m_t2p->t2pRegister (p, this);
}


/**
 * @brief Lock the data object we're holding, if any.
 *
 * Should be called with the mutex held.
 */
void DataProxy::lock (objLock_t&)
{
  DataObject* dobj = m_dObject;
  DataBucketBase* bucket = dynamic_cast<DataBucketBase*>(dobj);
  if (bucket)
    bucket->lock();
}

