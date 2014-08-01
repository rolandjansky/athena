/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: MdtPrepDataCollectionGetterTool.cxx 535250 2013-01-29 13:22:17Z salekd $
#include "MdtPrepDataCollectionGetterTool.h"

namespace D3PD {

MdtPrepDataCollectionGetterTool::MdtPrepDataCollectionGetterTool
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : Base (type, name, parent)
{
}

StatusCode MdtPrepDataCollectionGetterTool::initialize()
{
  CHECK( Base::initialize() );

  return StatusCode::SUCCESS;
}

const std::type_info& MdtPrepDataCollectionGetterTool::typeinfo() const
{
  return typeid(Muon::MdtPrepDataContainer);
}


const std::type_info& MdtPrepDataCollectionGetterTool::elementTypeinfo() const
{
  return typeid(Muon::MdtPrepData);
}

size_t MdtPrepDataCollectionGetterTool::sizeHint(bool allowMissing)
{
  const Muon::MdtPrepDataContainer* cont = (Muon::MdtPrepDataContainer*)getUntyped(allowMissing);

  if (!cont) {
    return 0;
  }
  int nclus = 0;

  // Loop over the container
  Muon::MdtPrepDataContainer::const_iterator itr = cont->begin();
  Muon::MdtPrepDataContainer::const_iterator itr_end = cont->end();
  for( ; itr!=itr_end; ++itr ){

    // Loop over the collection
    Muon::MdtPrepDataCollection::const_iterator itr_col = (*itr)->begin();
    Muon::MdtPrepDataCollection::const_iterator itr_col_end = (*itr)->end();
    for( ; itr_col!=itr_col_end; ++itr_col ){
      nclus++;
    }
  }

  return nclus;
}

StatusCode MdtPrepDataCollectionGetterTool::reset(bool allowMissing)
{
  const Muon::MdtPrepDataContainer* cont = (Muon::MdtPrepDataContainer*)getUntyped(allowMissing);
 
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


const void* MdtPrepDataCollectionGetterTool::nextUntyped()
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

  const Muon::MdtPrepData *clus = *m_collItr;
  
  m_collItr++;

  return clus;
}


// Dummies --- the default versions of these don't compile.
// But these are overridden by the derived class here.
template<>
const std::type_info&
CollectionGetterTool<Muon::MdtPrepDataContainer>::elementTypeinfo() const
{
  return typeid(Muon::MdtPrepData);
}

template<>
const void*
CollectionGetterTool<Muon::MdtPrepDataContainer>::nextUntyped()
{
  return 0;
}


}
