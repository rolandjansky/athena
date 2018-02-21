/* -*- C++ -*- */

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file SG_iterator.h
 *  defines a standard conforming forward iterator based on DataHandle
 */
#ifndef STOREGATE_SG_ITERATOR_H
#define STOREGATE_SG_ITERATOR_H 1

#include "StoreGate/DataHandle.h"
#include <boost/iterator/iterator_facade.hpp>

namespace SG {

namespace detail {
/** @class DHIteratorBase 
 *  Implementation class, not to be used directly
 */
  template <class DATA>
  class DHIteratorBase {
  protected:
    /// @name boost iterator implementation
    //@{
    DHIteratorBase() : m_dh() {}
    explicit DHIteratorBase(const DataHandle<DATA>& dh) : m_dh(dh) {}

    friend class boost::iterator_core_access;
    void increment() { ++m_dh; }
    //@}
    
    bool eql(const DHIteratorBase<DATA>& rhs) const {
      return m_dh == rhs.m_dh;
    }
    DATA& deref() const { return *m_dh; }
    const DATA& const_deref() const
    {
      const DataHandle<DATA>& dh = m_dh;
      return *dh;
    }

  public:
    /// @name SG-specific accessors
    //@{
    /// Get the key string with which the current object was stored.
    const std::string& key() const
    { return m_dh.key(); }
    /// RETRIEVES the data object to check it is valid and unlocked
    bool isValid() const
    {
      const DataHandle<DATA>& dh = m_dh;
      return dh.isValid();
    }
    /// RETRIEVES the data object to check it is valid
    //bool isValid()
    //{ return m_dh.isValid(); }
    /// weaker test but it does not touch the disk!
    bool isInitialized() const
    { return m_dh.isInitialized(); }
    //@}
  private:
    mutable DataHandle<DATA> m_dh;
  };

} //end ns detail
  
/** @class ConstIterator 
 *  a const_iterator facade to DataHandle. Behaves like a forward iterator
 */
  template <class DATA>
  class ConstIterator : 
  public detail::DHIteratorBase<DATA>, 
  public boost::iterator_facade<
    ConstIterator<DATA>, 
    const DATA,
    boost::forward_traversal_tag > 
  {
  public:
    ConstIterator() : detail::DHIteratorBase<DATA>() {}
    explicit ConstIterator(const DataHandle<DATA>& dh) :
      detail::DHIteratorBase<DATA>(dh) {}
  private:
    /// @name boost iterator implementation
    //@{
    friend class boost::iterator_core_access;
    //INHERITED void increment();
    bool equal(const ConstIterator<DATA>& rhs) const { 
      return detail::DHIteratorBase<DATA>::eql(rhs);
    }
    const DATA& dereference() const { 
      return detail::DHIteratorBase<DATA>::const_deref(); 
    }
    //@}
  };


  /** @class Iterator 
   *  an iterator facade to DataHandle. Behaves like a forward iterator
   */
  template <class DATA>
  class Iterator : 
  public detail::DHIteratorBase<DATA>, 
  public boost::iterator_facade<
    Iterator<DATA>, 
    DATA,
    boost::forward_traversal_tag > 
  {
  public:
    Iterator() : detail::DHIteratorBase<DATA>() {}
    explicit Iterator(const DataHandle<DATA>& dh) : 
      detail::DHIteratorBase<DATA>(dh) {}
  private:
    /// @name boost iterator implementation
    //@{
    friend class boost::iterator_core_access;
    //INHERITED void increment();
    bool equal(const Iterator<DATA>& rhs) const { 
      return detail::DHIteratorBase<DATA>::eql(rhs);
    }
    DATA& dereference() const { 
      return detail::DHIteratorBase<DATA>::deref();
    }
    //@}
  };
} //ns SG
#endif
    
