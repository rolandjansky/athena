/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDET_TRACKCNV_tlp2_H
#define INDET_TRACKCNV_tlp2_H


#include "AthenaPoolCnvSvc/AthenaPoolTopLevelExtTPConverter.h"

#include "InDetTrack_tlp2.h"

#include "InDetEventTPCnv/InDetRIO_OnTrack/PixelClusterOnTrackCnv_p2.h"
#include "InDetEventTPCnv/InDetRIO_OnTrack/SCT_ClusterOnTrackCnv_p2.h"
// #include "InDetEventTPCnv/InDetRIO_OnTrack/SiClusterOnTrackCnv_p2.h"
#include "InDetEventTPCnv/InDetRIO_OnTrack/TRT_DriftCircleOnTrackCnv_p2.h"

#include "InDetEventTPCnv/InDetCompetingRIOsOnTrack/CompetingPixelClustersOnTrackCnv_p1.h"
#include "InDetEventTPCnv/InDetCompetingRIOsOnTrack/CompetingSCT_ClustersOnTrackCnv_p1.h"
#include "InDetEventTPCnv/InDetCompetingRIOsOnTrack/CompetingTRT_DriftCirclesOnTrackCnv_p1.h"

#include <iostream>

class InDetTrackCnv_tlp2
    : public AthenaPoolTopLevelExtTPConverter< InDet::Track_tlp2 >
{
public:

    InDetTrackCnv_tlp2();
    virtual ~InDetTrackCnv_tlp2() {}

    virtual unsigned short    getConverterID() { return 1; }

    virtual void          setPStorage( InDet::Track_tlp2 *storage );

protected:
    PixelClusterOnTrackCnv_p2                 m_pixelClustersOTCnv;
    SCT_ClusterOnTrackCnv_p2                  m_sctClustersOTCnv;
    TRT_DriftCircleOnTrackCnv_p2              m_trtDriftCirclesOTCnv;
    // SiClusterOnTrackCnv_p2                    m_siClustersOTCnv;

    CompetingPixelClustersOnTrackCnv_p1       m_pixelCrotCnv;
    CompetingSCT_ClustersOnTrackCnv_p1        m_sctCrotCnv;
    CompetingTRT_DriftCirclesOnTrackCnv_p1    m_trtCrotCnv;


};


#endif


