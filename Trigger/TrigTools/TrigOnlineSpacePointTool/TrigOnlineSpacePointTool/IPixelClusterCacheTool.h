/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __IPIXEL_CLUSTER_CACHE_TOOL_H__
#define __IPIXEL_CLUSTER_CACHE_TOOL_H__

#include "ByteStreamData/RawEvent.h"
#include "GaudiKernel/IAlgTool.h"

typedef OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment         ROBF ;

static const InterfaceID IID_IPixelClusterCacheTool("IPixelClusterCacheTool", 1 , 0); 

class IPixelClusterCacheTool : virtual public IAlgTool {
public:

  static const InterfaceID& interfaceID() {
    return IID_IPixelClusterCacheTool;
  }

  // Convert clusters to space points
  virtual StatusCode m_convertBStoClusters(std::vector<const ROBF*>& robFragments,const std::vector<IdentifierHash> listOfPixIds,
					   std::vector<int>&, bool) = 0;
};

#endif
