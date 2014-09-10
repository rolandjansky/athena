/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKD3PDMAKER_PIXELCLUSTERCOLLECTIONGETTERTOOL_H
#define TRACKD3PDMAKER_PIXELCLUSTERCOLLECTIONGETTERTOOL_H

#include "D3PDMakerUtils/SGCollectionGetterTool.h" 
#include "InDetPrepRawData/PixelClusterContainer.h" 
#include "InDetPrepRawData/PixelClusterCollection.h"

namespace InDet {
  class PixelCluster;
}
class PixelID;

namespace D3PD { 

class PixelClusterCollectionGetterTool 
  : public SGCollectionGetterTool<InDet::PixelClusterContainer>
{ 
 public: 
  typedef SGCollectionGetterTool<InDet::PixelClusterContainer> Base; 
  PixelClusterCollectionGetterTool (const std::string& type, 
				    const std::string& name, 
				    const IInterface* parent); 

  StatusCode initialize(); 
  size_t sizeHint(bool=false); 
  StatusCode reset(bool=false);
  const void* nextUntyped();
  const std::type_info& typeinfo() const;
  const std::type_info& elementTypeinfo() const;

 private:

  bool isBLayer(const InDet::PixelCluster *clus);

  InDet::PixelClusterContainer::const_iterator m_contItr; 
  InDet::PixelClusterContainer::const_iterator m_contEnd; 
  
  InDet::PixelClusterCollection::const_iterator m_collItr; 
  InDet::PixelClusterCollection::const_iterator m_collEnd; 

  bool m_BLayerOnly;
  const PixelID* m_pixelId; 

}; 

// Dummies --- the default versions of these don't compile.
// But these are overridden by the derived class here.
template<>
const std::type_info&
CollectionGetterTool<InDet::PixelClusterContainer>::elementTypeinfo() const;

template<>
const void*
CollectionGetterTool<InDet::PixelClusterContainer>::nextUntyped();

} 

#endif // TRACKD3PDMAKER_PIXELCLUSTERCOLLECTIONGETTERTOOL_H 
