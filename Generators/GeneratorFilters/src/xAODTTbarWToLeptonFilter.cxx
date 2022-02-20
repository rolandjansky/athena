/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "GeneratorFilters/xAODTTbarWToLeptonFilter.h"

xAODTTbarWToLeptonFilter::xAODTTbarWToLeptonFilter(const std::string &name, ISvcLocator *pSvcLocator)
    : GenFilter(name, pSvcLocator)
{
    declareProperty("Ptcut", m_Ptmin = 200000.);
    declareProperty("NumLeptons", m_numLeptons = -1);            // Negative for >0, positive integers for the specific number
    declareProperty("fourTopsFilter", m_fourTopsFilter = false); // four top filter or not
    declareProperty("SSMLFilter", m_SSMLFilter = false);         // Same sign multilepton filter or not
}

StatusCode xAODTTbarWToLeptonFilter::filterEvent()
{
    // Retrieve full TruthEventContainer container
    const xAOD::TruthEventContainer *xTruthEventContainer = NULL;
    if (evtStore()->retrieve(xTruthEventContainer, "TruthEvents").isFailure())
    {
        ATH_MSG_ERROR("No TruthEvent collection with name "
                      << "TruthEvents"
                      << " found in StoreGate!");
        return StatusCode::FAILURE;
    }
    int N_quark_t = 0;
    int N_quark_tbar = 0;
    int N_quark_t_all = 0;
    int N_quark_tbar_all = 0;
    int N_pt_above_cut = 0;
    int N_pt_above_cut_plus = 0;
    int N_pt_above_cut_minus = 0;

    int foundlepton[6] = {0, 0, 0, 0, 0, 0}; // e+, e-, mu+, mu-, tau+, tau-
    int count_found_leptons = 1;             // In ttbar each charged lepton flavour is counted at most once
    if (m_fourTopsFilter)
        count_found_leptons = 2; // In four tops, one can have the same charged lepton flavour twice

    for (xAOD::TruthEventContainer::const_iterator itr = xTruthEventContainer->begin(); itr != xTruthEventContainer->end(); ++itr)
    {
        unsigned int nPart = (*itr)->nTruthParticles();

        for (unsigned int iPart = 0; iPart < nPart; ++iPart)
        {
            const xAOD::TruthParticle *pitr = (*itr)->truthParticle(iPart);
            if (std::abs(pitr->pdgId()) != 6)
                continue;
            if (pitr->pdgId() == 6)
                N_quark_t_all++;
            if (pitr->pdgId() == -6)
                N_quark_tbar_all++;
            auto decayVtx = pitr->decayVtx();
            // Verify if we got a valid pointer and retrieve the number of daughters
            if (!decayVtx)
                continue;
            // For this analysis we are not interested in t->t MC structures, only in decays
            if (decayVtx->nOutgoingParticles() < 2)
                continue;

            for (size_t thisChild_id = 0; thisChild_id < pitr->decayVtx()->nOutgoingParticles(); thisChild_id++)
            {
                auto child_mcpart = pitr->decayVtx()->outgoingParticle(thisChild_id);
                //  Implicitly assume that tops always decay to W X
                if (std::abs(child_mcpart->pdgId()) != 24)
                    continue;
                if (pitr->pdgId() == 6)
                    N_quark_t++;
                if (pitr->pdgId() == -6)
                    N_quark_tbar++;

                bool useNextVertex = false;
                auto w_decayVtx = child_mcpart->decayVtx();
                while (w_decayVtx)
                {
                    useNextVertex = false;
                    for (size_t w_thisChild_id = 0; w_thisChild_id < w_decayVtx->nOutgoingParticles(); w_thisChild_id++)
                    {
                        auto grandchild_mcpart = w_decayVtx->outgoingParticle(w_thisChild_id);

                        int grandchild_pid = grandchild_mcpart->pdgId();
                        ATH_MSG_DEBUG("W (t/tbar) has " << w_decayVtx->nOutgoingParticles() << " children and the pdgId of the next is " << grandchild_pid);
                        // Check if the W's child is W again. If yes, then move to its next decay vertex in a decay tree
                        if (std::abs(grandchild_pid) == 24)
                        {
                            w_decayVtx = grandchild_mcpart->decayVtx();
                            // If something wrong comes from truth...
                            if (!w_decayVtx)
                            {
                                ATH_MSG_ERROR("A stable W is found... ");
                                break;
                            }
                            useNextVertex = true;
                            break;
                        }

                        // use brute force to use only leptons that have not been found already
                        if (grandchild_pid == -11 && foundlepton[0] < count_found_leptons)
                        {
                            if (grandchild_mcpart->pt() >= m_Ptmin)
                            {
                                foundlepton[0]++;
                                N_pt_above_cut++;
                                N_pt_above_cut_minus++;
                            }
                        }
                        if (grandchild_pid == 11 && foundlepton[1] < count_found_leptons)
                        {
                            if (grandchild_mcpart->pt() >= m_Ptmin)
                            {
                                foundlepton[1]++;
                                N_pt_above_cut++;
                                N_pt_above_cut_plus++;
                            }
                        }
                        if (grandchild_pid == -13 && foundlepton[2] < count_found_leptons)
                        {
                            if (grandchild_mcpart->pt() >= m_Ptmin)
                            {
                                foundlepton[2]++;
                                N_pt_above_cut++;
                                N_pt_above_cut_minus++;
                            }
                        }
                        if (grandchild_pid == 13 && foundlepton[3] < count_found_leptons)
                        {
                            if (grandchild_mcpart->pt() >= m_Ptmin)
                            {
                                foundlepton[3]++;
                                N_pt_above_cut++;
                                N_pt_above_cut_plus++;
                            }
                        }
                        if (grandchild_pid == -15 && foundlepton[4] < count_found_leptons)
                        {
                            if (grandchild_mcpart->pt() >= m_Ptmin)
                            {
                                foundlepton[4]++;
                                N_pt_above_cut++;
                                N_pt_above_cut_minus++;
                            }
                        }
                        if (grandchild_pid == 15 && foundlepton[5] < count_found_leptons)
                        {
                            if (grandchild_mcpart->pt() >= m_Ptmin)
                            {
                                foundlepton[5]++;
                                N_pt_above_cut++;
                                N_pt_above_cut_plus++;
                            }
                        }
                    }
                    // If investigation of W's next decay vertex is not required then finish looking for leptons
                    if (!useNextVertex)
                        break;
                }
            }
        }
    }

    ATH_MSG_INFO("Found " << N_quark_t_all << " t    quarks in event record");
    ATH_MSG_INFO("Found " << N_quark_tbar_all << " tbar quarks in event record");
    ATH_MSG_INFO("Found " << N_quark_t << " t    -> W X decays");
    ATH_MSG_INFO("Found " << N_quark_tbar << " tbar -> W X decays");
    ATH_MSG_INFO("Num leptons from W decays from tops with lepton pt above " << m_Ptmin / 1000.0 << " CLHEP::GeV/c = " << N_pt_above_cut);

    int count_tops = 1;
    if (m_fourTopsFilter)
        count_tops = 2;
    if (N_quark_t_all < count_tops || N_quark_tbar_all < count_tops)
    {

        ATH_MSG_ERROR("No t or tbar quarks were found in a (presumably) ttbar event! Event is rejected.");
        setFilterPassed(false);
        return StatusCode::SUCCESS;
    }

    if (N_quark_t < count_tops || N_quark_tbar < count_tops)
    {

        ATH_MSG_ERROR("No t or tbar quarks were found decaying to W in a (presumably) ttbar event! Event is rejected. Event dump follows.");
        int event = 0;
        for (xAOD::TruthEventContainer::const_iterator itr = xTruthEventContainer->begin(); itr != xTruthEventContainer->end(); ++itr)
        {
            unsigned int nPart = (*itr)->nTruthParticles();
            event++;
            int part = 0;
            for (unsigned int iPart = 0; iPart < nPart; ++iPart)
            {
                const xAOD::TruthParticle *mcpart = (*itr)->truthParticle(iPart);

                part++;
                int pid = mcpart->pdgId();
                ATH_MSG_ERROR("In event (from MC collection) " << event << " particle number " << part << " has pdgId = " << pid);
                // retrieve decay vertex
                auto decayVtx = mcpart->decayVtx();
                // verify if we got a valid pointer
                if (!decayVtx)
                    continue;
                int part_child = 0;
                for (size_t thisChild_id = 0; thisChild_id < mcpart->decayVtx()->nOutgoingParticles(); thisChild_id++)
                {
                    auto child_mcpart = mcpart->decayVtx()->outgoingParticle(thisChild_id);
                    part_child++;
                    int child_pid = child_mcpart->pdgId();
                    ATH_MSG_ERROR("          child " << part_child << " with pdgId = " << child_pid);
                }
            }
        }
        setFilterPassed(false);
        return StatusCode::SUCCESS;
    }

    // If NumLeptons is negative (default), accept if Nlep > 0, otherwise only accept an exact match
    if (m_numLeptons < 0)
    {
        if ((N_quark_t >= 2 || N_quark_tbar >= 2) && !m_fourTopsFilter)
        {
            ATH_MSG_WARNING("More than one t -> W X or tbar -> W X decays found. Event is accepted anyway.");
        }
        if ((N_quark_t > 2 || N_quark_tbar > 2) && m_fourTopsFilter)
        {
            ATH_MSG_WARNING("More than one t -> W X or tbar -> W X decays found. Event is accepted anyway.");
        }
        setFilterPassed(N_pt_above_cut > 0);
    }
    else
    {
        if (m_fourTopsFilter)
        {
            if (m_SSMLFilter)
                setFilterPassed((N_pt_above_cut >= m_numLeptons) && (N_pt_above_cut_plus >= 2 || N_pt_above_cut_minus >= 2));
            else
                setFilterPassed(N_pt_above_cut >= m_numLeptons);
        }
        else
            setFilterPassed(N_pt_above_cut == m_numLeptons);
    }

    return StatusCode::SUCCESS;
}
