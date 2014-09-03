/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "CaloEvent/CaloClusterMomentStore.h"

// constructor
CaloClusterMomentStore::CaloClusterMomentStore() 
{ }

CaloClusterMomentStore::CaloClusterMomentStore(const CaloClusterMomentStore& 
					       rMomStore)
  : m_store (rMomStore.m_store)
{
}

CaloClusterMomentStore::CaloClusterMomentStore(const CaloClusterMomentStore*
					       pMomStore)
  : m_store (pMomStore->m_store)
{
  m_store = pMomStore->m_store;
}

CaloClusterMomentStore&
CaloClusterMomentStore::operator= (const CaloClusterMomentStore& rMomStore)
{
  if (this != &rMomStore)
    m_store = rMomStore.m_store;
  return *this;
}

// destructor
CaloClusterMomentStore::~CaloClusterMomentStore()
{ }

// retrieve moment value
bool CaloClusterMomentStore::retrieve(const moment_type& rMomType,
				      moment_value&      rMomData) const
{
  moment_iterator fIter = this->find(rMomType);
  return this->retrieve(fIter,rMomData);
}
bool CaloClusterMomentStore::retrieve(const moment_iterator& rMomIter,
				      moment_value&          rMomData) const
{
  if ( rMomIter != this->end() )
    {
      rMomData = rMomIter.getMoment();
      return true;
    }
  else
    {
      return false;
    }
}

// retrieve all stored moment types
bool CaloClusterMomentStore::retrieveMomentTypes(moment_type_list&
						 rMomList) const
{
  size_t oldSize = rMomList.size();
  moment_store_const_iter fMom = m_store.begin();
  moment_store_const_iter lMom = m_store.end();
  for ( ; fMom != lMom; fMom++ )
    {
      rMomList.push_back((moment_type)(*fMom).first);
    } 
  return oldSize < rMomList.size();
}
