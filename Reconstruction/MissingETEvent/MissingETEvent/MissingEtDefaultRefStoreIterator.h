/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MISSINGET_MISSINGETDEFAULTREFSTOREITERATOR
#define MISSINGET_MISSINGETDEFAULTREFSTOREITERATOR

class MissingEtDefaultRefStoreIterator
{
 public:
  MissingEtDefaultRefStoreIterator() : m_actual(0) { };
  MissingEtDefaultRefStoreIterator(refstore_t::iterator storeIter)
    : m_actual(storeIter)
  { };
  ~MissingEtDefaultRefStoreIterator() { };
  
  // stepping operators
  MissingEtDefaultRefStoreIterator operator++() 
  { m_actual++; return *this; }
  MissingEtDefaultRefStoreIterator operator++(int) 
  { ++m_actual; return *this; }
  MissingEtDefaultRefStoreIterator operator--() 
  { m_actual--; return *this; }
  MissingEtDefaultRefStoreIterator operator--(int) 
  { --m_actual; return *this; }
  
  // comparators
  bool operator==(const MissingEtDefaultRefStoreIterator& anIter) const
  { return anIter.m_actual == m_actual; }
  bool operator!=(const MissingEtDefaultRefStoreIterator& anIter) const
  { return anIter.m_actual != m_actual; }
  
  // data access
  const refobject_t* operator*()    const { return this->getObjectPtr(); }
  const refobject_t* getObjectPtr() const { return (*m_actual).first;    }
  const refweight_t& getWeight()    const { return (*m_actual).second;   }
  refweight_t&       getWeight()          { return (*m_actual).second;   }

  // internal iterator access
  //      const refstore_t::const_iterator& getInternalIterator() const 
  //     { return static_cast<restore_t::const_iterator&>(m_actual); }
  refstore_t::iterator& getInternalIterator()  { return m_actual; }
  
 private:
  
  refstore_t::iterator m_actual;
};

#endif
