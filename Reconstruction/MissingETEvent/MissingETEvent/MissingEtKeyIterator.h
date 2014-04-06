/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MISSINGETEVENT_MISSINGETKEYITERATOR_H
#define MISSINGETEVENT_MISSINGETKEYITERATOR_H

//namespace MissingEtMap
//{
  class MissingEtKeyIterator
  {
  public:

    MissingEtKeyIterator()                 : m_actual(iiterator()) { };
    MissingEtKeyIterator(iiterator  iIter) : m_actual(iIter)       { };
    ~MissingEtKeyIterator()                                        { };

    // stepping operators
    MissingEtKeyIterator operator++()    { m_actual++; return *this; }
    MissingEtKeyIterator operator++(int) { ++m_actual; return *this; }
    MissingEtKeyIterator operator--()    { m_actual--; return *this; }
    MissingEtKeyIterator operator--(int) { --m_actual; return *this; }

    // comparators
    bool operator==(const MissingEtKeyIterator& anIter) const
    { return anIter.m_actual == m_actual; }
    bool operator!=(const MissingEtKeyIterator& anIter) const
    { return anIter.m_actual != m_actual; }

    // data access
    const refstore_t& operator*()   const  { return this->getRefStore(); }
    const refstore_t& getRefStore() const  { return (*m_actual).second;  }
    refstore_t&       operator*()          { return this->getRefStore(); }
    refstore_t&       getRefStore()        { return (*m_actual).second;  }
    const key_t*      getKeyObject() const { return (*m_actual).first;   } 

    // distances
    size_t operator-(const MissingEtKeyIterator& anIter) const
    { return (size_t)(m_actual-anIter.m_actual); }
    MissingEtKeyIterator operator+(size_t iStep)
    { m_actual = m_actual + iStep; return *this; }

    // access to internal iterator
    iiterator getInternalIterator() { return m_actual; }

  private:

    iiterator m_actual;
  };

  class MissingEtKeyConstIterator
  {
  public:

    MissingEtKeyConstIterator()                 : m_actual(iconst_iterator()) { };
    MissingEtKeyConstIterator(iconst_iterator  iIter) : m_actual(iIter)       { };
    ~MissingEtKeyConstIterator()                                        { };

    // stepping operators
    MissingEtKeyConstIterator operator++()    { m_actual++; return *this; }
    MissingEtKeyConstIterator operator++(int) { ++m_actual; return *this; }
    MissingEtKeyConstIterator operator--()    { m_actual--; return *this; }
    MissingEtKeyConstIterator operator--(int) { --m_actual; return *this; }

    // comparators
    bool operator==(const MissingEtKeyConstIterator& anIter) const
    { return anIter.m_actual == m_actual; }
    bool operator!=(const MissingEtKeyConstIterator& anIter) const
    { return anIter.m_actual != m_actual; }

    // data access
    const refstore_t& operator*()   const  { return this->getRefStore(); }
    const refstore_t& getRefStore() const  { return (*m_actual).second;  }
    //    refstore_t&       operator*()          { return this->getRefStore(); }
    //    refstore_t&       getRefStore()        { return (*m_actual).second;  }
    const key_t*      getKeyObject() const { return (*m_actual).first;   } 

    // distances
    size_t operator-(const MissingEtKeyConstIterator& anIter) const
    { return (size_t)(m_actual-anIter.m_actual); }
    MissingEtKeyConstIterator operator+(size_t iStep)
    { m_actual = m_actual + iStep; return *this; }

    // access to internal iterator
    iconst_iterator getInternalIterator() const { return m_actual; }

  private:

    iconst_iterator m_actual;
  };
//};
#endif
