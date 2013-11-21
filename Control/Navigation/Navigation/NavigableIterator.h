// Hey emacs --- this is -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef NAVIGATION_NAVIGABLEITERATOR_H
#define NAVIGATION_NAVIGABLEITERATOR_H

#include "Navigation/NavigationDefaults.h"


// Normally, i'd want to use boost::iterator_adaptor here.
// But this causes dictionary problems:
//   - We'd need to add this to the selection (or use an auto-select).
//   - More importantly, it causes cint to crash because type names
//     get too long.
//
// Note also that we need to use member functions rather than free functions
// for operators, or pyroot gets confused.

template <typename CONT, typename RPAR, typename COLL>
class NavigableIterator
{
public:
  // Iterator trait typedefs.
  typedef typename COLL::const_iterator::iterator_category iterator_category;
  typedef const typename CONT::base_value_type*            value_type;
  typedef typename COLL::const_iterator::difference_type   difference_type;
  typedef value_type*                                      pointer;
  typedef value_type                                       reference;

  typedef typename CONT::base_value_type                            child_type;
  typedef typename NavigationDefaults::DefaultChildColl<CONT,RPAR> helper_type;


public:
  /////////////////
  // Constructor //
  /////////////////

  NavigableIterator()
    : m_collection (0)
  {}

  NavigableIterator(const COLL& theCollection)
    : m_actual (theCollection.begin()),
      m_collection(&theCollection)
  {
  }

  // Use implicit copy ctor, dtor, and assignment.


  //// --- Moving the iterator.

  NavigableIterator& operator++()
  {
    ++m_actual;
    return *this;
  }

  NavigableIterator operator++ (int)
  {
    NavigableIterator tmp = *this;
    ++m_actual;
    return tmp;
  }

  NavigableIterator& operator--()
  {
    --m_actual;
    return *this;
  }

  NavigableIterator operator-- (int)
  {
    NavigableIterator tmp = *this;
    --m_actual;
    return tmp;
  }

  NavigableIterator& operator+= (difference_type n)
  {
    m_actual += n;
    return *this;
  }

  NavigableIterator& operator-= (difference_type n)
  {
    m_actual -= n;
    return *this;
  }

  NavigableIterator operator+ (difference_type n) const
  {
    NavigableIterator tmp = *this;
    tmp += n;
    return tmp;
  }

  NavigableIterator operator- (difference_type n) const
  {
    NavigableIterator tmp = *this;
    tmp -= n;
    return tmp;
  }

  difference_type operator- (const NavigableIterator& other) const
  {
    return m_actual - other.m_actual;
  }


  //// --- Dereference.

  const child_type* operator*() const
  {
    return helper_type::getChildPtr(m_actual);
  }

  const child_type* operator->() const
  {
    return helper_type::getChildPtr(m_actual);
  }

  const child_type* operator[](difference_type n) const
  {
    return helper_type::getChildPtr(m_actual + n);
  }

  const ElementLink<CONT>& getElement() const
  {
    return *helper_type::getElementPtr (m_actual);
  }


  //// --- Comparison.

  bool operator== (const NavigableIterator& other) const
  {
    return m_actual == other.m_actual;
  }

  bool operator!= (const NavigableIterator& other) const
  {
    return m_actual != other.m_actual;
  }

  bool operator< (const NavigableIterator& other) const
  {
    return m_actual < other.m_actual;
  }

  bool operator> (const NavigableIterator& other) const
  {
    return m_actual > other.m_actual;
  }

  bool operator<= (const NavigableIterator& other) const
  {
    return m_actual <= other.m_actual;
  }

  bool operator>= (const NavigableIterator& other) const
  {
    return m_actual >= other.m_actual;
  }


  //// --- Special NavigableIterator methods.

  NavigableIterator begin()
  {
    m_actual = m_collection->begin();
    return *this;
  }

  NavigableIterator end()
  {
    m_actual = m_collection->end();
    return *this;
  }

  NavigableIterator next()
  {
    ++m_actual;
    return *this;
  }

  NavigableIterator previous()
  {
    --m_actual;
    return *this;
  }
  
  RPAR getParameter() const
    {
      return helper_type::getChildPar(m_actual);
    } 

  // access to internal iterator
  typename COLL::const_iterator getInternalIterator() const
  {
    return m_actual;
  }



protected:

  typename COLL::const_iterator m_actual;
  const COLL* m_collection;
};


template <typename CONT, typename RPAR, typename COLL>
NavigableIterator<CONT, RPAR, COLL>
operator+ (typename NavigableIterator<CONT, RPAR, COLL>::difference_type n,
           NavigableIterator<CONT, RPAR, COLL> a)
           
{
  a += n;
  return a;
}

#endif
