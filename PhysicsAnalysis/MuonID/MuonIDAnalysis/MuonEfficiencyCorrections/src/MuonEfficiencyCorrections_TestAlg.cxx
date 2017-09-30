/*
 Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
                    m_prw_Tool("CP::PileupReweighting/PileupReweightingTool", this),
                    m_test_helper() {
        declareProperty("SGKey", m_sgKey = "Muons");
        // prepare the handle
        declareProperty("PileupReweightingTool", m_prw_Tool);
        // force strict checking of return codes
        CP::SystematicCode::enableFailure();
        CP::CorrectionCode::enableFailure();
    }

    StatusCode MuonEfficiencyCorrections_TestAlg::initialize() {
        ATH_MSG_DEBUG("SGKey = " << m_sgKey);
        ATH_CHECK(m_prw_Tool.retrieve());
        ATH_MSG_DEBUG("PileupReweightingTool  = " << m_prw_Tool);
        if (m_effi_SF_tools.empty()){
            ATH_MSG_FATAL("Please provide at least one muon sf- tool");
            return StatusCode::FAILURE;
        }
        m_test_helper = std::unique_ptr<TestMuonSF::MuonSFTestHelper> (new TestMuonSF::MuonSFTestHelper());
        for (auto & SFTool : m_effi_SF_tools){
            ATH_CHECK(SFTool.retrieve());
            m_test_helper->addTool(SFTool);
        }
        if (!m_test_helper->init()){
            ATH_MSG_FATAL("Failed to initialize");
            return StatusCode::FAILURE;
        }
        return StatusCode::SUCCESS;
    }

    StatusCode MuonEfficiencyCorrections_TestAlg::execute() {

        // Retrieve the muons:
        const xAOD::MuonContainer* muons = 0;
        ATH_CHECK(evtStore()->retrieve(muons, m_sgKey));
        ATH_MSG_INFO("Number of muons: " << muons->size());

        // Retrieve the EventInfo:
        const xAOD::EventInfo* ei = 0;
        ATH_CHECK(evtStore()->retrieve(ei, "EventInfo"));
        //Apply the prwTool first before calling the efficiency correction methods
        ATH_CHECK(m_prw_Tool->apply(*ei));

        
        // Return gracefully:
        return StatusCode::SUCCESS;
    }

} // namespace CP
