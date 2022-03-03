/* -*- C++ -*- */

/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/** @file DataHandle.h
 *  defines an "iterator" over instances of a given type in StoreGateSvc
 */
#ifndef STOREGATE_DATAHANDLE_H
#define STOREGATE_DATAHANDLE_H

#include "AthenaKernel/DataBucket.h"
#include "AthenaKernel/IResetable.h"
#include "AthenaKernel/IProxyDict.h"
#include "SGTools/ProxyMap.h"
#include "SGTools/DataProxy.h"
#include "SGTools/DataHandleBase.h"
#include "CxxUtils/checker_macros.h"
#include <iterator>
#include <iostream>

/** @class DataHandle
 * @brief an iterator over instances of a given type in StoreGateSvc. It d-casts
 * and caches locally the pointed-at object, to speed-up subsequent accesses.
 * It can be reset by the store for asynchronous updates (IOVSvc)
 *
 * Holds a reference count on the proxy to which it's currently pointing.
 * Note: one may think that we should hold reference counts for the
 * entire range which the DataHandle references (if it is a range).
 * The problem with this is that the range may change underneath us.
 * For example, if someone does a SG record, then a new element may appear
 * within the range.  So we only hold the refcount for the object
 * to which we're currently pointing.
 *
 * @param DATA the data object type
 * @author ATLAS Collaboration
 **/
template <typename DATA> 
class ATLAS_NOT_THREAD_SAFE DataHandle :
  public DataHandleBase
{
public:
  using iterator_category = std::forward_iterator_tag;
  using value_type = DATA;
  using difference_type = std::ptrdiff_t;
  using pointer = value_type*;
  using reference = value_type&;

  using pointer_type = pointer;
  using const_pointer_type = const DATA*;
  using reference_type = reference;
  using const_reference_type = const DATA&;

  typedef DataHandleBase::ID_type ID_type;

  /// \name constructors and assignment
  //@{
  DataHandle();
  DataHandle(const DataHandle& h);
  DataHandle& operator= (const DataHandle& h);
  DataHandle& operator= (const DATA& d) 
  { 
    typename DataHandle<DATA>::pointer_type ptr = this->ptr();
    if (ptr) {
      *ptr = d; 
    } else {
      std::cerr << "invalid proxy\n";
    }
    return *this; 
  }

  virtual ~DataHandle();  ///< unbind from the proxy before we go
  //@}

  /// \name validity checks
  //@{
  bool isValid() const; ///<RETRIEVES the DO to check it is valid and unlocked

  // FIXME op! is to keep backward compatibility with Gaudi
  // FIXME similar to checking the SmartDataPtr
  // FIXME dangerous stuff: remove!
  ///DEPRECATED for statements like:  if (!DataHandle\<XXX\>) {...} 
  bool operator !() const { return !isValid(); }

  //FIXME VERY dangerous stuff: remove!
  ///DEPRECATED for statements like:  if (DataHandle\<XXX\>) {...} 
  operator int() const  { return isValid(); }
  //@}

  /// \name iterator interface
  //@{
  const DataHandle& operator++ () const;        ///<prefix
  DataHandle operator++ (int) const;           ///<postfix
    
  const_pointer_type operator->() const   { return cptr(); }

  const_reference_type operator*() const    { return *cptr(); }   
  //@}

  /// \name access to the underlying ptr
  //@{
  operator const_pointer_type() const { return cptr(); } ///< often ambiguous

  const_pointer_type cptr() const;   ///< safer explicit ptr accessor 

  virtual void reset (bool /*hard*/) override { m_ptr = 0; }        ///< reset pointer
  //@}

  /// \name other constructors and methods for SG internal use
  //@{
  DataHandle(SG::DataProxy* proxy); ///< 
  DataHandle(const SG::ConstProxyIterator& itr1,
             const SG::ConstProxyIterator& itr2);

  //@}

  /// the CLID of the object we are bound to
  virtual CLID clid() const override { return ClassID_traits<DATA>::ID(); }

  friend
  bool operator== ATLAS_NOT_THREAD_SAFE (const DataHandle<DATA>& h1,
                                         const DataHandle<DATA>& h2)
  {
    return h1.m_proxy == h2.m_proxy;
  }

  friend
  bool operator!= ATLAS_NOT_THREAD_SAFE (const DataHandle<DATA>& h1,
                                         const DataHandle<DATA>& h2)
  {
    return h1.m_proxy != h2.m_proxy;
  }
    
private:

  // OK since DataHandle should only be used locally, not shared between threads.
  // Eventually we'd like to deprecate DataHandle anyway.
  mutable const_pointer_type m_ptr;
  const_pointer_type dataPointer() const;


};

#include "StoreGate/DataHandle.icc"

/* FIXME LEGACY - No dependency on ActiveStoreSvc here, but a number of Muon AtlasEvent packages are 
   getting the include through this one!!!! */

#include "StoreGate/ActiveStoreSvc.h"

#endif // STOREGATE_DATAHANDLE_H











