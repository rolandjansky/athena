/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TPCNV_MUONMEASUREMENTS_TLP1_H
#define TPCNV_MUONMEASUREMENTS_TLP1_H

//-----------------------------------------------------------------------------
// MuonRIO_OnTrack
//-----------------------------------------------------------------------------
#include "MuonEventTPCnv/MuonRIO_OnTrack/MuonClusterOnTrack_p1.h"
#include "MuonEventTPCnv/MuonRIO_OnTrack/CscClusterOnTrack_p1.h"
#include "MuonEventTPCnv/MuonRIO_OnTrack/RpcClusterOnTrack_p1.h"
#include "MuonEventTPCnv/MuonRIO_OnTrack/TgcClusterOnTrack_p1.h"
#include "MuonEventTPCnv/MuonRIO_OnTrack/MdtDriftCircleOnTrack_p1.h"
#include "MuonEventTPCnv/MuonCompetingRIOsOnTrack/CompetingMuonClustersOnTrackCnv_p1.h"
#include <vector>

// ------------
// Muon Segment
// ------------

#include "MuonEventTPCnv/MuonSegment/MuonSegment_p1.h"
#include "MuonEventTPCnv/MuonSegment/MuonSegmentQuality_p1.h"



//-----------------------------------------------------------------------------
// MuonPrepRawData
//-----------------------------------------------------------------------------
//#include "MuonEventTPCnv/MuonPrepRawData/CscPrepData_p1.h"
//#include "MuonEventTPCnv/MuonPrepRawData/RpcPrepData_p1.h"
//#include "MuonEventTPCnv/MuonPrepRawData/TgcPrepData_p1.h"
//#include "MuonEventTPCnv/MuonPrepRawData/MdtPrepData_p1.h"

namespace TPCnv
{
    /** This object contains the muon "extensions" for e.g. Trk::Track (Trk::RIO_OnTrack) and Trk::Segment. 
    It could be split into sub-objects (i.e. one for Trk::Track and one for Trk::Segment) but that would 
    be more work. For the moment this will do */
    class MuonMeasurements_tlp1
    {
    public:
        MuonMeasurements_tlp1() {}
        std::vector< Muon::MuonClusterOnTrack_p1 >          m_muonClustersOT;
        std::vector< Muon::CscClusterOnTrack_p1 >           m_cscClustersOT;
        std::vector< Muon::RpcClusterOnTrack_p1 >           m_rpcClustersOT;
        std::vector< Muon::TgcClusterOnTrack_p1 >           m_tgcClustersOT;
        std::vector< Muon::MdtDriftCircleOnTrack_p1 >       m_mdtDriftCirclesOT;        
        std::vector< Muon::CompetingMuonClustersOnTrack_p1> m_muonCompetingROT;
        std::vector< Muon::MuonSegment_p1>                  m_muonSegments;
        std::vector< Muon::MuonSegmentQuality_p1>           m_muonSegmentQualities;
    };
}

#endif
