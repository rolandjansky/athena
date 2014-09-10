/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelClusterCollectionGetterTool.h"

#include "InDetPrepRawData/PixelCluster.h"
#include "Identifier/Identifier.h"
#include "InDetIdentifier/PixelID.h"

namespace D3PD {

PixelClusterCollectionGetterTool::PixelClusterCollectionGetterTool
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : Base (type, name, parent),
      m_pixelId(0)
{
  declareProperty("BLayerOnly", m_BLayerOnly = false);
}

StatusCode PixelClusterCollectionGetterTool::initialize()
{
  CHECK( Base::initialize() );

  CHECK( detStore()->retrieve(m_pixelId,"PixelID") );

  return StatusCode::SUCCESS;
}

const std::type_info& PixelClusterCollectionGetterTool::typeinfo() const
{
  return typeid(InDet::PixelClusterContainer);
}


const std::type_info& PixelClusterCollectionGetterTool::elementTypeinfo() const
{
  return typeid(InDet::PixelCluster);
}

size_t PixelClusterCollectionGetterTool::sizeHint(bool allowMissing)
{
  const InDet::PixelClusterContainer* cont = (InDet::PixelClusterContainer*)getUntyped(allowMissing);

  if (!cont) {
    return 0;
  }
  int nclus = 0;

  // Loop over the container
  InDet::PixelClusterContainer::const_iterator itr = cont->begin();
  InDet::PixelClusterContainer::const_iterator itr_end = cont->end();
  for( ; itr!=itr_end; ++itr ){

    // Loop over the collection
    InDet::PixelClusterCollection::const_iterator itr_col = (*itr)->begin();
    InDet::PixelClusterCollection::const_iterator itr_col_end = (*itr)->end();
    for( ; itr_col!=itr_col_end; ++itr_col ){
      nclus++;
    }
  }

  return nclus;
}

StatusCode PixelClusterCollectionGetterTool::reset(bool allowMissing)
{

  const InDet::PixelClusterContainer* cont = (InDet::PixelClusterContainer*)getUntyped(allowMissing);
 
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

const void* PixelClusterCollectionGetterTool::nextUntyped()
{
  if(m_collItr == m_collEnd){
    if(m_contItr == m_contEnd){
      return 0;
    }
    m_contItr++;
    if(m_contItr == m_contEnd){
      return 0;
    }
    m_collItr = (*m_contItr)->begin();
    m_collEnd = (*m_contItr)->end();
    if(m_collItr == m_collEnd){
      return 0;
    }
  }

  const InDet::PixelCluster *clus = *m_collItr;

  m_collItr++;

  if(m_BLayerOnly){
    if(clus){
      if(!isBLayer(clus)){
	clus = (const InDet::PixelCluster *)nextUntyped();
      }
    }
  }

  return clus;
}

bool PixelClusterCollectionGetterTool::isBLayer(const InDet::PixelCluster *clus){

  Identifier id = clus->identify();

  if ( id.is_valid() ) {
    if( m_pixelId->is_blayer(id) ) {
      return true;
    }
  }

  return false;
}

// Dummies --- the default versions of these don't compile.
// But these are overridden by the derived class here.
template<>
const std::type_info&
CollectionGetterTool<InDet::PixelClusterContainer>::elementTypeinfo() const
{
  return typeid(InDet::PixelCluster);
}

template<>
const void*
CollectionGetterTool<InDet::PixelClusterContainer>::nextUntyped()
{
  return 0;
}

}
