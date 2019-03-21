/*
 Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

// Local include(s):
#include "MuonEfficiencyCorrections_TestAlg.h"
#include "PATInterfaces/SystematicsTool.h"
#include <cmath>

#include "xAODMuon/MuonContainer.h"
#include "xAODMuon/MuonAuxContainer.h"

namespace CP {

    MuonEfficiencyCorrections_TestAlg::MuonEfficiencyCorrections_TestAlg(const std::string& name, ISvcLocator* svcLoc) :
                AthAlgorithm(name, svcLoc),
                m_sgKey("Muons"),
                m_histSvc("THistSvc/THistSvc", name),
                m_effi_SF_tools(),
                m_comparison_tools(),
                m_prw_Tool("CP::PileupReweighting/PileupReweightingTool", this),
                m_sel_tool("CP::MuonSelectionTool/SelectionTool",this),
                m_test_helper(),
                m_comparison_helper(),
                m_first_release_name("First"),
                m_second_release_name("Second"),                
                m_pt_cut(-1),
                m_eta_cut(-1),
                m_evNumber(0) {
        declareProperty("SGKey", m_sgKey = "Muons");
        // prepare the handle
        declareProperty("PileupReweightingTool", m_prw_Tool);
        declareProperty("MuonSelectionTool", m_sel_tool);
        
        declareProperty("EfficiencyTools", m_effi_SF_tools);
        declareProperty("EfficiencyToolsForComparison", m_comparison_tools);

        declareProperty("DefaultRelease", m_first_release_name);
        declareProperty("ValidationRelease", m_second_release_name);

        declareProperty("MinPt", m_pt_cut);
        declareProperty("MaxEta", m_eta_cut);
        
        // force strict checking of return codes
        CP::SystematicCode::enableFailure();
        CP::CorrectionCode::enableFailure();
    }

    StatusCode MuonEfficiencyCorrections_TestAlg::initialize() {
        ATH_MSG_DEBUG("SGKey = " << m_sgKey);
        ATH_CHECK(m_prw_Tool.retrieve());
        ATH_CHECK(m_sel_tool.retrieve());
        ATH_MSG_DEBUG("PileupReweightingTool  = " << m_prw_Tool);
        if (m_effi_SF_tools.empty()) {
            ATH_MSG_FATAL("Please provide at least one muon sf- tool");
            return StatusCode::FAILURE;
        }
        if (m_comparison_tools.empty()) {
            m_test_helper = std::make_unique < TestMuonSF::MuonSFTestHelper > ();
        } else {
            m_test_helper = std::make_unique < TestMuonSF::MuonSFTestHelper > (m_first_release_name);
            m_comparison_helper = std::make_unique < TestMuonSF::MuonSFTestHelper> (m_test_helper->tree(), m_second_release_name);
            for (auto & SFTool : m_comparison_tools) {
                ATH_CHECK(SFTool.retrieve());
                m_comparison_helper->addTool(SFTool);
            }
        }
        m_test_helper->setSelectionTool(m_sel_tool);
        m_test_helper->tree()->Branch("eventNumber", &m_evNumber);
        for (auto & SFTool : m_effi_SF_tools) {
            ATH_CHECK(SFTool.retrieve());
            m_test_helper->addTool(SFTool);
        }
        if (!m_test_helper->init()) {
            ATH_MSG_FATAL("Failed to initialize");
            return StatusCode::FAILURE;
        }
        if (m_comparison_helper && !m_comparison_helper->init()) {
            ATH_MSG_FATAL("Failed to initialize");
            return StatusCode::FAILURE;
        }
        ATH_CHECK(m_histSvc->regTree(std::string("/MUONEFFTESTER/") + m_test_helper->tree()->GetName(), m_test_helper->tree()));
        return StatusCode::SUCCESS;
    }

    StatusCode MuonEfficiencyCorrections_TestAlg::execute() {

        // Retrieve the muons:
        const xAOD::MuonContainer* muons = 0;
        ATH_CHECK(evtStore()->retrieve(muons, m_sgKey));
        // Retrieve the EventInfo:
        const xAOD::EventInfo* ei = 0;
        ATH_CHECK(evtStore()->retrieve(ei, "EventInfo"));
        //Apply the prwTool first before calling the efficiency correction methods
        ATH_CHECK(m_prw_Tool->apply(*ei));
        m_evNumber = ei->eventNumber();

        for (const auto& mu : *muons) {
            if (mu->pt() < m_pt_cut || (m_eta_cut > 0 && std::fabs(mu->eta()) >= m_eta_cut)) continue;
            // reject all loose muons
            if (m_sel_tool->getQuality(*mu) > xAOD::Muon::Loose) continue;
            if (m_test_helper->fill(mu) != CP::CorrectionCode::Ok) return EXIT_FAILURE;
            if (m_comparison_helper && m_comparison_helper->fill(mu) != CP::CorrectionCode::Ok) return EXIT_FAILURE;
            m_test_helper->fillTree();
        }
        // Return gracefully:
        return StatusCode::SUCCESS;
    }

} // namespace CP
