/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// -----------------------------------------------------------------------------------------------
//
// xAODBSignalFilter.cxx
//
// Description:  finds all B-chains, applies cuts specified in jobOptionsFile
//
// Author: Malte Muller  <maltemlr@yahoo.co.uk>
// August 2002      Malte Muller
// Sept.  2002      M.Smizanska: minor updates in fill Ntuple
// Dec.   2003      M.Smizanska: filtering out events with B-hadrons
//                               left undecayed either in Pythia or in EvtGen.
//                               This is used as a method of selecting only wanted
//                               exclusive channels into persistent output. See PythiaB manual.
// Feb.   2004	    J. Catmore   LVL1/LVL2 cut facility added.
//
// Sept.  2013      C. Alpigiani - Removed the code that writes the ntuples
//                               - Wrote the code that implements the particle selection (pT, eta
//                                 and invariant mass)
// -----------------------------------------------------------------------------------------------

#include "GeneratorFilters/xAODBSignalFilter.h"
#include "TruthUtils/PIDHelpers.h"
#include "CLHEP/Vector/LorentzVector.h"

#include <sstream>

xAODBSignalFilter::xAODBSignalFilter(const std::string &name, ISvcLocator *pSvcLocator) : GenFilter(name, pSvcLocator)
{
    // ** Declare the algorithm's properties **
    declareProperty("LVL1MuonCutOn", m_localLVL1MuonCutOn = false);
    declareProperty("LVL2MuonCutOn", m_localLVL2MuonCutOn = false);
    declareProperty("LVL2ElectronCutOn", m_localLVL2ElectronCutOn = false);
    declareProperty("LVL1MuonCutPT", m_localLVL1MuonCutPT = 0.0);
    declareProperty("LVL1MuonCutEta", m_localLVL1MuonCutEta = 102.5);
    declareProperty("LVL2MuonCutPT", m_localLVL2MuonCutPT = 0.0);
    declareProperty("LVL2MuonCutEta", m_localLVL2MuonCutEta = 102.5);
    declareProperty("LVL2ElectronCutPT", m_localLVL2ElectronCutPT = 0.0);
    declareProperty("LVL2ElectronCutEta", m_localLVL2ElectronCutEta = 102.5);
    declareProperty("Cuts_Final_e_switch", m_cuts_f_e_on = false);
    declareProperty("Cuts_Final_e_pT", m_cuts_f_e_pT = 0.);
    declareProperty("Cuts_Final_e_eta", m_cuts_f_e_eta = 2.5);
    declareProperty("Cuts_Final_mu_switch", m_cuts_f_mu_on = false);
    declareProperty("Cuts_Final_mu_pT", m_cuts_f_mu_pT = 0.);
    declareProperty("Cuts_Final_mu_eta", m_cuts_f_mu_eta = 102.5);
    declareProperty("Cuts_Final_hadrons_switch", m_cuts_f_had_on = false);
    declareProperty("Cuts_Final_hadrons_pT", m_cuts_f_had_pT = 0.);
    declareProperty("Cuts_Final_hadrons_eta", m_cuts_f_had_eta = 102.5);
    declareProperty("Cuts_Final_gamma_switch", m_cuts_f_gam_on = false);
    declareProperty("Cuts_Final_gamma_pT", m_cuts_f_gam_pT = 0.);
    declareProperty("Cuts_Final_gamma_eta", m_cuts_f_gam_eta = 102.5);
    declareProperty("Cuts_Final_K0_switch", m_cuts_f_K0_on = false);
    declareProperty("Cuts_Final_K0_pT", m_cuts_f_K0_pT = 0.);
    declareProperty("Cuts_Final_K0_eta", m_cuts_f_K0_eta = 102.5);
    //
    // ** Declare the signal B-meson/hadron PDGid
    declareProperty("B_PDGCode", m_B_pdgid = 0);
    //
    // ** Declare properties for mass filter **
    declareProperty("InvMass_switch", m_InvMass_switch = false);
    declareProperty("InvMass_PartId1", m_InvMass_PartId1 = 13);
    declareProperty("InvMass_PartId2", m_InvMass_PartId2 = -m_InvMass_PartId1);
    declareProperty("InvMass_PartFakeMass1", m_InvMass_PartFakeMass1 = -1.);
    declareProperty("InvMass_PartFakeMass2", m_InvMass_PartFakeMass2 = -1.);
    declareProperty("InvMassMin", m_InvMassMin = 0.0);
    declareProperty("InvMassMax", m_InvMassMax = 14000000.0);
    declareProperty("TotalInvMass_switch", m_TotalInvMass_switch = false);
    declareProperty("TotalInvMassMin", m_TotalInvMassMin = 0.0);
    declareProperty("TotalInvMassMax", m_TotalInvMassMax = 14000000.0);

    // ** Initialise event counter **
    m_EventCnt = 0;
    m_LVL1Counter = 0;
    m_LVL2Counter = 0;
    m_rejectedTrigger = 0;
    m_rejectedAll = 0;
}

