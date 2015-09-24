/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDET_TRACKCNV_TLP1_H
#define INDET_TRACKCNV_TLP1_H


#include "AthenaPoolCnvSvc/AthenaPoolTopLevelExtTPConverter.h"

#include "InDetTrack_tlp1.h"


#include "InDetEventTPCnv/InDetPrepRawData/PixelClusterCnv_p1.h"
#include "InDetEventTPCnv/InDetPrepRawData/SCT_ClusterCnv_p1.h"
#include "InDetEventTPCnv/InDetPrepRawData/TRT_DriftCircleCnv_p1.h"

#include "InDetEventTPCnv/InDetRIO_OnTrack/PixelClusterOnTrackCnv_p1.h"
#include "InDetEventTPCnv/InDetRIO_OnTrack/SCT_ClusterOnTrackCnv_p1.h"
#include "InDetEventTPCnv/InDetRIO_OnTrack/TRT_DriftCircleOnTrackCnv_p1.h"
#include "InDetEventTPCnv/InDetRIO_OnTrack/SiClusterOnTrackCnv_p1.h"

#include "InDetEventTPCnv/InDetCompetingRIOsOnTrack/CompetingPixelClustersOnTrackCnv_p1.h"
#include "InDetEventTPCnv/InDetCompetingRIOsOnTrack/CompetingSCT_ClustersOnTrackCnv_p1.h"
#include "InDetEventTPCnv/InDetCompetingRIOsOnTrack/CompetingTRT_DriftCirclesOnTrackCnv_p1.h"

#include <iostream>

class InDetTrackCnv_tlp1
   : public AthenaPoolTopLevelExtTPConverter< InDet::Track_tlp1 >
{
public:

  InDetTrackCnv_tlp1();
  virtual ~InDetTrackCnv_tlp1() {}

  virtual unsigned short    getConverterID() { return 1; }
  
  virtual void          setPStorage( InDet::Track_tlp1 *storage );

protected:
  PixelClusterOnTrackCnv_p1                 m_pixelClustersOTCnv;
  SCT_ClusterOnTrackCnv_p1                  m_sctClustersOTCnv;
  TRT_DriftCircleOnTrackCnv_p1              m_trtDriftCirclesOTCnv;
  SiClusterOnTrackCnv_p1                    m_siClustersOTCnv;

  CompetingPixelClustersOnTrackCnv_p1       m_pixelCrotCnv;
  CompetingSCT_ClustersOnTrackCnv_p1        m_sctCrotCnv;
  CompetingTRT_DriftCirclesOnTrackCnv_p1    m_trtCrotCnv;

  PixelClusterCnv_p1     m_pixelClustersCnv;
  SCT_ClusterCnv_p1      m_sctClustersCnv;
  TRT_DriftCircleCnv_p1      m_trtDriftCirclesCnv;
  SiClusterCnv_p1        m_siClustersCnv;
  SiWidthCnv_p1          m_siWidthsCnv;

};


#endif


