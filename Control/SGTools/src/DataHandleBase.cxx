///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// DataHandleBase.cxx 
// Implementation file for class DataHandleBase
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 

// SGTools includes
#include "SGTools/DataHandleBase.h"

// STL includes



/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////

/// Default constructor: 
DataHandleBase::DataHandleBase() :
  IResetable(),
  m_itr(), 
  m_itrEnd(), 
  m_proxy(0),
  //m_ptr(0),
  m_useItr(false)
{}

/// Copy constructor: 
DataHandleBase::DataHandleBase( const DataHandleBase& rhs ) :
  IResetable(),
  m_itr(rhs.m_itr), 
  m_itrEnd(rhs.m_itrEnd),
  m_proxy(rhs.m_proxy), 
  //m_ptr(rhs.m_ptr),
  m_useItr(rhs.m_useItr)
{
  if (m_proxy) {
    m_proxy->addRef();
  }
}


/// Assignment operator: 
DataHandleBase& 
DataHandleBase::operator=( const DataHandleBase& rhs )
{
  if (this != &rhs) {
    if (m_proxy) {
      m_proxy->release();
    }

    m_itr =    rhs.m_itr;
    m_itrEnd = rhs.m_itrEnd;
    m_useItr = rhs.m_useItr;
    m_proxy =  rhs.m_proxy;
    //m_ptr =    rhs.m_ptr;
    if (m_proxy) {
      m_proxy->addRef();
    }
  }

  return *this;
}

/// Constructor with parameters: 
DataHandleBase::DataHandleBase(SG::DataProxy* proxy) :
  IResetable(),
  m_itr(), 
  m_itrEnd(), 
  m_proxy(proxy), 
  //m_ptr(0),
  m_useItr(false)
{
  if (m_proxy) {
    m_proxy->addRef();
  }
}

DataHandleBase::DataHandleBase(const SG::ConstProxyIterator& itr,
                               const SG::ConstProxyIterator& itrEnd) :
  IResetable(),
  m_itr(itr), 
  m_itrEnd(itrEnd),
  m_proxy(0), 
  //m_ptr(0),
  m_useItr(true)
{
  if (m_itr != m_itrEnd) {
    m_proxy = m_itr->second;
  }

  if (m_proxy) {
    m_proxy->addRef();
  }
}


/// Destructor: 
DataHandleBase::~DataHandleBase()
{
  if (m_proxy != 0) {
    m_proxy->unbindHandle(this);
    m_proxy->release();
  }
}

// Destructor
///////////////

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

StatusCode 
DataHandleBase::setState(SG::ConstProxyIterator &itr, 
                         const SG::ConstProxyIterator &itrEnd) const 
{
  if (m_proxy) m_proxy->release();

  m_itr = itr;
  m_itrEnd = itrEnd;
  m_useItr = true;

  // ouch! FIXME ?
  // we could have a void* m_ptr data member and have a
  // typeless_dataPointer method using the typeless machinery of
  // DataProxy+DataBucketBase...
  const_cast<DataHandleBase*>(this)->reset (false);

  // scan from itr to itrEnd and set m_itr to the first valid iterator:

  for (; itr != itrEnd; itr++) {
    if (itr->second->isValid()) {
      m_itr = itr;
      m_proxy = m_itr->second;
      if (m_proxy) m_proxy->addRef();
      return StatusCode::SUCCESS;
    }
  }

  m_itr = itrEnd;
  m_proxy = 0;
  
  return StatusCode::FAILURE; 
}

StatusCode 
DataHandleBase::setState(SG::DataProxy* proxy) const
{
  if (0 == proxy || !proxy->isValid()) {
    return StatusCode::FAILURE;
  }

  if (m_proxy != proxy) {
    if (m_proxy) m_proxy->release();
    m_proxy = proxy;
    m_proxy->addRef();
  }

  m_useItr = false;
  const_cast<DataHandleBase*>(this)->reset (false);

  return StatusCode::SUCCESS;
}

StatusCode 
DataHandleBase::setState(IProxyDict* store, const ID_type& key) const 
{
  if (0 == store) {
    return StatusCode::FAILURE;
  }
  CLID cid = this->clid();
  SG::DataProxy* proxy = store->proxy(cid, key);
  return this->setState(proxy);
}

const std::string& 
DataHandleBase::key() const
{
  if (m_itr == m_itrEnd) {
    return m_proxy->name();
  } else {
    return m_itr->first;
  }
}

bool 
DataHandleBase::isConst() const
{
  return 0 != m_proxy 
    ? m_proxy->isConst() 
    : false;
}


// A weaker test that
// *does not* retrieve the GaudiObject to check validity if not already done
bool 
DataHandleBase::isInitialized() const
{
  return (0 != m_proxy);
}

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

StatusCode 
DataHandleBase::setState(SG::DataProxy* proxy)
{
  if (0 == proxy || !proxy->isValid() || proxy->isConst()) {
    return StatusCode::FAILURE;
  }

  if (m_proxy != proxy) {
    if (m_proxy) m_proxy->release();
    m_proxy = proxy;
    m_proxy->addRef();
  }

  m_useItr = false;
  this->reset (false);
  return StatusCode::SUCCESS;
}

StatusCode 
DataHandleBase::setState(IProxyDict* store, const ID_type& key)
{
  if (0 == store) {
    return StatusCode::FAILURE;
  }
  CLID cid = this->clid();
  SG::DataProxy* proxy = store->proxy(cid, key);
  return this->setState(proxy);
}

/////////////////////////////////////////////////////////////////// 
// Protected methods: 
/////////////////////////////////////////////////////////////////// 


