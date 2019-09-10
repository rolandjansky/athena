/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELGCBUILDER_H
#define PIXELGCBUILDER_H
#include "InDetIdentifier/PixelID.h"
#include "InDetPrepRawData/PixelClusterCollection.h"

class TrigSiSpacePoint;
class TrigSiSpacePoint;
namespace InDetDD {
  class SiDetectorElementCollection;
}

class PixelGCBuilder { 
public:
  PixelGCBuilder(const PixelID*, int);
  ~PixelGCBuilder();

  void formSpacePoints (const InDet::PixelClusterCollection& clusterColl,
                        const InDetDD::SiDetectorElementCollection* elements,
			std::vector<TrigSiSpacePoint*>& space_points);

private:
  const PixelID* m_pixelID;
  int m_OffsetEndcapPixels;
};

#endif
