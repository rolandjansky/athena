/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_GCBUILDER_H
#define SCT_GCBUILDER_H
#include "InDetIdentifier/SCT_ID.h"
#include "InDetPrepRawData/SCT_ClusterCollection.h"
#include "InDetReadoutGeometry/SiDetectorManager.h"

class TrigSiSpacePoint;

class SCT_GCBuilder {
public:
  SCT_GCBuilder(const InDetDD::SiDetectorManager* &manager, const SCT_ID* , bool, int, int);
  ~SCT_GCBuilder();

  void formSpacePoints (const InDet::SCT_ClusterCollection& phi_clusterColl,
			std::vector<TrigSiSpacePoint*>& spacePoints);
  
  void formSpacePoints (const InDet::SCT_ClusterCollection& phi_clusterColl, 
			const InDet::SCT_ClusterCollection& uv_clusterColl, 
			bool allowPhiOnly,
			std::vector<TrigSiSpacePoint*>& spacePoints);
private:
  const SCT_ID* m_sctID;
  const InDetDD::SiDetectorManager* m_manager; 
  bool m_useOfflineAlgorithm;
  int m_OffsetBarrelSCT;
  int m_OffsetEndcapSCT;
};

#endif
