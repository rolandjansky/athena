/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/// @author Miha Muskinja

//
// includes
//

#include <AsgAnalysisAlgorithms/EventSelectionByObjectFlagAlg.h>

//
// method implementations
//

namespace CP {

EventSelectionByObjectFlagAlg ::EventSelectionByObjectFlagAlg(
    const std::string &name, ISvcLocator *pSvcLocator)
    : AnaAlgorithm(name, pSvcLocator) {
}

StatusCode EventSelectionByObjectFlagAlg ::initialize() {
    m_systematicsList.addHandle(m_particleHandle);
    ANA_CHECK(m_systematicsList.initialize());
    ANA_CHECK(m_preselection.initialize());

    return StatusCode::SUCCESS;
}

StatusCode EventSelectionByObjectFlagAlg ::execute() {
    return m_systematicsList.foreach ([&] (const CP::SystematicSet &sys) -> StatusCode {
        m_total++;

        // particle container
        const xAOD::IParticleContainer *particles = nullptr;
        ANA_CHECK(m_particleHandle.retrieve(particles, sys));

        // reject events with any particle passing the input selection
        for (const xAOD::IParticle *particle : *particles) {
            if (m_preselection.getBool(*particle)) {
                ATH_MSG_VERBOSE("Event failed.");
                setFilterPassed(false);
                return StatusCode::SUCCESS;
            }
        }

        // event passed
        m_passed++;
        ATH_MSG_VERBOSE("Event passed all flags.");
        setFilterPassed(true);

        return StatusCode::SUCCESS;
    });
}

StatusCode EventSelectionByObjectFlagAlg ::finalize() {
    ATH_MSG_INFO("Events passing selection: " << m_passed << " / " << m_total);
    return StatusCode::SUCCESS;
}

} // namespace CP
