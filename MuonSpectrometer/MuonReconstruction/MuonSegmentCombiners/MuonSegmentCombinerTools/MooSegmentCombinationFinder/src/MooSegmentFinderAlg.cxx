/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MooSegmentFinderAlg.h"

#include "MuonPrepRawData/MuonPrepDataContainer.h"
#include "MuonSegment/MuonSegment.h"

MooSegmentFinderAlg::MooSegmentFinderAlg(const std::string& name, ISvcLocator* pSvcLocator) : AthReentrantAlgorithm(name, pSvcLocator) {}

StatusCode MooSegmentFinderAlg::initialize() {
    ATH_CHECK(m_segmentFinder.retrieve());
    ATH_CHECK(m_clusterSegMaker.retrieve());
    ATH_CHECK(m_overlapRemovalTool.retrieve());

    ATH_CHECK(m_keyMdt.initialize(m_useMdt));  // Nullify key from scheduler if not needed
    ATH_CHECK(m_keyCsc.initialize(m_useCsc));
    ATH_CHECK(m_keyRpc.initialize(m_useRpc));
    ATH_CHECK(m_keyTgcPriorBC.initialize(m_useTgcPriorBC));
    ATH_CHECK(m_keyTgcNextBC.initialize(m_useTgcNextBC));
    ATH_CHECK(m_keyTgc.initialize(m_useTgc));

    ATH_CHECK(m_tgcTruth.initialize(m_doClusterTruth));
    ATH_CHECK(m_rpcTruth.initialize(m_doClusterTruth));

    ATH_CHECK(m_patternCombiLocation.initialize());
    ATH_CHECK(m_segmentLocation.initialize());
    ATH_CHECK(m_houghDataPerSectorVecKey.initialize(!m_houghDataPerSectorVecKey.empty()));

    return StatusCode::SUCCESS;
}

StatusCode MooSegmentFinderAlg::execute(const EventContext& ctx) const {
    std::vector<const Muon::MdtPrepDataCollection*> mdtCols;
    std::vector<const Muon::CscPrepDataCollection*> cscCols;
    std::vector<const Muon::TgcPrepDataCollection*> tgcCols;
    std::vector<const Muon::RpcPrepDataCollection*> rpcCols;
    if (m_useMdt) retrieveCollections(mdtCols, m_keyMdt, ctx);
    if (m_useCsc) retrieveCollections(cscCols, m_keyCsc, ctx);
    if (m_useTgc) retrieveCollections(tgcCols, m_keyTgc, ctx);
    if (m_useTgcPriorBC) retrieveCollections(tgcCols, m_keyTgcPriorBC, ctx);
    if (m_useTgcNextBC) retrieveCollections(tgcCols, m_keyTgcNextBC, ctx);
    if (m_useRpc) retrieveCollections(rpcCols, m_keyRpc, ctx);

    Muon::IMooSegmentCombinationFinder::Output output;

    SG::WriteHandle<Trk::SegmentCollection> segHandle(m_segmentLocation, ctx);

    if (segHandle.record(std::make_unique<Trk::SegmentCollection>()).isSuccess()) {
        ATH_MSG_VERBOSE("stored MuonSegmentCollection at " << m_segmentLocation.key());
    } else {
        ATH_MSG_ERROR("Failed to store MuonSegmentCollection ");
    }
    output.segmentCollection = segHandle.ptr();

    SG::WriteHandle<MuonPatternCombinationCollection> patHandle(m_patternCombiLocation, ctx);

    m_segmentFinder->findSegments(mdtCols, cscCols, tgcCols, rpcCols, output, ctx);

    if (output.patternCombinations) {
        if (patHandle.record(std::move(output.patternCombinations)).isSuccess()) {
            ATH_MSG_VERBOSE("stored MuonPatternCombinationCollection at " << m_patternCombiLocation.key());
        } else {
            ATH_MSG_ERROR("Failed to store MuonPatternCombinationCollection at " << m_patternCombiLocation.key());
        }
    } else {
        if (patHandle.record(std::make_unique<MuonPatternCombinationCollection>()).isSuccess()) {
            ATH_MSG_VERBOSE("stored MuonPatternCombinationCollection at " << m_patternCombiLocation.key());
        } else {
            ATH_MSG_ERROR("Failed to store MuonPatternCombinationCollection at " << m_patternCombiLocation.key());
        }
    }

    // write hough data to SG
    if (!m_houghDataPerSectorVecKey.empty()) {
        SG::WriteHandle<Muon::HoughDataPerSectorVec> handle{m_houghDataPerSectorVecKey, ctx};
        if (output.houghDataPerSectorVec) {
            ATH_CHECK(handle.record(std::move(output.houghDataPerSectorVec)));
        } else {
            ATH_CHECK(handle.record(std::make_unique<Muon::HoughDataPerSectorVec>()));
            ATH_MSG_VERBOSE("HoughDataPerSectorVec was empty, key: " << m_houghDataPerSectorVecKey.key());
        }
    }

    // do cluster based segment finding
    if (m_doTGCClust || m_doRPCClust) {
        const PRD_MultiTruthCollection* tgcTruthColl = nullptr;
        const PRD_MultiTruthCollection* rpcTruthColl = nullptr;
        if (m_doClusterTruth) {
            SG::ReadHandle<PRD_MultiTruthCollection> tgcTruth(m_tgcTruth, ctx);
            SG::ReadHandle<PRD_MultiTruthCollection> rpcTruth(m_rpcTruth, ctx);
            tgcTruthColl = tgcTruth.cptr();
            rpcTruthColl = rpcTruth.cptr();
        }
        SG::ReadHandle<Muon::MdtPrepDataContainer> mdth(m_keyMdt, ctx);
        m_clusterSegMaker->getClusterSegments(mdth.cptr(), m_doTGCClust ? &tgcCols : nullptr, m_doRPCClust ? &rpcCols : nullptr, tgcTruthColl,
                                              rpcTruthColl, segHandle.ptr());
    }

    m_overlapRemovalTool->removeDuplicates(*segHandle);

    return StatusCode::SUCCESS;
}  // execute