StatusCode xAODBSignalFilter::filterEvent()
{
    ATH_MSG_INFO("");
    ATH_MSG_INFO(" ---------------------------------- ");
    ATH_MSG_INFO(" >>> xAODBSignalFilter::FilterEvent <<< ");
    ATH_MSG_INFO(" ---------------------------------- ");
    ATH_MSG_INFO("");

    // ** Return ERROR and exit if the user has not selected the PDGid of the B-meson/hadron signal **
    if (m_cuts_f_e_on || m_cuts_f_mu_on || m_cuts_f_had_on || m_cuts_f_gam_on || m_cuts_f_K0_on)
        if (m_B_pdgid == 0)
        {
            ATH_MSG_ERROR("");
            ATH_MSG_ERROR(" ==>> 'B_PDGCode = 0'! Please define the PDGid of the B-meson/hadron signal!");
            ATH_MSG_ERROR("");
            return StatusCode::FAILURE;
        }
    
    // Retrieve full TruthEventContainer container
    const xAOD::TruthEventContainer *xTruthEventContainer=NULL;
    if (evtStore()->retrieve(xTruthEventContainer, "TruthEvents").isFailure())
    {
        ATH_MSG_ERROR("No TruthEvent collection with name " << "TruthEvents" << " found in StoreGate!");
        return StatusCode::FAILURE;
    }
    // ** Begin iterating over McEventCollection **
    for ( xAOD::TruthEventContainer::const_iterator itr = xTruthEventContainer->begin(); itr != xTruthEventContainer->end(); ++itr)
    {
        unsigned int nPart = (*itr)->nTruthParticles();
        m_EventCnt++;

        bool acceptEvent = true;

        // ** Check HepMC for particles activating LVL1 trigger, if that is what user wishes **
        //
        bool LVL1Passed = false;
        int LVL1MuonBarcode = 0;
        //
        if (m_localLVL1MuonCutOn)
        {
            //
            for (unsigned int iPart = 0; iPart < nPart; ++iPart) 
            {
                const xAOD::TruthParticle* part =  (*itr)->truthParticle(iPart);
                bool LVL1Result = LVL1_Mu_Trigger(part);
                if (LVL1Result)
                {
                    LVL1Passed = true;
                    LVL1MuonBarcode = part->barcode();//HepMC::barcode(part); // Remember the muon for LVL2 testing
                    break;
                }
            }
        }

        // ** Check HepMC for particles activating LVL2 trigger, if that is what user wishes **
        //
        bool LVL2Passed = false;
        //
        if (LVL1Passed && (m_localLVL2MuonCutOn || m_localLVL2ElectronCutOn))
        {
            for (unsigned int iPart = 0; iPart < nPart; ++iPart) 
            {
                const xAOD::TruthParticle* part =  (*itr)->truthParticle(iPart);
                bool LVL2Result = LVL2_eMu_Trigger(part);
                if (LVL2Result)
                {
                    if (part->barcode() != LVL1MuonBarcode) // Check the particle triggering LVL2 isn't
                        LVL2Passed = true;                       // the muon that triggered LVL1 --> Artificial,
                                                                 // since, effectively, LVL2 trigger is not applied.
                                                                 // This is needed to "trigger" the 2nd muon!
                }
            }
        }

        // ** Flag event as passing LVL1 if it has passed **
        //
        if (m_localLVL1MuonCutOn && LVL1Passed)
        {
            ATH_MSG_DEBUG(" LVL1 Trigger activated for event " << m_EventCnt);
            m_LVL1Counter++;
        }
        // ** Flag event as passing LVL2 if it has passed **
        //
        if ((m_localLVL2MuonCutOn || m_localLVL2ElectronCutOn) && LVL2Passed)
        {
            ATH_MSG_DEBUG(" LVL2 Trigger activated for event " << m_EventCnt);
            m_LVL2Counter++;
        }

        // ** If user hasn't requested triggers then set everything to true so nothing is thrown away **
        //
        if (!m_localLVL1MuonCutOn)
        {
            LVL1Passed = true;
            LVL2Passed = true;
        }
        if (!m_localLVL2MuonCutOn && !m_localLVL2ElectronCutOn)
            LVL2Passed = true;

        // ** Reject event if an undecayed quark is found **
        //
        for (unsigned int iPart = 0; iPart < nPart; ++iPart) 
        {
            const xAOD::TruthParticle* part =  (*itr)->truthParticle(iPart);
            if (std::abs(part->pdgId()) <= 6 && part->status() == 1)
            {
                acceptEvent = false;
                const int pID = part->pdgId();
                ATH_MSG_WARNING(" Undecayed quark " << pID << " found"
                                                    << " , status = " << part->status());
            }
        }

        // ** Look for signal events **
        //
        bool SignalPassedCuts = false;
        if (LVL1Passed && LVL2Passed)
        {
            // ** Loop on all particles **
            for (unsigned int iPart = 0; iPart < nPart; ++iPart) 
            {
                const xAOD::TruthParticle* part =  (*itr)->truthParticle(iPart);
                const int particleID = part->pdgId();
                //
                bool motherIsB = false;
                bool newBChain = false;

                if ((MC::PID::isBottomMeson(particleID) || MC::PID::isBottomBaryon(particleID)) && part->status() != 3) // p->status()!=3 excludes the partons
                                                                                                                        // including immediate decays of resonances.
                {
                    // ** Reject whole event if any of B-hadrons in the event is not decayed **
                    if (part->status() == 1 || part->status() == 899)
                    {
                        acceptEvent = false;
                    }

                    for(size_t thisParent_id=0; thisParent_id < part->prodVtx()->nIncomingParticles(); thisParent_id++)
                    {
                        auto parent = part->prodVtx()->incomingParticle(thisParent_id);
                        int parentID = parent->pdgId();
                        if (MC::PID::isBottomMeson(parentID) || MC::PID::isBottomBaryon(parentID))
                            motherIsB = true;
                    }
                    if (motherIsB)
                    {
                        newBChain = false; // Since the chain is not new
                    }
                    else
                    {
                        newBChain = true;
                    }
                }
                else
                {
                    newBChain = false;
                } // Since particle is not b

                // ** New B-signal found, output message and find whole decay tree **
                if (newBChain)
                {
                    const HepPDT::ParticleData *HadronData = particleData(particleID);
                    std::string HadronName = "unknown particle";
                    if (HadronData)
                    {
                        HadronName = HadronData->name();
                        if (particleID < 0)
                            HadronName = "anti - " + HadronName;
                    }
                    ATH_MSG_DEBUG("");
                    ATH_MSG_DEBUG(" ------------------------------------------ ");
                    ATH_MSG_DEBUG(" *** xAODBSignalFilter.cxx: B-signal found ***  ");
                    ATH_MSG_DEBUG(" ------------------------------------------ ");
                    ATH_MSG_DEBUG("");
                    // cppcheck-suppress shiftNegative; false positive
                    ATH_MSG_DEBUG(" Event " << m_EventCnt << " --> B-hadron/B-meson id " << particleID << " (" << HadronName << ") , status " << part->status());
                    ATH_MSG_DEBUG("");

                    // ** Looping on all children checking if they have passed the selection cuts defined by the user **
                    if (m_cuts_f_e_on || m_cuts_f_mu_on || m_cuts_f_had_on || m_cuts_f_gam_on || m_cuts_f_K0_on || m_InvMass_switch || m_TotalInvMass_switch)
                    {
                        ATH_MSG_DEBUG(" *** KINEMATIC CUTS ON PARTICLES ACTIVATED *** ");
                        ATH_MSG_DEBUG("");
                        ATH_MSG_DEBUG(" --------------------------------- ");
                        ATH_MSG_DEBUG("  Started looping on all children ");
                        ATH_MSG_DEBUG(" --------------------------------- ");
                        ATH_MSG_DEBUG("");
                        //
                        bool isSignal = false;
                        bool havePassedCuts = true;
                        TLorentzVector CandPart1, CandPart2, total_4mom;
                        //
                        FindAllChildren(part, "", false, isSignal, havePassedCuts, CandPart1, CandPart2, false, total_4mom);
                        //
                        ATH_MSG_DEBUG("");
                        ATH_MSG_DEBUG(" ------------------------------- ");
                        ATH_MSG_DEBUG("  Ended looping on all children  ");
                        ATH_MSG_DEBUG(" ------------------------------- ");
                        ATH_MSG_DEBUG("");

                        SignalPassedCuts = (isSignal && havePassedCuts);

                        // ** If signal event is found and InvMass_switch = true, check if the selected
                        //    couple of particles has an invariant mass within the range set by the user **
                        if (m_InvMass_switch)
                        {
                            if (SignalPassedCuts)
                            {
                                bool accept_mass = false;
                                bool accept_total_mass = false;
                                ATH_MSG_DEBUG("");
                                ATH_MSG_DEBUG(" *** INVARIANT MASS CUTS ON PARTICLES ACTIVATED! *** ");
                                ATH_MSG_DEBUG("");
                                if (m_InvMass_switch)
                                    ATH_MSG_DEBUG("     -- Mass cuts -->>  " << m_InvMassMin << " < mass < " << m_InvMassMax << " MeV");
                                //
                                double invMass = (CandPart1 + CandPart2).M();
                                double invMass_total = total_4mom.M();
                                //

                                if (m_InvMass_switch)
                                {
                                    ATH_MSG_DEBUG("     -- Invariant mass of the couple of particles = " << invMass << " MeV");
                                    if (m_InvMassMin < invMass && invMass < m_InvMassMax)
                                    {
                                        ATH_MSG_DEBUG("        ==>> Event has passed the mass filter!");
                                        ATH_MSG_DEBUG("");
                                        accept_mass = true;
                                    }
                                    else
                                    {
                                        ATH_MSG_DEBUG("        ==>> Event has NOT passed the mass filter!");
                                        ATH_MSG_DEBUG("");
                                    }
                                    SignalPassedCuts = SignalPassedCuts && accept_mass;
                                }
                                if (m_TotalInvMass_switch)
                                {
                                    ATH_MSG_DEBUG("     -- Total invariant mass of the final-state particles = " << invMass_total << " MeV");
                                    if (m_TotalInvMassMin < invMass_total && invMass_total < m_TotalInvMassMax)
                                    {
                                        ATH_MSG_DEBUG("        ==>> Event has passed the total mass filter!");
                                        ATH_MSG_DEBUG("");
                                        accept_total_mass = true;
                                    }
                                    else
                                    {
                                        ATH_MSG_DEBUG("        ==>> Event has NOT passed the total mass filter!");
                                        ATH_MSG_DEBUG("");
                                    }
                                    SignalPassedCuts = SignalPassedCuts && accept_total_mass;
                                }
                            }
                            else
                            {
                                ATH_MSG_DEBUG("");
                                ATH_MSG_DEBUG(" *** INVARIANT MASS CUTS ON PARTICLES NOT APPLIED (since the event is not a signal that passed cuts)! *** ");
                                ATH_MSG_DEBUG("");
                            }
                        }
                        else
                        {
                            ATH_MSG_DEBUG(" *** INVARIANT MASS CUTS ON PARTICLES NOT ACTIVATED *** ");
                            ATH_MSG_DEBUG("");
                        }
                    }
                    else
                    {
                        SignalPassedCuts = true;
                        //
                        ATH_MSG_DEBUG(" *** KINEMATIC CUTS ON PARTICLES NOT ACTIVATED ==>> ACCEPT ALL EVENTS! *** ");
                        ATH_MSG_DEBUG("");
                    }
                } // End newBChain
                if (SignalPassedCuts)
                    break; // We have found the signal, we do not need to look for other particles
            }              // End particle iteration

            if (SignalPassedCuts)
            {
                ATH_MSG_DEBUG(" -->> All signal children have passed cuts on particles!");
            }
            else
            {
                ATH_MSG_DEBUG(" -->> Signal children have NOT passed cuts on particles!");
            }
        } // end of LVL1/LVL2 selection
        else
        {
            ATH_MSG_DEBUG("");
            ATH_MSG_DEBUG(" -->> Event has NOT passed the trigger!");
        }
        // ** Reject event if it has not passed triggers **
        //    --> If not requested by user these flags will be set
        //        to true so there will be no erroneous rejection
        if ((!LVL1Passed) || (!LVL2Passed))
        {
            m_rejectedTrigger++;
            acceptEvent = false;
        }

        // ** Check if all conditions are true **
        //    --> Accept the event if acceptEvent is true and the event is a signal event
        acceptEvent = acceptEvent && SignalPassedCuts;

        // ** Print whether the event has passed or not **
        //
        ATH_MSG_DEBUG("");
        if (!acceptEvent)
        {
            setFilterPassed(false);
            m_rejectedAll++;
            ATH_MSG_DEBUG(" ==========================");
            ATH_MSG_DEBUG("  Event REJECTED by Filter ");
            ATH_MSG_DEBUG(" ==========================");
        }
        else
        {
            setFilterPassed(true);
            ATH_MSG_DEBUG(" ==========================");
            ATH_MSG_DEBUG("  Event ACCEPTED by Filter ");
            ATH_MSG_DEBUG(" ==========================");
        }
        ATH_MSG_DEBUG("");

    } // End event iteration
    // End of execution for each event
    return StatusCode::SUCCESS;
}

