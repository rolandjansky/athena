/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKD3PDMAKER_SCTCLUSTERCOLLECTIONGETTERTOOL_H
#define TRACKD3PDMAKER_SCTCLUSTERCOLLECTIONGETTERTOOL_H

#include "D3PDMakerUtils/SGCollectionGetterTool.h" 
#include "InDetPrepRawData/SCT_ClusterContainer.h" 
#include "InDetPrepRawData/SCT_ClusterCollection.h"
#include "InDetPrepRawData/SCT_Cluster.h" 
#include "PixelClusterCollectionGetterTool.h"

namespace D3PD { 
  
class SCTClusterCollectionGetterTool 
  : public SGCollectionGetterTool<InDet::SCT_ClusterContainer>
{ 
 public: 
  typedef SGCollectionGetterTool<InDet::SCT_ClusterContainer> Base; 

  SCTClusterCollectionGetterTool (const std::string& type,
				  const std::string& name, 
				  const IInterface* parent); 

  StatusCode initialize(); 
  size_t sizeHint(bool=false); 
  StatusCode reset(bool=false); 
  const void* nextUntyped(); 
  const std::type_info& typeinfo() const; 
  const std::type_info& elementTypeinfo() const; 
  
 private: 
  InDet::SCT_ClusterContainer::const_iterator m_contItr; 
  InDet::SCT_ClusterContainer::const_iterator m_contEnd; 
  
  InDet::SCT_ClusterCollection::const_iterator m_collItr; 
  InDet::SCT_ClusterCollection::const_iterator m_collEnd; 
}; 

// Dummies --- the default versions of these don't compile.
// But these are overridden by the derived class here.
template<>
const std::type_info&
CollectionGetterTool<InDet::SCT_ClusterContainer>::elementTypeinfo() const;

template<>
const void*
CollectionGetterTool<InDet::SCT_ClusterContainer>::nextUntyped();

} 

#endif // TRACKD3PDMAKER_SCTCLUSTERCOLLECTIONGETTERTOOL_H 
