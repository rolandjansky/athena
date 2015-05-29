/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __ISCT_CLUSTER_CACHE_TOOL_H__
#define __ISCT_CLUSTER_CACHE_TOOL_H__

#include "ByteStreamData/RawEvent.h"
#include "GaudiKernel/IAlgTool.h"
#include <vector>

typedef OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment         ROBF ;

static const InterfaceID IID_ISCT_ClusterCacheTool("ISCT_ClusterCacheTool", 1, 0);


class ISCT_ClusterCacheTool : virtual public IAlgTool {
public:

  static const InterfaceID& interfaceID() {
    return IID_ISCT_ClusterCacheTool; 
  }

  // Convert clusters to space points
  virtual StatusCode m_convertBStoClusters(std::vector<const ROBF*>& robFragments,
					   std::vector<IdentifierHash>listOfSCTIds, std::vector<int>&, bool) = 0;
};

#endif
