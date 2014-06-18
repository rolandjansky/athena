/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <cassert>

#include "AthenaKernel/IResetable.h"
#include "AthenaKernel/getMessageSvc.h"
#include "GaudiKernel/MsgStream.h"

#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/IConversionSvc.h"
#include "GaudiKernel/GenericAddress.h"
#include "SGTools/TransientAddress.h"
#include "SGTools/T2pMap.h"
#include "SGTools/DataBucketBase.h"
#include <stdexcept>
using SG::TransientAddress;


#include "SGTools/DataProxy.h"
using SG::DataProxy;


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
		     IConversionSvc* svc,
		     bool constFlag, bool resetOnly):
  m_tAddress(tAddr),
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
  resetGaudiRef(m_dataLoader);
  resetGaudiRef(m_dObject);
  delete m_tAddress;
}

void DataProxy::setT2p(T2pMap* t2p)
{
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
  m_const = true;
  lock();
}

bool DataProxy::bindHandle(IResetable* ir) {
  if (ir->isSet()) {
    return false;
  } else {
    m_handles.push_back(ir); 
    return true;
  }
}



void DataProxy::reset()
{

  if (! m_handles.empty()) { resetBoundHandles(); }

  resetGaudiRef(m_dObject);
  m_tAddress->reset();
  m_const = m_origConst;

}

/// don't need a comment
void DataProxy::resetBoundHandles() {
  std::list<IResetable*>::iterator itre=m_handles.end();
  for (std::list<IResetable*>::iterator itr=m_handles.begin();
       itr != itre; ++itr) {
    (*itr)->reset();
  }
}

void DataProxy::unbindHandle(IResetable *ir) {
  if( m_handles.empty() ) { return; }
  m_handles.remove( ir );
}
  
/// return refCount
unsigned long DataProxy::refCount() const
{
  return m_refCount;
}

/// Add reference to object
unsigned long DataProxy::addRef()
{ 
  return ++m_refCount;
}

/// release reference to object
unsigned long DataProxy::release()
{ 
  unsigned long count(--m_refCount);
  if ( 0 == count ) delete this;
  return count;
}


///request to release the instance (may just reset it)
bool DataProxy::requestRelease(bool force /*= false*/) {
  bool canRelease = !isResetOnly() || force;
#ifndef NDEBUG
  MsgStream gLog(m_ims, "DataProxy");
  if (gLog.level() <= MSG::VERBOSE) {
    gLog << MSG::VERBOSE << "requestRelease(): "
	 << (canRelease ? " release " : " reset")
	 <<" object " 
	 << name() << " CLID " << clID() << " address " << MSG::hex
	 << object() << MSG::dec << endreq;
  }
#endif
  if (canRelease)  release();
    else             reset();
  return canRelease;
}

/// set a DataObject address
void DataProxy::setObject(DataObject* dObject)
{
  setGaudiRef(dObject, m_dObject);
  if (0 != m_dObject) {
    m_dObject->setRegistry(this);
    if (m_const) lock();
  }
}


// set IOpaqueAddress
void DataProxy::setAddress(IOpaqueAddress* address)
{
  m_tAddress->setAddress(address);
}
//////////////////////////////////////////////////////////////

/// Access DataObject on-demand using conversion service
DataObject* DataProxy::accessData()
{
  if (0 != m_dObject) return m_dObject;  // cached object

  if (0 == m_dataLoader) {
    //MsgStream gLog(m_ims, "DataProxy");
    //gLog << MSG::WARNING
    //	  << "accessData:  IConversionSvc ptr not set" <<endreq;
    m_errno=NOCNVSVC;
    setObject(0);
    return 0;   
  }
  if (!isValidAddress()) {
    MsgStream gLog(m_ims, "DataProxy");
    gLog << MSG::WARNING
	 << "accessData:  IOA pointer not set" <<endreq;
    m_errno=NOIOA;
    setObject(0);
    return 0;
  }

  DataObject* obj(0);
  StatusCode sc = m_dataLoader->createObj(m_tAddress->address(), obj);

  if ( sc.isSuccess() )
  {  
    setObject(obj);
    DataBucketBase* bucket = dynamic_cast<DataBucketBase*>(obj);
    if (bucket) {
      void* payload = bucket->object();
      m_t2p->t2pRegister(payload, this);
      m_errno=ALLOK;

  
      // Register bases as well.
      const SG::BaseInfoBase* bi = SG::BaseInfoBase::find (m_tAddress->clID());
      if (bi) {
        std::vector<CLID> base_clids = bi->get_bases();
        for (unsigned i=0; i < base_clids.size(); i++) {
          void* bobj = SG::DataProxy_cast (this, base_clids[i]);
          if (bobj && bobj != payload)
            m_t2p->t2pRegister (bobj, this);
        }
      }
    } else {
      MsgStream gLog(m_ims, "DataProxy");
      gLog << MSG::ERROR
	   << "accessData: ERROR registering object in t2p map" 
	   <<m_tAddress->clID() << '/' << m_tAddress->name() << '\n'
	   <<" Returning NULL DataObject pointer  " << endreq;
      obj=0; 
      setObject(0);
      m_errno=T2PREGFAILED;
      }
  } else {
    MsgStream gLog(m_ims, "DataProxy");
    gLog << MSG::WARNING 
	 << "accessData: conversion failed for data object " 
	 <<m_tAddress->clID() << '/' << m_tAddress->name() << '\n'
	 <<" Returning NULL DataObject pointer  " << endreq;
    obj=0; 
    setObject(0);
    m_errno=CNVFAILED;
  }

  return obj;

}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
bool DataProxy::isValidAddress() const
{
  return (0 != m_tAddress && m_tAddress->isValid());
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
  if (m_t2p)
    m_t2p->t2pRegister (p, this);
}


/**
 * @brief Lock the data object we're holding, if any.
 */
void DataProxy::lock()
{
  DataBucketBase* bucket = dynamic_cast<DataBucketBase*>(m_dObject);
  if (bucket)
    bucket->lock();
}

