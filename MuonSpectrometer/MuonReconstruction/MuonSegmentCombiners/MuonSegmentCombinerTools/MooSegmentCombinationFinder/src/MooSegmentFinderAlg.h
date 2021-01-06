/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MOOSEGMENTFINDERS_MOOSEGMENTFINDERALGS_H
#define MOOSEGMENTFINDERS_MOOSEGMENTFINDERALGS_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/WriteHandleKey.h"
#include "StoreGate/ReadHandleKey.h"

#include "MuonPattern/MuonPatternCombinationCollection.h"
#include "MuonPrepRawData/CscPrepDataCollection.h"
#include "MuonPrepRawData/MdtPrepDataCollection.h"
#include "MuonPrepRawData/MuonPrepDataContainer.h"
#include "MuonPrepRawData/RpcPrepDataCollection.h"
#include "MuonPrepRawData/TgcPrepDataCollection.h"
#include "MuonRecToolInterfaces/HoughDataPerSec.h"
#include "MuonSegment/MuonSegmentCombinationCollection.h"
#include "MuonSegmentMakerToolInterfaces/IMuonClusterSegmentFinder.h"
#include "TrkSegment/SegmentCollection.h"
#include "TrkTruthData/PRD_MultiTruthCollection.h"
#include "MuonSegmentCombinerToolInterfaces/IMooSegmentCombinationFinder.h"
#include "MuonSegmentMakerToolInterfaces/IMuonSegmentOverlapRemovalTool.h"

class MooSegmentFinderAlg : public AthReentrantAlgorithm {
  public:
    MooSegmentFinderAlg(const std::string& name, ISvcLocator* pSvcLocator);

    virtual ~MooSegmentFinderAlg()=default;

    virtual StatusCode initialize() override;
    virtual StatusCode execute(const EventContext& ctx) const override;

  private:
    template <class T, class Y>
      void retrieveCollections(std::vector<const T*>& cols, const SG::ReadHandleKey<Y>& key, const EventContext& ctx) const;

    /** extract segments from a MuonSegmentCombinationCollection */
    Trk::SegmentCollection* extractSegmentCollection(const MuonSegmentCombinationCollection& segmentCombinations) const;

    /** selection flags for all four technologies */
    Gaudi::Property<bool> m_useTgc{this,"UseTGC",true};
    Gaudi::Property<bool> m_useTgcPriorBC{this,"UseTGCPriorBC",false};
    Gaudi::Property<bool> m_useTgcNextBC{this,"UseTGCNextBC",false};
    Gaudi::Property<bool> m_useRpc{this,"UseRPC",true};
    Gaudi::Property<bool> m_useCsc{this,"UseCSC",true};
    Gaudi::Property<bool> m_useMdt{this,"UseMDT",true};

    /** selection flags for cluster based segment finding */
    Gaudi::Property<bool> m_doTGCClust{this,"doTGCClust",false};
    Gaudi::Property<bool> m_doRPCClust{this,"doRPCClust",false};
    Gaudi::Property<bool> m_doClusterTruth{this,"doClusterTruth",false};

    /** storegate location of the MuonPrepDataContainer for all four technologies */
    SG::ReadHandleKey<Muon::TgcPrepDataContainer> m_keyTgc{this,"TgcPrepDataContainer","TGC_Measurements"};
    SG::ReadHandleKey<Muon::TgcPrepDataContainer> m_keyTgcPriorBC{this,"TgcPrepDataContainerPriorBC","TGC_MeasurementsPriorBC"};
    SG::ReadHandleKey<Muon::TgcPrepDataContainer> m_keyTgcNextBC{this,"TgcPrepDataContainerNextBC","TGC_MeasurementsNextBC"};
    SG::ReadHandleKey<Muon::RpcPrepDataContainer> m_keyRpc{this,"RpcPrepDataContainer","RPC_Measurements"};
    SG::ReadHandleKey<Muon::CscPrepDataContainer> m_keyCsc{this,"CscPrepDataContainer","CSC_Clusters"};
    SG::ReadHandleKey<Muon::MdtPrepDataContainer> m_keyMdt{this,"MdtPrepDataContainer","MDT_DriftCircles"};

    SG::ReadHandleKey<PRD_MultiTruthCollection> m_tgcTruth{this,"TGCTruth","TGC_TruthMap","TGC PRD Multi-truth Collection"};
    SG::ReadHandleKey<PRD_MultiTruthCollection> m_rpcTruth{this,"RPCTruth","RPC_TruthMap","RPC PRD Multi-truth Collection"};

    SG::WriteHandleKey<MuonPatternCombinationCollection> m_patternCombiLocation{this,"MuonPatternCombinationLocation","MuonHoughPatternCombinations"};
    SG::WriteHandleKey<Trk::SegmentCollection> m_segmentLocation{this,"MuonSegmentOutputLocation","MooreSegments"};
    SG::WriteHandleKey<Muon::HoughDataPerSectorVec> m_houghDataPerSectorVecKey{this,"Key_MuonLayerHoughToolHoughDataPerSectorVec","HoughDataPerSectorVec","HoughDataPerSectorVec key"};

    ToolHandle<Muon::IMooSegmentCombinationFinder> m_segmentFinder{this,"SegmentFinder","Muon::MooSegmentCombinationFinder/MooSegmentCombinationFinder","pointer to the segment finder"};
    ToolHandle<Muon::IMuonClusterSegmentFinder> m_clusterSegMaker{this,"MuonClusterSegmentFinderTool","Muon::MuonClusterSegmentFinder/MuonClusterSegmentFinder"};
    ToolHandle<Muon::IMuonSegmentOverlapRemovalTool> m_overlapRemovalTool{this,"SegmentOverlapRemovalTool","Muon::MuonSegmentOverlapRemovalTool/MuonSegmentOverlapRemovalTool","tool to removal overlaps in segment combinations"};
};

template <class T, class Y>
void
  MooSegmentFinderAlg::retrieveCollections(std::vector<const T*>& cols, const SG::ReadHandleKey<Y>& key, const EventContext& ctx) const
{

  SG::ReadHandle<Y> cscPrds(key, ctx);
    if (cscPrds.isValid() == false) {
        ATH_MSG_ERROR("Cannot retrieve Container " << key.key() << " accessing via collections ");

    } else {
        const Y* ptr = cscPrds.cptr();
        cols.reserve(cols.size() + ptr->size());
        for (auto p : *ptr)
            if (!p->empty()) cols.push_back(p);
        ATH_MSG_VERBOSE("Retrieved " << cscPrds.key() << " Container " << cols.size());
    }
}


#endif
