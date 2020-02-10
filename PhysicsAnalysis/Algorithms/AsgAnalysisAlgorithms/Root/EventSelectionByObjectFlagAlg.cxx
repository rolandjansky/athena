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
    : AnaAlgorithm(name, pSvcLocator) {}

StatusCode EventSelectionByObjectFlagAlg ::initialize() {

    if (m_eventDecisionOutputDecoration.empty()) {
        ANA_MSG_ERROR("no scale factor decoration name set");
        return StatusCode::FAILURE;
    }

    m_systematicsList.addHandle(m_particleHandle);
    ANA_CHECK(m_systematicsList.initialize());
    ANA_CHECK(m_preselection.initialize());
    ANA_CHECK(m_veto.initialize());

    return StatusCode::SUCCESS;
}

StatusCode EventSelectionByObjectFlagAlg ::execute() {

    ANA_CHECK(m_eventDecisionOutputDecoration.preExecute(m_systematicsList));

    // if all systematics fail we can discard the event
    bool pass_event = false;
    m_total++;

    // loop over systematics
    ATH_CHECK(m_systematicsList.foreach ([&](const CP::SystematicSet &sys) -> StatusCode {

        // decision for current systematic
        bool pass_current = true;

        // particle container
        const xAOD::IParticleContainer *particles = nullptr;
        ANA_CHECK(m_particleHandle.retrieve(particles, sys));

        // event info object
        xAOD::EventInfo *eventInfo = nullptr;
        ANA_CHECK(m_eventInfoHandle.getCopy(eventInfo, sys));

        // reject events with any particle passing the input selection
        for (const xAOD::IParticle *particle : *particles) {
            if (m_preselection.getBool(*particle)) {
                if (m_veto.getBool(*particle)) {
                    ATH_MSG_VERBOSE("Event failed.");
                    pass_current = false;
                    break;
                }
            }
        }
        // decorate event info
        m_eventDecisionOutputDecoration.set(*eventInfo, pass_current, sys);

        // reject event only if fail for all systematics
        if (pass_current)
            pass_event = true;

        return StatusCode::SUCCESS;
    }));

    // discard events that failed for all systematics
    if (pass_event) {
        m_passed++;
        setFilterPassed(true);
    } else {
        setFilterPassed(false);
    }

    return StatusCode::SUCCESS;
}

StatusCode EventSelectionByObjectFlagAlg ::finalize() {
    ATH_MSG_INFO("Events passing selection: " << m_passed << " / " << m_total);
    return StatusCode::SUCCESS;
}

} // namespace CP
