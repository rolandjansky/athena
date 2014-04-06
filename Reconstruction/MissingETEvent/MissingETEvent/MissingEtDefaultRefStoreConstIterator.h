/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MISSINGET_MISSINGETDEFAULTREFSTORECONSTITERATOR
#define MISSINGET_MISSINGETDEFAULTREFSTORECONSTITERATOR

class MissingEtDefaultRefStoreConstIterator
{
 public:
  MissingEtDefaultRefStoreConstIterator() : m_actual(0) { };
  MissingEtDefaultRefStoreConstIterator(refstore_t::const_iterator 
					iIter)
    : m_actual(iIter)
  { };
  ~MissingEtDefaultRefStoreConstIterator() { };
  
  // stepping operators
  MissingEtDefaultRefStoreConstIterator operator++() 
  { m_actual++; return *this; }
  MissingEtDefaultRefStoreConstIterator operator++(int) 
  { ++m_actual; return *this; }
  MissingEtDefaultRefStoreConstIterator operator--() 
  { m_actual--; return *this; }
  MissingEtDefaultRefStoreConstIterator operator--(int) 
  { --m_actual; return *this; }
  
  // comparators
  bool operator==(const MissingEtDefaultRefStoreConstIterator& anIter) const
  { return anIter.m_actual == m_actual; }
  bool operator!=(const MissingEtDefaultRefStoreConstIterator& anIter) const
  { return anIter.m_actual != m_actual; }
  
  // data access
  const refobject_t* operator*()    const { return this->getObjectPtr(); }
  const refobject_t* getObjectPtr() const { return (*m_actual).first;    }
  const refweight_t& getWeight()    const { return (*m_actual).second;   }

  // internal iterator access
  //      const refstore_t::const_iterator& getInternalIterator() const 
  //     { return static_cast<restore_t::const_iterator&>(m_actual); }
  const refstore_t::const_iterator& getInternalIterator() const 
  { return m_actual; }
  
 private:
  
  refstore_t::const_iterator m_actual;
};

#endif
