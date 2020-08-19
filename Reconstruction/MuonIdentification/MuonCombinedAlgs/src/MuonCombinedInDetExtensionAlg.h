/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCOMBINEDALGS_MUONCOMBINEDINDETEXTENSIONALG_H
#define MUONCOMBINEDALGS_MUONCOMBINEDINDETEXTENSIONALG_H

#include <string>

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuonCombinedEvent/InDetCandidateCollection.h"
#include "MuonCombinedEvent/InDetCandidateToTagMap.h"
#include "MuonPrepRawData/MMPrepDataContainer.h"
#include "MuonPrepRawData/MuonPrepDataContainer.h"
#include "MuonPrepRawData/sTgcPrepDataContainer.h"
#include "StoreGate/ReadHandleKey.h"
#include "TrkSegment/SegmentCollection.h"
#include "TrkTrack/TrackCollection.h"

namespace MuonCombined {
class IMuonCombinedInDetExtensionTool;
}

class MuonCombinedInDetExtensionAlg : public AthAlgorithm {
  public:
    MuonCombinedInDetExtensionAlg(const std::string& name, ISvcLocator* pSvcLocator);

    ~MuonCombinedInDetExtensionAlg();

    StatusCode initialize();
    StatusCode execute();
    StatusCode finalize();

  private:
    ToolHandleArray<MuonCombined::IMuonCombinedInDetExtensionTool> m_muonCombinedInDetExtensionTools;
    SG::ReadHandleKey<InDetCandidateCollection>                    m_indetCandidateCollectionName{
        this,
        "InDetCandidateLocation",
        "InDetCandidates",
        "ID candidates",
    };
    SG::ReadHandleKey<Muon::MdtPrepDataContainer> m_MDT_ContainerName{
        this,
        "MDTPrepDataLocation",
        "MDT_DriftCircles",
        "MDT prep data",
    };
    SG::ReadHandleKey<Muon::CscPrepDataContainer> m_CSC_ContainerName{
        this,
        "CSCPrepDataLocation",
        "CSC_Clusters",
        "CSC prep data",
    };
    SG::ReadHandleKey<Muon::RpcPrepDataContainer> m_RPC_ContainerName{
        this,
        "RPCPrepDataLocation",
        "RPC_Measurements",
        "RPC prep data",
    };
    SG::ReadHandleKey<Muon::TgcPrepDataContainer> m_TGC_ContainerName{
        this,
        "TGCPrepDataLocation",
        "TGC_Measurements",
        "TGC prep data",
    };
    SG::ReadHandleKey<Muon::sTgcPrepDataContainer> m_sTGC_ContainerName{
        this,
        "sTGCPrepDataLocation",
        "STGC_Measurements",
        "sTGC prep data",
    };
    SG::ReadHandleKey<Muon::MMPrepDataContainer> m_MM_ContainerName{
        this,
        "MMPrepDataLocation",
        "MM_Measurements",
        "MM prep data",
    };
    SG::WriteHandleKey<MuonCombined::InDetCandidateToTagMap> m_tagMap{
        this,
        "TagMap",
        "muGirlTagMap",
        "tag map",
    };
    SG::WriteHandleKey<TrackCollection> m_combTracks{
        this,
        "CombinedTrackCollection",
        "MuGirlCombinedTracks",
        "combined track collection",
    };
    SG::WriteHandleKey<TrackCollection> m_METracks{
        this,
        "METrackCollection",
        "MuGirlMETracks",
        "ME track collection",
    };
    SG::WriteHandleKey<Trk::SegmentCollection> m_segments{
        this,
        "SegmentCollection",
        "MuGirlSegments",
        "Segment collection",
    };

    bool m_usePRDs;
    bool m_hasCSC;
    bool m_hasSTGC;
    bool m_hasMM;
};


#endif
