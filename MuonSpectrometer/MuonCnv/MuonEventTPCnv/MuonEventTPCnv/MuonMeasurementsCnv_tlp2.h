/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONMEASUREMENTSCNV_TLP2_H
#define MUONMEASUREMENTSCNV_TLP2_H
#include "AthenaPoolCnvSvc/AthenaPoolTopLevelExtTPConverter.h"
#include "MuonMeasurements_tlp2.h"
#include "MuonEventTPCnv/MuonRIO_OnTrack/CscClusterOnTrackCnv_p2.h"
#include "MuonEventTPCnv/MuonRIO_OnTrack/RpcClusterOnTrackCnv_p3.h"
#include "MuonEventTPCnv/MuonRIO_OnTrack/TgcClusterOnTrackCnv_p2.h"
#include "MuonEventTPCnv/MuonRIO_OnTrack/MdtDriftCircleOnTrackCnv_p2.h"
#include "MuonEventTPCnv/MuonRIO_OnTrack/MM_ClusterOnTrackCnv_p1.h"
#include "MuonEventTPCnv/MuonRIO_OnTrack/STGC_ClusterOnTrackCnv_p1.h"
#include "MuonEventTPCnv/MuonCompetingRIOsOnTrack/CompetingMuonClustersOnTrackCnv_p2.h"
#include "MuonEventTPCnv/MuonSegment/MuonSegmentCnv_p2.h"
#include "MuonEventTPCnv/MuonSegment/MuonSegmentQualityCnv_p1.h"

class MuonMeasurementsCnv_tlp2
   : public AthenaPoolTopLevelExtTPConverter< TPCnv::MuonMeasurements_tlp2 >
{
public:

  MuonMeasurementsCnv_tlp2();
  virtual ~MuonMeasurementsCnv_tlp2() {}

  virtual unsigned short            getConverterID() { return 2; } // This is the number of the extending convertor (i.e. ID version is 1)
  
  virtual void                      setPStorage( TPCnv::MuonMeasurements_tlp2 *storage );

protected:
    CscClusterOnTrackCnv_p2         m_cscClustersOTCnv;
    RpcClusterOnTrackCnv_p3         m_rpcClustersOTCnv;
    TgcClusterOnTrackCnv_p2         m_tgcClustersOTCnv;
    MdtDriftCircleOnTrackCnv_p2     m_mdtDriftCirclesOTCnv;
    MuonSegmentCnv_p2               m_muonSegmentsCnv;
    MuonSegmentQualityCnv_p1        m_muonSegmentQualitiesCnv;
    CompetingMuonClustersOnTrackCnv_p2  m_muonCompetingROTsCnv;
    MM_ClusterOnTrackCnv_p1         m_mmClustersOTCnv;
    STGC_ClusterOnTrackCnv_p1       m_stgcClustersOTCnv;
};
#endif



