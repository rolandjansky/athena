/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELGCBUILDER_H
#define PIXELGCBUILDER_H
#include "InDetIdentifier/PixelID.h"
#include "InDetPrepRawData/PixelClusterCollection.h"
#include "InDetReadoutGeometry/SiDetectorManager.h"

class TrigSiSpacePoint;
class TrigSiSpacePoint;

class PixelGCBuilder { 
public:
  PixelGCBuilder(const InDetDD::SiDetectorManager* &manager, const PixelID*, int); 
  ~PixelGCBuilder();

  void formSpacePoints (const InDet::PixelClusterCollection& clusterColl,
			std::vector<TrigSiSpacePoint*>& space_points);

private:
  const PixelID* m_pixelID;
  const InDetDD::SiDetectorManager* m_manager;
  int m_OffsetEndcapPixels;
};

#endif
