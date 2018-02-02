/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef NAVIGATION_NAVIGABLEVECTOR_H
#define NAVIGATION_NAVIGABLEVECTOR_H
///////////////////////////////////////////////////////////////////////////////
// Simple container for navigable objects.                                   //
///////////////////////////////////////////////////////////////////////////////

#include "AthLinks/tools/GenerateIndexingPolicy.h"
#include "AthLinks/ElementLink.h"

#include "Navigation/NavigableVectorIterator.h"

#include <vector>

template < typename CONT, typename OBJCT=typename CONT::base_value_type >
class NavigableVector
{
public:
  
  // object container index type
  typedef typename SG::GenerateIndexingPolicy<CONT>::type::index_type
  external_index_type;
  
  // 
  typedef ElementLink<CONT>                              data_type;
  typedef std::vector<data_type>                        store_type;
  typedef typename store_type::iterator                 store_iter;
  typedef typename store_type::const_iterator     store_const_iter;
  typedef NavigableVectorIterator<OBJCT,store_type> const_iterator;
  typedef OBJCT*                                        object_ptr;
  typedef const OBJCT*                            object_const_ptr;

  NavigableVector() 
  { }

  virtual ~NavigableVector() 
  { 
    m_store.clear();
  };

  // add elements
  void addElement(const CONT* aContainer, const OBJCT* anObject)
  {
    data_type aLink;
    aLink.toContainedElement(*aContainer,(object_ptr)anObject);
    m_store.push_back(aLink);
  }

  void addElement(const CONT* aContainer, external_index_type anIndex)
  {
    data_type aLink;
    aLink.toIndexedElement(*aContainer,anIndex);
    m_store.push_back(aLink);
  }

  void addElement(const data_type& aLink)
  {
    m_store.push_back(aLink);
  }

  unsigned int size() const { return m_store.size(); }

  // iterator access
  const_iterator begin() const
  {
    return const_iterator(m_store).begin();
  }
  const_iterator end() const
  {
    return const_iterator(m_store).end();
  }
  const_iterator find(const OBJCT* anObject) const
  {
    return const_iterator(m_store).find(anObject);
  }

  // operators
  const OBJCT* operator[](int i) const 
  {
    return i >= 0 && i < (int)m_store.size()
    ? *(m_store[i])
    : 0;
  }
  const OBJCT* operator[](size_t i) const
  {
    return i < m_store.size()
    ? *(m_store[i])
    : 0;
  }

  // Return contained ElementLink directly.
  const data_type& linkAt (size_t i) const
  {
    if (i < m_store.size())
      return m_store[i];
    static data_type null;
    return null;
  }

  void push_back(data_type& theData)
  {
    m_store.push_back(theData);
  }

protected:

  store_type     m_store;
};
#endif