bool xAODBSignalFilter::test_cuts(const double myPT, const double testPT, const double myEta, const double testEta) const
{
    return ((myPT >= testPT) && (std::abs(myEta) <= testEta));
}

bool xAODBSignalFilter::LVL1_Mu_Trigger(const xAOD::TruthParticle* child) const
{
    bool accept = false;
    int pID = child->pdgId();
    double myPT = child->pt();
    double myEta = child->eta();

    if ((std::abs(pID) == 13) && m_localLVL1MuonCutOn)
        accept = test_cuts(myPT, m_localLVL1MuonCutPT, myEta, m_localLVL1MuonCutEta);

    return accept;
}

bool xAODBSignalFilter::LVL2_eMu_Trigger(const xAOD::TruthParticle* child) const
{
    bool accept = false;
    int pID = child->pdgId();
    double myPT = child->pt();
    double myEta = child->eta();

    if ((std::abs(pID) == 11) && m_localLVL2ElectronCutOn)
        accept = test_cuts(myPT, m_localLVL2ElectronCutPT, myEta, m_localLVL2ElectronCutEta);
    if ((std::abs(pID) == 13) && m_localLVL2MuonCutOn)
        accept = test_cuts(myPT, m_localLVL2MuonCutPT, myEta, m_localLVL2MuonCutEta);

    return accept;
}

