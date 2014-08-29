/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONMEASUREMENTSCNV_TLP1_H
#define MUONMEASUREMENTSCNV_TLP1_H


#include "AthenaPoolCnvSvc/AthenaPoolTopLevelExtTPConverter.h"

#include "MuonMeasurements_tlp1.h"

#include "MuonEventTPCnv/MuonRIO_OnTrack/MuonClusterOnTrackCnv_p1.h"
#include "MuonEventTPCnv/MuonRIO_OnTrack/CscClusterOnTrackCnv_p1.h"
#include "MuonEventTPCnv/MuonRIO_OnTrack/RpcClusterOnTrackCnv_p1.h"
#include "MuonEventTPCnv/MuonRIO_OnTrack/TgcClusterOnTrackCnv_p1.h"
#include "MuonEventTPCnv/MuonRIO_OnTrack/MdtDriftCircleOnTrackCnv_p1.h"
#include "MuonEventTPCnv/MuonCompetingRIOsOnTrack/CompetingMuonClustersOnTrackCnv_p1.h"

#include "MuonEventTPCnv/MuonSegment/MuonSegmentCnv_p1.h"
#include "MuonEventTPCnv/MuonSegment/MuonSegmentQualityCnv_p1.h"


#include <iostream>

class MuonMeasurementsCnv_tlp1
   : public AthenaPoolTopLevelExtTPConverter< TPCnv::MuonMeasurements_tlp1 >
{
public:

  MuonMeasurementsCnv_tlp1();
  virtual ~MuonMeasurementsCnv_tlp1() {}

  virtual unsigned short            getConverterID() { return 2; } // This is the number of the extending convertor (i.e. ID version is 1)
  
  virtual void                      setPStorage( TPCnv::MuonMeasurements_tlp1 *storage );

protected:
    MuonClusterOnTrackCnv_p1        m_muonClustersOTCnv;
    CscClusterOnTrackCnv_p1         m_cscClustersOTCnv;
    RpcClusterOnTrackCnv_p1         m_rpcClustersOTCnv;
    TgcClusterOnTrackCnv_p1         m_tgcClustersOTCnv;
    MdtDriftCircleOnTrackCnv_p1     m_mdtDriftCirclesOTCnv;
    MuonSegmentCnv_p1               m_muonSegmentsCnv;
    MuonSegmentQualityCnv_p1        m_muonSegmentQualitiesCnv;
    CompetingMuonClustersOnTrackCnv_p1  m_muonCompetingROTsCnv;
};


#endif


