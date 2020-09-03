/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MOOSEGMENTFINDERS_MUOSEGMENTFINDERALGS_H
#define MOOSEGMENTFINDERS_MUOSEGMENTFINDERALGS_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MuonPattern/MuonPatternChamberIntersect.h"
#include "MuonPrepRawData/CscPrepDataCollection.h"
#include "MuonPrepRawData/MdtPrepDataCollection.h"
#include "MuonPrepRawData/RpcPrepDataCollection.h"
#include "MuonPrepRawData/TgcPrepDataCollection.h"
#include "MuonRecHelperTools/MuonEDMPrinterTool.h"
#include "MuonRecToolInterfaces/IMuonClusterOnTrackCreator.h"
#include "MuonRecToolInterfaces/IMuonSegmentMaker.h"
#include "MuonRecToolInterfaces/IMuonTruthSummaryTool.h"
#include "MuonSegment/MuonSegmentCombinationCollection.h"
#include "MuonSegmentMakerToolInterfaces/IMuonClusterSegmentFinderTool.h"
#include "MuonSegmentMakerToolInterfaces/IMuonPatternCalibration.h"
#include "MuonSegmentMakerToolInterfaces/IMuonPatternSegmentMaker.h"
#include "MuonSegmentMakerToolInterfaces/IMuonSegmentOverlapRemovalTool.h"
#include "TrkSegment/SegmentCollection.h"
#include "TrkTruthData/PRD_MultiTruthCollection.h"

namespace Muon {
class IMuonClusterSegmentFinder;
}

class MsgStream;
class ICscSegmentFinder;

class MuonSegmentFinderAlg : public AthAlgorithm {
  public:
    MuonSegmentFinderAlg(const std::string& name, ISvcLocator* pSvcLocator);

    virtual ~MuonSegmentFinderAlg();

    virtual StatusCode initialize() override;
    virtual StatusCode execute() override;
    virtual StatusCode finalize() override;


  private:
    ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc{
        this,
        "MuonIdHelperSvc",
        "Muon::MuonIdHelperSvc/MuonIdHelperSvc",
    };

    ToolHandle<Muon::MuonEDMPrinterTool> m_printer{
        this,
        "EDMPrinter",
        "Muon::MuonEDMPrinterTool/MuonEDMPrinterTool",
    };  //<! helper printer tool
    ToolHandle<Muon::IMuonPatternCalibration> m_patternCalibration{
        this,
        "MuonPatternCalibration",
        "Muon::MuonPatternCalibration/MuonPatternCalibration",
    };
    ToolHandle<Muon::IMuonPatternSegmentMaker> m_patternSegmentMaker{
        this,
        "MuonPatternSegmentMaker",
        "Muon::MuonPatternSegmentMaker/MuonPatternSegmentMaker",
    };
    ToolHandle<Muon::IMuonSegmentMaker> m_segmentMaker{
        this,
        "SegmentMaker",
        "Muon::DCMathSegmentMaker/DCMathSegmentMaker",
    };
    ToolHandle<Muon::IMuonClusterSegmentFinder> m_clusterSegMaker{
        this,
        "MuonClusterSegmentFinder",
        "Muon::MuonClusterSegmentFinder/MuonClusterSegmentFinder",
    };
    ToolHandle<Muon::IMuonSegmentOverlapRemovalTool> m_segmentOverlapRemovalTool{
        this,
        "MuonSegmentOverlapRemovalTool",
        "Muon::MuonSegmentOverlapRemovalTool/MuonSegmentOverlapRemovalTool",
    };
    ToolHandle<Muon::IMuonClusterOnTrackCreator> m_clusterCreator{
        this,
        "ClusterCreator",
        "Muon::MuonClusterOnTrackCreator/MuonClusterOnTrackCreator",
    };  //<! pointer to muon cluster rio ontrack creator
    ToolHandle<Muon::IMuonClusterOnTrackCreator> m_mmClusterCreator{
        this,
        "MMClusterCreator",
        "Muon::MMClusterOnTrackCreator/MMClusterOnTrackCreator",
    };  //<! pointer to mm cluster rio ontrack creator
    ToolHandle<Muon::IMuonClusterSegmentFinderTool> m_clusterSegMakerNSW{
        this,
        "MuonClusterSegmentFinderTool",
        "Muon::MuonClusterSegmentFinderTool/MuonClusterSegmentFinderTool",
    };
    ToolHandle<Muon::IMuonTruthSummaryTool> m_truthSummaryTool{
        this,
        "MuonTruthSummaryTool",
        "Muon::MuonTruthSummaryTool/MuonTruthSummaryTool",
    };
    ToolHandle<ICscSegmentFinder> m_csc2dSegmentFinder{
        this,
        "Csc2dSegmentMaker",
        "Csc2dSegmentMaker/Csc2dSegmentMaker",
    };
    ToolHandle<ICscSegmentFinder> m_csc4dSegmentFinder{
        this,
        "Csc4dSegmentMaker",
        "Csc4dSegmentMaker/Csc4dSegmentMaker",
    };


    SG::WriteHandleKey<Trk::SegmentCollection> m_segmentCollectionKey{
        this,
        "SegmentCollectionName",
        "MuonSegments",
        "Muon Segments",
    };
    SG::ReadHandleKey<Muon::CscPrepDataContainer> m_cscPrdsKey{
        this,
        "CSC_clusterkey",
        "CSC_Clusters",
        "CSC PRDs",
    };
    SG::ReadHandleKey<Muon::MdtPrepDataContainer> m_mdtPrdsKey{
        this,
        "MDT_PRDs",
        "MDT_DriftCircles",
        "MDT PRDs",
    };
    SG::ReadHandleKey<Muon::RpcPrepDataContainer> m_rpcPrdsKey{
        this,
        "RPC_PRDs",
        "RPC_Measurements",
        "RPC PRDs",
    };
    SG::ReadHandleKey<Muon::TgcPrepDataContainer> m_tgcPrdsKey{
        this,
        "TGC_PRDs",
        "TGC_Measurements",
        "TGC PRDs",
    };
    SG::ReadHandleKey<MuonPatternCombinationCollection> m_patternCollKey{
        this,
        "MuonLayerHoughCombisKey",
        "MuonLayerHoughCombis",
        "Hough combinations",
    };
    SG::ReadHandleKey<PRD_MultiTruthCollection> m_tgcTruth{
        this,
        "TGCTruth",
        "TGC_TruthMap",
        "TGC PRD Multi-truth Collection",
    };
    SG::ReadHandleKey<PRD_MultiTruthCollection> m_rpcTruth{
        this,
        "RPCTruth",
        "RPC_TruthMap",
        "RPC PRD Multi-truth Collection",
    };

    void createSegmentsWithMDTs(const Muon::MuonPatternCombination* patt, Trk::SegmentCollection* segs,
                                const std::vector<const Muon::RpcPrepDataCollection*> rpcCols,
                                const std::vector<const Muon::TgcPrepDataCollection*> tgcCols);
    void createSegmentsFromClusters(const Muon::MuonPatternCombination* patt, Trk::SegmentCollection* segments);

    bool m_printSummary;

    /** selection flags for cluster based segment finding */
    bool m_doTGCClust;
    bool m_doRPCClust;
    bool m_doClusterTruth;
};

#endif

/*  LocalWords:  ifndef
 */
