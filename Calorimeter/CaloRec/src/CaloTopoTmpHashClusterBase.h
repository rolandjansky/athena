/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------
// File and Version Information:
// $Id: CaloTopoTmpHashClusterBase.h,v 1.7 2008-12-23 02:57:16 ssnyder Exp $
//
// Description: base class for temporary Cluster Container for 
// topological cluster making tools
//   
// Environment:
//      Software developed for the ATLAS Detector at the CERN LHC
//
// Author List:
//      Sven Menke
//
//-----------------------------------------------------------------------

#ifndef CALOTOPOTMPHASHCLUSTERBASE_H
#define CALOTOPOTMPHASHCLUSTERBASE_H

#include <vector>
#include "CaloTopoTmpHashCell.h"
#include "CxxUtils/pointer_list.h"


template <class T>
class CaloTopoTmpHashClusterBase
{
protected:
  typedef CaloTopoTmpHashCell<T> HashCell;
  typedef CxxUtils::pointer_list<> pointer_list;

  // Friends
  
  // Data members

  pointer_list m_members;
  float m_maxRatio;
  // Helper functions

public:
  typedef pointer_list::pool_type pool_type;

  class iterator
    : public pointer_list::iterator
  {
  public:
    typedef T*          value_type;
    typedef value_type& reference;
    typedef value_type* pointer;

    iterator (pointer_list::iterator iter)
      : pointer_list::iterator (iter)
    {}

    reference operator*()
    {
      // cppcheck-suppress returnTempReference; false positive
      return reinterpret_cast<reference> (pointer_list::iterator::operator*());
    }

    reference operator->()
    {
      // cppcheck-suppress returnTempReference; false positive
      return reinterpret_cast<reference> (pointer_list::iterator::operator*());
    }
  };

  
  // Constructors
  CaloTopoTmpHashClusterBase (pool_type& pool) 
    : m_members (pool),
    m_maxRatio (0)
  {
  }

  // Operators
  
  // Selectors 

  inline float getMaxRatio() const
  {
    return m_maxRatio;
  }

  inline int size() const
  {
    return m_members.size();
  }

  iterator begin()
  {
    return iterator (m_members.begin());
  }

  iterator end()
  {
    return iterator (m_members.end());
  }

  void removeAll() 
  {
    m_members.clear();
    m_maxRatio=0;
  }

  void add(const HashCell& hashCell);

  void add(CaloTopoTmpHashClusterBase& hashCluster);

};


template <class T>
void CaloTopoTmpHashClusterBase<T>::add(const HashCell& hashCell)
{
  T* cell = hashCell.getCaloTopoTmpClusterCell();
  m_members.push_back (cell);
  
  float ratio = cell->getSignedRatio(); 
  if ( ratio > m_maxRatio ) {
    m_maxRatio = ratio;
  }
}

template <class T>
void CaloTopoTmpHashClusterBase<T>::add
  (CaloTopoTmpHashClusterBase& hashCluster)
{
  pointer_list::iterator beg = hashCluster.m_members.begin();
  pointer_list::iterator end = hashCluster.m_members.end();
  while (beg != end) {
    m_members.push_back (*beg);
    ++beg;
  }
  if ( hashCluster.m_maxRatio > m_maxRatio) {
    m_maxRatio = hashCluster.m_maxRatio;
  }
}

#endif // CALOTOPOTMPHASHCLUSTERBASE_H

