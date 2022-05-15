/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/// @author Miha Muskinja

//
// includes
//

// EDM include(s):
#include "xAODTruth/TruthParticleContainer.h"

// Local include(s):
#include <AsgAnalysisAlgorithms/SysTruthWeightAlg.h>

//
// method implementations
//

namespace CP
{
SysTruthWeightAlg::SysTruthWeightAlg(const std::string &name, ISvcLocator *pSvcLocator)
    : AnaAlgorithm(name, pSvcLocator), m_sysTruthWeightTool("PMGTools::PMGHFProductionFractionTool", this)
{
    // The truth particle container
    declareProperty("TruthParticleContainer", m_truthParticleContainer = "TruthParticles");

    // The production fraction rw tool
    declareProperty("sysTruthWeightTool", m_sysTruthWeightTool);
}

StatusCode SysTruthWeightAlg::initialize()
{
    if (m_decoration.empty())
    {
        ANA_MSG_ERROR("no decoration name set");
        return StatusCode::FAILURE;
    }

    ANA_CHECK(m_sysTruthWeightTool.retrieve());
    ANA_CHECK(m_eventInfoHandle.initialize(m_systematicsList, SG::AllowEmpty));
    ANA_CHECK(m_decoration.initialize(m_systematicsList, m_eventInfoHandle, SG::AllowEmpty));
    ANA_CHECK(m_systematicsList.addSystematics(*m_sysTruthWeightTool));
    ANA_CHECK(m_systematicsList.initialize());

    return StatusCode::SUCCESS;
}

StatusCode SysTruthWeightAlg::execute()
{

    // Retreive the truth particles container
    const xAOD::TruthParticleContainer *truthParticles = nullptr;
    ANA_CHECK(evtStore()->retrieve(truthParticles, m_truthParticleContainer));

    for (const auto &sys : m_systematicsList.systematicsVector())
    {
        ANA_CHECK(m_sysTruthWeightTool->applySystematicVariation(sys));
        const xAOD::EventInfo *eventInfo = nullptr;
        ANA_CHECK(m_eventInfoHandle.retrieve(eventInfo, sys));

        m_decoration.set(*eventInfo, m_sysTruthWeightTool->getWeight(truthParticles), sys);
    }

    // return
    return StatusCode::SUCCESS;
}
} // namespace CP
