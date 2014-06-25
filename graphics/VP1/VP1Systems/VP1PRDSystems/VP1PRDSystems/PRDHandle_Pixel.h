/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PRDHANDLE_PIXEL_H
#define PRDHANDLE_PIXEL_H

#include "VP1PRDSystems/PRDHandleBase.h"
#include "InDetPrepRawData/PixelCluster.h"

class PRDCollHandle_Pixel;
class SoGroup;

class PRDHandle_Pixel : public PRDHandleBase {
public:

  PRDHandle_Pixel(PRDCollHandle_Pixel*,const InDet::PixelCluster*);
  virtual ~PRDHandle_Pixel() {};

  void buildShapes(SoNode*&shape_simple, SoNode*&shape_detailed);
  int regionIndex();

  const InDet::PixelCluster * cluster() const { return m_cluster; }
  const Trk::PrepRawData * getPRD() const { return m_cluster; }
  bool isBarrel() const { return m_cluster->detectorElement()->isBarrel(); }
  bool isPositiveZ() const { return m_cluster->detectorElement()->center().z() > 0.0; }
  bool isSane() const; //!< returns false if anything seems weird about the Pixelcluster

private:
  const InDet::PixelCluster* m_cluster;

  SoGroup* createErrorAtPixelCluster( const double& sigmaScale,
				      const int& numPoints ) const;
	

};

#endif