void xAODBSignalFilter::FindAllChildren(const xAOD::TruthParticle* mother, std::string treeIDStr,
                                        bool fromFinalB, bool &foundSignal, bool &passedAllCuts,
                                        TLorentzVector &p1, TLorentzVector &p2, bool fromSelectedB, TLorentzVector &total_4mom) const
{
    int pID = mother->pdgId();
    //
    if (!(mother->decayVtx()) && (mother->status() == 1)) // i.e. this is a final state
    {
        bool hasChildGoodParent = fromFinalB && (fromSelectedB || m_B_pdgid == 0);
        //
        if (fromFinalB && hasChildGoodParent)
        {
            foundSignal = true;
            bool passedCut = FinalStatePassedCuts(mother); // X = X && ... in case of multiple particles (e.g. KK)
            if (m_cuts_f_e_on && std::abs(pID) == 11)
                passedAllCuts = passedAllCuts && passedCut;
            if (m_cuts_f_mu_on && std::abs(pID) == 13)
                passedAllCuts = passedAllCuts && passedCut;
            if (m_cuts_f_had_on && MC::PID::isHadron(pID) && MC::PID::isCharged(pID))
                passedAllCuts = passedAllCuts && passedCut;
            if (m_cuts_f_gam_on && std::abs(pID) == 22)
                passedAllCuts = passedAllCuts && passedCut;
            if (m_cuts_f_K0_on && std::abs(pID) == 311)
                passedAllCuts = passedAllCuts && passedCut;
            //
            if ((m_InvMass_switch || m_TotalInvMass_switch) && m_InvMass_PartId1 == pID)
            {
                if (m_InvMass_PartFakeMass1 < 0.)
                    p1.SetPxPyPzE(mother->px(), mother->py(), mother->pz(), mother->e());
                else
                    p1.SetXYZM(mother->px(), mother->py(), mother->pz(), m_InvMass_PartFakeMass1);
                total_4mom = total_4mom + p1;
            }
            else if ((m_InvMass_switch || m_TotalInvMass_switch) && m_InvMass_PartId2 == pID)
            {
                if (m_InvMass_PartFakeMass2 < 0.)
                    p2.SetPxPyPzE(mother->px(), mother->py(), mother->pz(), mother->e());
                else
                    p2.SetXYZM(mother->px(), mother->py(), mother->pz(), m_InvMass_PartFakeMass2);
                total_4mom = total_4mom + p2;
            }
            else if (m_TotalInvMass_switch)
            {
                TLorentzVector current_4p;
                current_4p.SetPxPyPzE(mother->px(), mother->py(), mother->pz(), mother->e());
                total_4mom = total_4mom + current_4p;
            }
        }
        return;
    }
    else
    {
        if (!(mother->decayVtx())) // i.e. something is wrong in HepMC
        {
            if (mother->status() == 2)
                ATH_MSG_DEBUG(" Inconsistent entry in HepMC (status 2 particle not decayed), chain rejected!");
            return;
        }
    }

    int childCnt = 0;
    std::string childIDStr;
    if (!(treeIDStr == ""))
        treeIDStr = treeIDStr + ".";

    // ** Find out whether particle is child of final (non-excited) B, used for cuts **
    if ((!fromFinalB) && (MC::PID::isBottomMeson(pID) || MC::PID::isBottomBaryon(pID)))
    {
        fromFinalB = true;
        int pID;
        for(size_t thisChild_id=0; thisChild_id < mother->decayVtx()->nOutgoingParticles(); thisChild_id++)
        {
            auto child = mother->decayVtx()->outgoingParticle(thisChild_id);
            pID = child->pdgId();
            if (MC::PID::isBottomMeson(pID) || MC::PID::isBottomBaryon(pID))
                fromFinalB = false;
        }
    }

    // ** Main loop: iterate over all children, call method recursively.
    for(size_t thisChild_id=0; thisChild_id < mother->decayVtx()->nOutgoingParticles(); thisChild_id++)
    {
        auto child = mother->decayVtx()->outgoingParticle(thisChild_id);
        childCnt++;
        std::stringstream childCntSS;
        childCntSS << childCnt;
        childIDStr = treeIDStr + childCntSS.str();
        PrintChild(child, childIDStr, fromFinalB);
        FindAllChildren(child, childIDStr, fromFinalB, foundSignal, passedAllCuts, p1, p2, (pID == m_B_pdgid) || fromSelectedB, total_4mom);
    }

    return;
}

