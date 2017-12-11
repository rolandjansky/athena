/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TPCNV_MUONMEASUREMENTS_TLP2_H
#define TPCNV_MUONMEASUREMENTS_TLP2_H

//-----------------------------------------------------------------------------
// MuonRIO_OnTrack
//-----------------------------------------------------------------------------
#include "MuonEventTPCnv/MuonRIO_OnTrack/CscClusterOnTrack_p2.h"
#include "MuonEventTPCnv/MuonRIO_OnTrack/RpcClusterOnTrack_p3.h"
#include "MuonEventTPCnv/MuonRIO_OnTrack/TgcClusterOnTrack_p2.h"
#include "MuonEventTPCnv/MuonRIO_OnTrack/MdtDriftCircleOnTrack_p2.h"
#include "MuonEventTPCnv/MuonRIO_OnTrack/MM_ClusterOnTrack_p1.h"
#include "MuonEventTPCnv/MuonRIO_OnTrack/STGC_ClusterOnTrack_p1.h"
#include "MuonEventTPCnv/MuonCompetingRIOsOnTrack/CompetingMuonClustersOnTrack_p1.h"
#include <vector>

// ------------
// Muon Segment
// ------------

#include "MuonEventTPCnv/MuonSegment/MuonSegment_p1.h"
#include "MuonEventTPCnv/MuonSegment/MuonSegmentQuality_p1.h"

namespace TPCnv
{
    /** This object contains the muon "extensions" for e.g. Trk::Track (Trk::RIO_OnTrack) and Trk::Segment. */
    class MuonMeasurements_tlp2
    {
    public:
        MuonMeasurements_tlp2() {}
        std::vector< Muon::CscClusterOnTrack_p2 >           m_cscClustersOT;
        std::vector< Muon::RpcClusterOnTrack_p3 >           m_rpcClustersOT;
        std::vector< Muon::TgcClusterOnTrack_p2 >           m_tgcClustersOT;
        std::vector< Muon::MdtDriftCircleOnTrack_p2 >       m_mdtDriftCirclesOT;        
        std::vector< Muon::CompetingMuonClustersOnTrack_p1> m_muonCompetingROT;
        std::vector< Muon::MuonSegment_p1>                  m_muonSegments;
        std::vector< Muon::MuonSegmentQuality_p1>           m_muonSegmentQualities;
        std::vector< Muon::MM_ClusterOnTrack_p1 >           m_mmClustersOT;
        std::vector< Muon::STGC_ClusterOnTrack_p1 >         m_stgcClustersOT;
    };
}

#endif
