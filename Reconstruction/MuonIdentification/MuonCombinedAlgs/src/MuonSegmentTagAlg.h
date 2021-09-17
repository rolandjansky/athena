/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCOMBINEDALGS_MUONSEGMENTAGALG_H
#define MUONCOMBINEDALGS_MUONSEGMENTAGALG_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuonCombinedEvent/InDetCandidateCollection.h"
#include "MuonCombinedEvent/InDetCandidateToTagMap.h"
#include "MuonCombinedToolInterfaces/IMuonSegmentTagTool.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteDecorHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
#include "xAODMuon/MuonSegmentContainer.h"

class MuonSegmentTagAlg : public AthReentrantAlgorithm {
public:
    MuonSegmentTagAlg(const std::string& name, ISvcLocator* pSvcLocator);

    ~MuonSegmentTagAlg() = default;

    virtual StatusCode initialize()override;
    virtual StatusCode execute(const EventContext& ctx) const override;

private:
    ToolHandle<MuonCombined::IMuonSegmentTagTool> m_muonSegmentTagTool{
        this, "MuonSegmentTagTool", "MuonCombined::MuonSegmentTagTool/MuonSegmentTagTool", "Muon segment tag tool"};
    SG::ReadHandleKey<InDetCandidateCollection> m_indetCandidateCollectionName{this, "InDetCandidateLocation", "InDetCandidates",
                                                                               "name of ID candidate collection"};
    SG::ReadHandleKey<xAOD::MuonSegmentContainer> m_muonSegmentCollectionName{this, "MuonSegmentLocation", "xaodMuonSegments",
                                                                              "name of muon segment container"};
    SG::WriteHandleKey<MuonCombined::InDetCandidateToTagMap> m_tagMap{this, "TagMap", "segmentTagMap", "tag map"};
};

#endif