bool xAODBSignalFilter::FinalStatePassedCuts(const xAOD::TruthParticle* child) const
{
    bool accept = true;

    // ** Look if children has passed the cuts **
    //
    int pID = child->pdgId();
    double myPT = child->pt();
    double myEta = child->eta();

    ATH_MSG_DEBUG("");
    ATH_MSG_DEBUG("    ** Found a child with: ");
    ATH_MSG_DEBUG("       - id  = " << pID);
    ATH_MSG_DEBUG("       - pT  = " << myPT << " MeV");
    ATH_MSG_DEBUG("       - eta = " << myEta);
    ATH_MSG_DEBUG("");

    if (m_cuts_f_e_on)
    {
        if (std::abs(pID) == 11)
        {
            ATH_MSG_DEBUG("       ** ( pT , eta ) cuts applied on the electron --> ( " << m_cuts_f_e_pT
                                                                                       << " , " << m_cuts_f_e_eta << " )");
            if (test_cuts(myPT, m_cuts_f_e_pT, myEta, m_cuts_f_e_eta))
            {
                ATH_MSG_DEBUG("           ==>> Found an electron which passed the pT and eta cuts!");
                ATH_MSG_DEBUG("                - Electron: pT  = " << myPT << " MeV");
                ATH_MSG_DEBUG("                - Electron: eta = " << myEta);
                ATH_MSG_DEBUG("");
                ATH_MSG_DEBUG("    ==>> Accepted the electron!");
                ATH_MSG_DEBUG("");
            }
            else
            {
                accept = false;
                ATH_MSG_DEBUG("           ==>> The electron has NOT passed the pT and eta cuts!");
                ATH_MSG_DEBUG("");
            }
        }
    }
    if (m_cuts_f_mu_on)
    {
        if (std::abs(pID) == 13)
        {
            ATH_MSG_DEBUG("       ** ( pT , eta ) cuts applied on the muon --> ( " << m_cuts_f_mu_pT
                                                                                   << " , " << m_cuts_f_mu_eta << " )");
            if (test_cuts(myPT, m_cuts_f_mu_pT, myEta, m_cuts_f_mu_eta))
            {
                ATH_MSG_DEBUG("           ==>> Found a muon which passed the pT and eta cuts!");
                ATH_MSG_DEBUG("                - Muon: pT  = " << myPT << " MeV");
                ATH_MSG_DEBUG("                - Muon: eta = " << myEta);
                ATH_MSG_DEBUG("");
                ATH_MSG_DEBUG("    ==>> Accepted the muon!");
                ATH_MSG_DEBUG("");
            }
            else
            {
                accept = false;
                ATH_MSG_DEBUG("           ==>> The muon has NOT passed the pT and eta cuts!");
                ATH_MSG_DEBUG("");
            }
        }
    }
    if (m_cuts_f_had_on)
    {
        if (MC::PID::isHadron(pID) && MC::PID::isCharged(pID))
        {
            ATH_MSG_DEBUG("       ** ( pT , eta ) cuts applied on the charged hadron --> ( " << m_cuts_f_had_pT
                                                                                             << " , " << m_cuts_f_had_eta << " )");
            if (test_cuts(myPT, m_cuts_f_had_pT, myEta, m_cuts_f_had_eta))
            {
                ATH_MSG_DEBUG("           ==>> Found a charged hadron which passed the pT and eta cuts!");
                ATH_MSG_DEBUG("                - Charged hadron: pT  = " << myPT << " MeV");
                ATH_MSG_DEBUG("                - Charged hadron: eta = " << myEta);
                ATH_MSG_DEBUG("");
                ATH_MSG_DEBUG("    ==>> Accepted the charged hadron!");
                ATH_MSG_DEBUG("");
            }
            else
            {
                accept = false;
                ATH_MSG_DEBUG("           ==>> The charged hadron has NOT passed the pT and eta cuts!");
                ATH_MSG_DEBUG("");
            }
        }
    }
    if (m_cuts_f_gam_on)
    {
        if (std::abs(pID) == 22)
        {
            ATH_MSG_DEBUG("       ** ( pT , eta ) cuts applied on the gamma --> ( " << m_cuts_f_gam_pT
                                                                                    << " , " << m_cuts_f_gam_eta << " )");
            if (test_cuts(myPT, m_cuts_f_gam_pT, myEta, m_cuts_f_gam_eta))
            {
                ATH_MSG_DEBUG("           ==>> Found a gamma which passed the pT and eta cuts!");
                ATH_MSG_DEBUG("                - Gamma: pT  = " << myPT << " MeV");
                ATH_MSG_DEBUG("                - Gamma: eta = " << myEta);
                ATH_MSG_DEBUG("");
                ATH_MSG_DEBUG("    ==>> Accepted the gamma!");
                ATH_MSG_DEBUG("");
            }
            else
            {
                accept = false;
                ATH_MSG_DEBUG("           ==>> The gamma has NOT passed the pT and eta cuts!");
                ATH_MSG_DEBUG("");
            }
        }
    }
    if (m_cuts_f_K0_on)
    {
        if (std::abs(pID) == 311)
        {
            ATH_MSG_DEBUG("       ** ( pT , eta ) cuts applied on the K0 --> ( " << m_cuts_f_K0_pT
                                                                                 << " , " << m_cuts_f_K0_eta << " )");
            if (test_cuts(myPT, m_cuts_f_K0_pT, myEta, m_cuts_f_K0_eta))
            {
                ATH_MSG_DEBUG("           ==>> Found a K0 which passed the pT and eta cuts!");
                ATH_MSG_DEBUG("                - K0: pT  = " << myPT << " MeV");
                ATH_MSG_DEBUG("                - K0: eta = " << myEta);
                ATH_MSG_DEBUG("");
                ATH_MSG_DEBUG("    ==>> Accepted the K0!");
                ATH_MSG_DEBUG("");
            }
            else
            {
                accept = false;
                ATH_MSG_DEBUG("           ==>> The K0 has NOT passed the pT and eta cuts!");
                ATH_MSG_DEBUG("");
            }
        }
    }

    return accept;
}

