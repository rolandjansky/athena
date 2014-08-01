/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: RpcPrepDataCollectionGetterTool.cxx 535250 2013-01-29 13:22:17Z salekd $
#include "RpcPrepDataCollectionGetterTool.h"

namespace D3PD {

RpcPrepDataCollectionGetterTool::RpcPrepDataCollectionGetterTool
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : Base (type, name, parent)
{
}

StatusCode RpcPrepDataCollectionGetterTool::initialize()
{
  CHECK( Base::initialize() );

  return StatusCode::SUCCESS;
}

const std::type_info& RpcPrepDataCollectionGetterTool::typeinfo() const
{
  return typeid(Muon::RpcPrepDataContainer);
}


const std::type_info& RpcPrepDataCollectionGetterTool::elementTypeinfo() const
{
  return typeid(Muon::RpcPrepData);
}

size_t RpcPrepDataCollectionGetterTool::sizeHint(bool allowMissing)
{
  const Muon::RpcPrepDataContainer* cont = (Muon::RpcPrepDataContainer*)getUntyped(allowMissing);

  if (!cont) {
    return 0;
  }
  int nclus = 0;

  // Loop over the container
  Muon::RpcPrepDataContainer::const_iterator itr = cont->begin();
  Muon::RpcPrepDataContainer::const_iterator itr_end = cont->end();
  for( ; itr!=itr_end; ++itr ){

    // Loop over the collection
    Muon::RpcPrepDataCollection::const_iterator itr_col = (*itr)->begin();
    Muon::RpcPrepDataCollection::const_iterator itr_col_end = (*itr)->end();
    for( ; itr_col!=itr_col_end; ++itr_col ){
      nclus++;
    }
  }

  return nclus;
}

StatusCode RpcPrepDataCollectionGetterTool::reset(bool allowMissing)
{
  const Muon::RpcPrepDataContainer* cont = (Muon::RpcPrepDataContainer*)getUntyped(allowMissing);
 
  if (!cont) {
    m_contItr = m_contEnd;
    m_collItr = m_collEnd;
    return StatusCode::FAILURE;
  }

  m_contItr = cont->begin();
  m_contEnd = cont->end();

  if(m_contItr == m_contEnd){
    m_collItr = m_collEnd;
    return StatusCode::SUCCESS;
  }

  m_collItr = (*m_contItr)->begin();
  m_collEnd = (*m_contItr)->end();

  return StatusCode::SUCCESS;
}


const void* RpcPrepDataCollectionGetterTool::nextUntyped()
{
  if(m_collItr == m_collEnd){
    if(m_contItr==m_contEnd){
      return 0;
    }
    m_contItr++;
    if(m_contItr==m_contEnd){
      return 0;
    }
    m_collItr = (*m_contItr)->begin();
    m_collEnd = (*m_contItr)->end();
    if(m_collItr == m_collEnd){
      return 0;
    }
  }

  const Muon::RpcPrepData *clus = *m_collItr;
  
  m_collItr++;

  return clus;
}


// Dummies --- the default versions of these don't compile.
// But these are overridden by the derived class here.
template<>
const std::type_info&
CollectionGetterTool<Muon::RpcPrepDataContainer>::elementTypeinfo() const
{
  return typeid(Muon::RpcPrepData);
}

template<>
const void*
CollectionGetterTool<Muon::RpcPrepDataContainer>::nextUntyped()
{
  return 0;
}


}
