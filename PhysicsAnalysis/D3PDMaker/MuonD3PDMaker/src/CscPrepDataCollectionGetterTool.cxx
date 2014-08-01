/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CscPrepDataCollectionGetterTool.cxx 535250 2013-01-29 13:22:17Z salekd $
#include "CscPrepDataCollectionGetterTool.h"

namespace D3PD {

CscPrepDataCollectionGetterTool::CscPrepDataCollectionGetterTool
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : Base (type, name, parent)
{
}

StatusCode CscPrepDataCollectionGetterTool::initialize()
{
  CHECK( Base::initialize() );

  return StatusCode::SUCCESS;
}

const std::type_info& CscPrepDataCollectionGetterTool::typeinfo() const
{
  return typeid(Muon::CscPrepDataContainer);
}


const std::type_info& CscPrepDataCollectionGetterTool::elementTypeinfo() const
{
  return typeid(Muon::CscPrepData);
}

size_t CscPrepDataCollectionGetterTool::sizeHint(bool allowMissing)
{
  const Muon::CscPrepDataContainer* cont = (Muon::CscPrepDataContainer*)getUntyped(allowMissing);

  if (!cont) {
    return 0;
  }
  int nclus = 0;

  // Loop over the container
  Muon::CscPrepDataContainer::const_iterator itr = cont->begin();
  Muon::CscPrepDataContainer::const_iterator itr_end = cont->end();
  for( ; itr!=itr_end; ++itr ){

    // Loop over the collection
    Muon::CscPrepDataCollection::const_iterator itr_col = (*itr)->begin();
    Muon::CscPrepDataCollection::const_iterator itr_col_end = (*itr)->end();
    for( ; itr_col!=itr_col_end; ++itr_col ){
      nclus++;
    }
  }

  return nclus;
}

StatusCode CscPrepDataCollectionGetterTool::reset(bool allowMissing)
{
  const Muon::CscPrepDataContainer* cont = (Muon::CscPrepDataContainer*)getUntyped(allowMissing);
 
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


const void* CscPrepDataCollectionGetterTool::nextUntyped()
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

  const Muon::CscPrepData *clus = *m_collItr;
  
  m_collItr++;

  return clus;
}


// Dummies --- the default versions of these don't compile.
// But these are overridden by the derived class here.
template<>
const std::type_info&
CollectionGetterTool<Muon::CscPrepDataContainer>::elementTypeinfo() const
{
  return typeid(Muon::CscPrepData);
}

template<>
const void*
CollectionGetterTool<Muon::CscPrepDataContainer>::nextUntyped()
{
  return 0;
}


}
