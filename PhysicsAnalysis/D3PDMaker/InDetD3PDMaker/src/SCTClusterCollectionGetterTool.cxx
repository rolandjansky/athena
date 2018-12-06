/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SCTClusterCollectionGetterTool.h"

#include "InDetPrepRawData/SCT_Cluster.h"

namespace D3PD {

SCTClusterCollectionGetterTool::SCTClusterCollectionGetterTool
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : Base (type, name, parent)
{
}

StatusCode SCTClusterCollectionGetterTool::initialize()
{
  CHECK( Base::initialize() );

  return StatusCode::SUCCESS;
}

const std::type_info& SCTClusterCollectionGetterTool::typeinfo() const
{
  return typeid(InDet::SCT_ClusterContainer);
}


const std::type_info& SCTClusterCollectionGetterTool::elementTypeinfo() const
{
  return typeid(InDet::SCT_Cluster);
}

size_t SCTClusterCollectionGetterTool::sizeHint(bool allowMissing)
{
  const InDet::SCT_ClusterContainer* cont = (InDet::SCT_ClusterContainer*)getUntyped(allowMissing);

  if (!cont) {
    return 0;
  }
  int nclus = 0;

  // Loop over the container
  InDet::SCT_ClusterContainer::const_iterator itr = cont->begin();
  InDet::SCT_ClusterContainer::const_iterator itr_end = cont->end();
  for( ; itr!=itr_end; ++itr ){

    // Loop over the collection
    InDet::SCT_ClusterCollection::const_iterator itr_col = (*itr)->begin();
    InDet::SCT_ClusterCollection::const_iterator itr_col_end = (*itr)->end();
    for( ; itr_col!=itr_col_end; ++itr_col ){
      nclus++;
    }
  }

  return nclus;
}

StatusCode SCTClusterCollectionGetterTool::reset(bool allowMissing)
{

  //std::cout << "reset in SCT Cluster Getter" << std::endl;

  const InDet::SCT_ClusterContainer* cont = (InDet::SCT_ClusterContainer*)getUntyped(allowMissing);
 
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


const void* SCTClusterCollectionGetterTool::nextUntyped()
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

  const InDet::SCT_Cluster *clus = *m_collItr;
  
  m_collItr++;

  //std::cout << "nextUntyped in SCT Cluster Getter. count_this " << std::endl;

  return clus;
}


// Dummies --- the default versions of these don't compile.
// But these are overridden by the derived class here.
template<>
const std::type_info&
CollectionGetterTool<InDet::SCT_ClusterContainer>::elementTypeinfo() const
{
  return typeid(InDet::SCT_Cluster);
}

template<>
const void*
CollectionGetterTool<InDet::SCT_ClusterContainer>::nextUntyped()
{
  return 0;
}


}
