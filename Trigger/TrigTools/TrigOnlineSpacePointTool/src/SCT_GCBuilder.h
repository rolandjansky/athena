/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_GCBUILDER_H
#define SCT_GCBUILDER_H

#include "InDetPrepRawData/SCT_ClusterCollection.h"

#include <vector>

class SCT_ID;
class TrigSiSpacePoint;
namespace InDetDD {
  class SiDetectorElementCollection;
}

class SCT_GCBuilder {
public:
  SCT_GCBuilder(const SCT_ID* , bool, int, int);
  virtual ~SCT_GCBuilder() = default;

  void formSpacePoints (const InDet::SCT_ClusterCollection& phi_clusterColl,
                        const InDetDD::SiDetectorElementCollection* elements,
                        std::vector<TrigSiSpacePoint*>& spacePoints);
  
  void formSpacePoints (const InDet::SCT_ClusterCollection& phi_clusterColl,
                        const InDet::SCT_ClusterCollection& uv_clusterColl,
                        const bool allowPhiOnly,
                        const InDetDD::SiDetectorElementCollection* elements,
                        std::vector<TrigSiSpacePoint*>& spacePoints);
private:
  const SCT_ID* m_sctID;
  bool m_useOfflineAlgorithm;
  int m_OffsetBarrelSCT;
  int m_OffsetEndcapSCT;
};

#endif
