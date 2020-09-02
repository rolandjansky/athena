/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/// @author Miha Muskinja

//
// includes
//

#include <AsgAnalysisAlgorithms/EventSelectionByObjectFlagAlg.h>
#include <SystematicsHandles/SysFilterReporter.h>
#include <SystematicsHandles/SysFilterReporterCombiner.h>

//
// method implementations
//

namespace CP {

EventSelectionByObjectFlagAlg ::EventSelectionByObjectFlagAlg(
    const std::string &name, ISvcLocator *pSvcLocator)
    : AnaAlgorithm(name, pSvcLocator) {}

StatusCode EventSelectionByObjectFlagAlg ::initialize() {

    m_systematicsList.addHandle(m_particleHandle);
    ANA_CHECK(m_systematicsList.initialize());
    ANA_CHECK(m_preselection.initialize());
    ANA_CHECK(m_veto.initialize());
    ANA_CHECK(m_filterParams.initialize());

    return StatusCode::SUCCESS;
}

StatusCode EventSelectionByObjectFlagAlg ::execute() {

    SysFilterReporterCombiner filterCombiner (m_filterParams, m_systematicsList, true);

    // loop over systematics
    return m_systematicsList.foreach ([&](const CP::SystematicSet &sys) -> StatusCode {
        SysFilterReporter filter (filterCombiner, sys);

        // particle container
        const xAOD::IParticleContainer *particles = nullptr;
        ANA_CHECK(m_particleHandle.retrieve(particles, sys));

        // reject events with any particle passing the input selection
        for (const xAOD::IParticle *particle : *particles) {
            if (m_preselection.getBool(*particle)) {
                if (m_veto.getBool(*particle)) {
                    ATH_MSG_VERBOSE("Event failed.");
                    filter.setPassed (false);
                    break;
                }
            }
        }

        return StatusCode::SUCCESS;
    });
}

StatusCode EventSelectionByObjectFlagAlg ::finalize() {
    ANA_CHECK (m_filterParams.finalize());
    return StatusCode::SUCCESS;
}

} // namespace CP
