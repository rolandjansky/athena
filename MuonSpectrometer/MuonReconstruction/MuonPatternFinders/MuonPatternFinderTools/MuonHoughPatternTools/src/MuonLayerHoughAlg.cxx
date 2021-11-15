/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonLayerHoughAlg.h"

#include "MuonPrepRawData/CscPrepDataCollection.h"
#include "MuonPrepRawData/MMPrepDataCollection.h"
#include "MuonPrepRawData/MdtPrepDataCollection.h"
#include "MuonPrepRawData/MuonPrepDataContainer.h"
#include "MuonPrepRawData/RpcPrepDataCollection.h"
#include "MuonPrepRawData/TgcPrepDataCollection.h"
#include "MuonPrepRawData/sTgcPrepDataCollection.h"

MuonLayerHoughAlg::MuonLayerHoughAlg(const std::string& name, ISvcLocator* pSvcLocator) : AthReentrantAlgorithm(name, pSvcLocator) {}

StatusCode MuonLayerHoughAlg::initialize() {
    if (m_layerTool.empty()) {
        ATH_MSG_ERROR("MuonLayerScanTool property is empty");
        return StatusCode::FAILURE;
    }
    ATH_CHECK(m_layerTool.retrieve());
    ATH_CHECK(m_printer.retrieve());
    ATH_CHECK(m_keyRpc.initialize());
    ATH_CHECK(m_keyMdt.initialize());
    ATH_CHECK(m_keyTgc.initialize());
    if (!m_keyCsc.empty()) ATH_CHECK(m_keyCsc.initialize());
    if (!m_keysTgc.empty()) ATH_CHECK(m_keysTgc.initialize());
    if (!m_keyMM.empty()) ATH_CHECK(m_keyMM.initialize());
    ATH_CHECK(m_combis.initialize());
    ATH_CHECK(m_houghDataPerSectorVecKey.initialize());

    return StatusCode::SUCCESS;
}

StatusCode MuonLayerHoughAlg::execute(const EventContext& ctx) const {
    const Muon::RpcPrepDataContainer* rpcPrds = GetObject(m_keyRpc, ctx);
    const Muon::MdtPrepDataContainer* mdtPrds = GetObject(m_keyMdt, ctx);
    const Muon::TgcPrepDataContainer* tgcPrds = GetObject(m_keyTgc, ctx);
    const Muon::CscPrepDataContainer* cscPrds = m_keyCsc.empty() ? nullptr : GetObject(m_keyCsc, ctx);
    const Muon::sTgcPrepDataContainer* stgcPrds = m_keysTgc.empty() ? nullptr : GetObject(m_keysTgc, ctx);
    const Muon::MMPrepDataContainer* mmPrds = m_keyMM.empty() ? nullptr : GetObject(m_keyMM, ctx);
    ATH_MSG_VERBOSE("calling layer tool ");
    auto [combis, houghDataPerSectorVec] = m_layerTool->find(mdtPrds, cscPrds, tgcPrds, rpcPrds, stgcPrds, mmPrds, ctx);
    SG::WriteHandle<MuonPatternCombinationCollection> Handle(m_combis, ctx);
    if (combis) {
        if (Handle.record(std::move(combis)).isFailure()) {
            ATH_MSG_WARNING("Failed to record MuonPatternCombinationCollection at MuonLayerHoughCombis");
        } else {
            ATH_MSG_DEBUG("Recorded MuonPatternCombinationCollection at MuonLayerHoughCombis: size " << Handle->size());
            if (m_printSummary || msgLvl(MSG::DEBUG)) {
                ATH_MSG_DEBUG("Number of MuonPatternCombinations  " << Handle->size() << std::endl << m_printer->print(*Handle));
            }
        }
    } else {
        ATH_MSG_VERBOSE("CombinationCollection " << m_combis << " is empty, recording");
        ATH_CHECK(Handle.record(std::make_unique<MuonPatternCombinationCollection>()));
    }

    // write hough data to SG
    SG::WriteHandle<Muon::HoughDataPerSectorVec> handle{m_houghDataPerSectorVecKey, ctx};
    if (houghDataPerSectorVec) {
        ATH_CHECK(handle.record(std::move(houghDataPerSectorVec)));
    } else {
        ATH_MSG_VERBOSE("HoughDataPerSectorVec " << m_houghDataPerSectorVecKey << " is empty, recording");
        ATH_CHECK(handle.record(std::make_unique<Muon::HoughDataPerSectorVec>()));
    }
    return StatusCode::SUCCESS;
}  // execute
