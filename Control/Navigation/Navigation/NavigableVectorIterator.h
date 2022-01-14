/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef NAVIGATION_NAVIGABLEVECTORITERATOR_H
#define NAVIGATION_NAVIGABLEVECTORITERATOR_H
////////////////////////////////////////////////////////////////////////
// iterator
////////////////////////////////////////////////////////////////////////

#include <vector>

template< typename OBJCT, typename STORE >
class NavigableVectorIterator
{ 
 public:

  typedef OBJCT object_type;
  typedef STORE store_type;
  typedef typename store_type::const_iterator store_const_iter;

  //////////////////////////////
  // Constuctors & Destructor //
  //////////////////////////////

  // default
  NavigableVectorIterator(){};
  // copy
  //  NavigableVectorIterator(NavigableVectorIterator& anIter)
  //    {
  //      m_storePtr   = anIter.m_storePtr;
  //      m_actualIter = anIter.m_actualIter;
  //   }
  // initial
  NavigableVectorIterator(const store_type* aStore)
    : m_storePtr(aStore)
    , m_actualIter(aStore->begin())
    { };

  NavigableVectorIterator(const store_type& aStore) 
    : m_storePtr(&aStore)
    , m_actualIter(aStore.begin())
    { }

  // destructor
  virtual ~NavigableVectorIterator() = default;

  /////////////////////////
  // Iterator Operations //
  /////////////////////////

  // begin and end 
  NavigableVectorIterator begin()
    {
      m_actualIter = m_storePtr->begin();
      return *this;
    }
  NavigableVectorIterator begin() const
    {
      m_actualIter = m_storePtr->begin();
      return *this;
    }

  NavigableVectorIterator end()
    {
      m_actualIter = m_storePtr->end();
      return *this;
    }

  NavigableVectorIterator end() const
    {
      m_actualIter = m_storePtr->end();
      return *this;
    }

  // iterator stepping
  NavigableVectorIterator operator++()
    { 
      m_actualIter++;
      return *this;
    }
  NavigableVectorIterator operator++(int)
    {
      m_actualIter++;
      return *this;
    }
  NavigableVectorIterator operator--()
    {
      m_actualIter--;
      return *this;
    }
  NavigableVectorIterator& operator--(int)
    {
      m_actualIter--;
      return *this--;
    }

  // data access operations
  const object_type* operator[](int i)
    {
      return i >= 0 && i < m_storePtr->size()
	? (*m_storePtr)[i]
	: 0;
    }

  const object_type* operator[](unsigned int i)
    {
      return i < m_storePtr->size()
	? (*m_storePtr)[i]
	: 0;
    }
  const object_type* operator*()
    {
      return *(*m_actualIter);
    }

  // comparisons and finding
  bool operator==(const NavigableVectorIterator& anIter) const
    {
      return m_actualIter == anIter.m_actualIter;
    }
  bool operator!=(const NavigableVectorIterator& anIter) const
    {
      return m_actualIter != anIter.m_actualIter;
    }
  NavigableVectorIterator& find(const object_type* anObject)
    {
      bool found = false;
      m_actualIter = m_storePtr->begin(); 
      while ( ( ! found ) && m_actualIter != m_storePtr->end() )
	{
	  found        = *(*m_actualIter) == anObject;
	  if ( !found ) m_actualIter++;
	}
      return *this;
    }

 private:
  
  const store_type*   m_storePtr;
  store_const_iter  m_actualIter;
};
#endif