void xAODBSignalFilter::PrintChild(const xAOD::TruthParticle* child,
                                   const std::string &treeIDStr, const bool fromFinalB) const
{
    int pID = child->pdgId();
    // ** Find name **
    const HepPDT::ParticleData *pData = particleData(std::abs(pID));
    std::string pName = "unknown particle";
    if (pData)
    {
        pName = pData->name();
        if (pID < 0)
            pName = "anti - " + pName;
    }
    ATH_MSG_DEBUG("    " << treeIDStr << "   "
                         << "Child id = " << pID << " (" << pName
                         << ") ,  status = " << (child->status()) << " , from final B = " << fromFinalB);

    return;
}

StatusCode xAODBSignalFilter::filterFinalize()
{
    double total = m_EventCnt;
    ATH_MSG_ALWAYS(" I===============================================================================================");
    ATH_MSG_ALWAYS(" I                                    xAODBSignalFilter Summary Report                               ");
    ATH_MSG_ALWAYS(" I===============================================================================================");
    if (m_localLVL1MuonCutOn)
    {
        ATH_MSG_ALWAYS(" I  LVL1 muon trigger report:");
        ATH_MSG_ALWAYS(" I        Muon pT cut " << '\t' << '\t' << '\t' << '\t' << '\t' << '\t' << '\t' << '\t' << "I" << '\t'
                                                << m_localLVL1MuonCutPT << " MeV ");
        ATH_MSG_ALWAYS(" I        Muon pseudo-rapidity cut" << '\t' << '\t' << '\t' << '\t' << '\t' << '\t' << '\t' << "I" << '\t'
                                                            << m_localLVL1MuonCutEta);
        ATH_MSG_ALWAYS(" I        No of events containing at least " << '\t' << '\t' << '\t' << '\t' << '\t' << "I");
        ATH_MSG_ALWAYS(" I        one particle satisfying these cuts " << '\t' << '\t' << '\t' << '\t' << '\t' << "I" << '\t'
                                                                       << m_LVL1Counter);
        if (m_localLVL2MuonCutOn)
        {
            ATH_MSG_ALWAYS(" I  LVL2 muon trigger report:");
            ATH_MSG_ALWAYS(" I        Muon pT cut " << '\t' << '\t' << '\t' << '\t' << '\t' << '\t' << '\t' << '\t' << "I" << '\t'
                                                    << m_localLVL2MuonCutPT << " MeV ");
            ATH_MSG_ALWAYS(" I        Muon pseudo-rapidity cut " << '\t' << '\t' << '\t' << '\t' << '\t' << '\t' << "I" << '\t'
                                                                 << m_localLVL2MuonCutEta);
        }
        if (m_localLVL2ElectronCutOn)
        {
            ATH_MSG_ALWAYS(" I  LVL2 electron trigger report:");
            ATH_MSG_ALWAYS(" I        Electron pT cut " << '\t' << '\t' << '\t' << '\t' << '\t' << '\t' << '\t' << " I" << '\t' << m_localLVL2ElectronCutPT << " MeV ");
            ATH_MSG_ALWAYS(" I        Electron pseudo-rapidity cut " << '\t' << '\t' << '\t' << '\t' << '\t' << "I"
                                                                     << '\t' << m_localLVL2ElectronCutEta);
        }
        if (m_localLVL2MuonCutOn || m_localLVL2ElectronCutOn)
        {
            ATH_MSG_ALWAYS(" I        No of events containing at least one muon satisfying LVL1 cut" << '\t' << "I");
            ATH_MSG_ALWAYS(" I        and at least one separate particle passing these LVL2 cuts " << '\t' << '\t' << "I" << '\t'
                                                                                                   << m_LVL2Counter);
        }
    }
    ATH_MSG_ALWAYS(" I  Total no of input events " << '\t' << '\t' << '\t' << '\t' << '\t' << '\t' << '\t' << "I" << '\t'
                                                   << total);
    ATH_MSG_ALWAYS(" I  No of events rejected by trigger " << '\t' << '\t' << '\t' << '\t' << '\t' << '\t' << '\t' << "I" << '\t'
                                                           << m_rejectedTrigger);
    ATH_MSG_ALWAYS(" I  No of events rejected in total " << '\t' << '\t' << '\t' << '\t' << '\t' << '\t' << '\t' << "I" << '\t'
                                                         << m_rejectedAll);
    if (m_localLVL1MuonCutOn && (!m_localLVL2MuonCutOn && !m_localLVL2ElectronCutOn))
        ATH_MSG_ALWAYS(" I  To obtain correct cross section, multiply BX in PythiaB report by " << '\t' << '\t' << "I" << '\t'
                                                                                                << m_LVL1Counter / total);
    if (m_localLVL1MuonCutOn && (m_localLVL2MuonCutOn || m_localLVL2ElectronCutOn))
        ATH_MSG_ALWAYS(" I  To obtain correct cross section, multiply BX in PythiaB report by " << '\t' << '\t' << "I" << '\t'
                                                                                                << m_LVL2Counter / total);
    if (!m_localLVL1MuonCutOn)
        ATH_MSG_ALWAYS(" I  No trigger requests made");
    //
    ATH_MSG_ALWAYS(" I=========================================== End of report =====================================");

    return StatusCode::SUCCESS;
}
