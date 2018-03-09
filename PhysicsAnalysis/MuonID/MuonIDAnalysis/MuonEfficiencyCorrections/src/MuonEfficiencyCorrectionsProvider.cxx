/*
 Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

#include "xAODMuon/MuonContainer.h"

// note: stolen from CPToolTests example!
// test algorithm using the MuonEfficiencyScaleFactors class

// Local include(s):
#include "MuonEfficiencyCorrectionsProvider.h"

namespace CP {

    MuonEfficiencyCorrectionsProvider::MuonEfficiencyCorrectionsProvider(const std::string& name, ISvcLocator* svcLoc) :
                    AthAlgorithm(name, svcLoc),
                    m_sf_Tool("CP::MuonEfficiencyScaleFactors/MuonEfficiencyScaleFactors") {

        declareProperty("Input", m_sgKey = "Muons");
        declareProperty("Tool", m_sf_Tool);

    }

    StatusCode MuonEfficiencyCorrectionsProvider::initialize() {
        ATH_CHECK(m_sf_Tool.retrieve());
        return StatusCode::SUCCESS;
    }

    StatusCode MuonEfficiencyCorrectionsProvider::execute() {

        // Retrieve the muons:
        const xAOD::MuonContainer* muons = 0;
        ATH_CHECK(evtStore()->retrieve(muons, m_sgKey));

        for (auto mu : *muons) {
            // decorate the muon with SF info
            if (m_sf_Tool->applyEfficiencyScaleFactor(*mu) == CorrectionCode::Error || m_sf_Tool->applyDataEfficiency(*mu) == CorrectionCode::Error) {
                ATH_MSG_WARNING("Couldn't run efficiencies on muon!");
                continue;
            }
            ATH_MSG_VERBOSE(" efficiency = " << mu->auxdataConst<float>("Efficiency") << " scale factor=" << mu->auxdataConst<float>("EfficiencyScaleFactor"));
        }

        // Return gracefully:
        return StatusCode::SUCCESS;
    }

} // namespace CP
