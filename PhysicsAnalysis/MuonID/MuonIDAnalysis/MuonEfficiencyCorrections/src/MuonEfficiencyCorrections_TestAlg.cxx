/*
 Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

#include "xAODMuon/MuonContainer.h"
#include "xAODMuon/MuonAuxContainer.h"

// note: stolen from CPToolTests example!
// test algorithm using the MuonEfficiencyScaleFactors class

// Local include(s):
#include "MuonEfficiencyCorrections_TestAlg.h"
#include "PATInterfaces/SystematicsTool.h"
#include <cmath>
namespace CP {

    MuonEfficiencyCorrections_TestAlg::MuonEfficiencyCorrections_TestAlg(const std::string& name, ISvcLocator* svcLoc) :
                    AthAlgorithm(name, svcLoc),
                    m_sf_Tool("CP::MuonEfficiencyScaleFactors/MuonEfficiencyScaleFactors", this),
                    m_isosf_Tool("CP::MuonEfficiencyScaleFactors/MuonEfficiencyScaleFactors", this),
                    m_ttvasf_Tool("CP::MuonEfficiencyScaleFactors/MuonEfficiencyScaleFactors", this),
                    m_trigsf_Tool("CP::MuonEfficiencyScaleFactors/MuonTriggerScaleFactors", this),
                    m_prw_Tool("CP::PileupReweighting/PileupReweightingTool", this)
    {
        declareProperty("SGKey", m_sgKey = "Muons");
        // prepare the handle
        declareProperty("TrigScaleFactorTool", m_trigsf_Tool);
        declareProperty("ScaleFactorTool", m_sf_Tool);
        declareProperty("TTVAScaleFactorTool", m_ttvasf_Tool);
        declareProperty("IsolationScaleFactorTool", m_isosf_Tool);
        declareProperty("PileupReweightingTool", m_prw_Tool);
        // force strict checking of return codes
        CP::SystematicCode::enableFailure();
        CP::CorrectionCode::enableFailure();
    }

    StatusCode MuonEfficiencyCorrections_TestAlg::initialize() {

        ATH_MSG_INFO("Initialising - Package version: " << PACKAGE_VERSION);
        ATH_MSG_DEBUG("SGKey = " << m_sgKey);

        ATH_MSG_DEBUG("ScaleFactorTool  = " << m_sf_Tool);
        ATH_CHECK(m_sf_Tool.retrieve());

        ATH_MSG_DEBUG("IsolationScaleFactorTool  = " << m_isosf_Tool);
        ATH_CHECK(m_isosf_Tool.retrieve());

        ATH_MSG_DEBUG("TTVAScaleFactorTool  = " << m_ttvasf_Tool);
        ATH_CHECK(m_ttvasf_Tool.retrieve());

        ATH_MSG_DEBUG("TriggerScaleFactorTool  = " << m_trigsf_Tool);
        ATH_CHECK(m_trigsf_Tool.retrieve());

        ATH_MSG_DEBUG("PileupReweightingTool  = " << m_prw_Tool);
        ATH_CHECK(m_prw_Tool.retrieve());

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

        // Loop over them:
        xAOD::MuonContainer::const_iterator mu_itr = muons->begin();
        xAOD::MuonContainer::const_iterator mu_end = muons->end();

        CP::SystematicSet statup;
        statup.insert(CP::SystematicVariation("MUON_EFF_STAT", 1));
        CP::SystematicSet statupLowPt;
        statupLowPt.insert(CP::SystematicVariation("MUON_EFF_STAT_LOWPT", 1));
        CP::SystematicSet sysup;
        sysup.insert(CP::SystematicVariation("MUON_EFF_SYS", 1));
        CP::SystematicSet sysupLowPt;
        sysupLowPt.insert(CP::SystematicVariation("MUON_EFF_SYS_LOWPT", 1));
//     statup.insert (CP::SystematicVariation ("MUON_EFF_STAT", 1));
        // make sure the tool is not affected by other unsupported systematics in the same set
        statup.insert(CP::SystematicVariation("TÖRÖÖÖÖ", 1));

        // isolation systematic uncertainty
        CP::SystematicSet isosystdown;
        isosystdown.insert(CP::SystematicVariation("MUON_ISO_SYS", -1));

        xAOD::MuonContainer *SelectedMuon = new xAOD::MuonContainer;
        xAOD::MuonAuxContainer *SelectedMuonAux = new xAOD::MuonAuxContainer;
        SelectedMuon->setStore(SelectedMuonAux);
        CHECK(evtStore()->record(SelectedMuon, "SelectedMuon"));
        CHECK(evtStore()->record(SelectedMuonAux, "SelectedMuonAux."));

        for (; mu_itr != mu_end; ++mu_itr) {

            if (fabs((**mu_itr).eta()) > 2.7) continue;
            if ((**mu_itr).pt() < 10000) continue;

            // Print some info about the selected muon:
            ATH_MSG_INFO("  Selected muon: eta = " << (**mu_itr).eta()
                            << ", phi = " << (**mu_itr).phi()
                            << ", pt = " << (**mu_itr).pt());

            if ((**mu_itr).pt() > 120000) {
                ATH_MSG_INFO(" THIS IS A HIGH PT CANDIDATE!!");
            }
            // obtain a scale factor directly from the muon
            float sf = -1;
            if (!m_sf_Tool->applySystematicVariation(statup)) {
                ATH_MSG_WARNING("Unable to switch to stat up sys!");
                continue;
            }
            if (m_sf_Tool->getEfficiencyScaleFactor((**mu_itr), sf) == CP::CorrectionCode::Error) {
                ATH_MSG_WARNING("Couldn't run efficiencies on muon with stat up variation!");
                continue;
            }
            ATH_MSG_INFO("       Scale Factor (stat up) via getEfficiencyScaleFactor = " << sf);
            if (!m_sf_Tool->applySystematicVariation(statupLowPt)) {
                ATH_MSG_WARNING("Unable to switch to low-pt stat up sys!");
                continue;
            }
            if (m_sf_Tool->getEfficiencyScaleFactor((**mu_itr), sf) == CP::CorrectionCode::Error) {
                ATH_MSG_WARNING("Couldn't run efficiencies on muon with stat up variation!");
                continue;
            }
            ATH_MSG_INFO("       Scale Factor (low-pt stat up) via getEfficiencyScaleFactor = " << sf);
            if (!m_sf_Tool->applySystematicVariation(CP::SystematicSet())) {
                ATH_MSG_WARNING("Unable to switch to nominal sys!");
                continue;
            }
            if (!m_sf_Tool->applySystematicVariation(sysup)) {
                ATH_MSG_WARNING("Unable to switch to sys up sys!");
                continue;
            }
            if (m_sf_Tool->getEfficiencyScaleFactor((**mu_itr), sf) == CP::CorrectionCode::Error) {
                ATH_MSG_WARNING("Couldn't run efficiencies on muon with sys up variation!");
                continue;
            }
            ATH_MSG_INFO("       Scale Factor (sys up) via getEfficiencyScaleFactor = " << sf);
            if (!m_sf_Tool->applySystematicVariation(sysupLowPt)) {
                ATH_MSG_WARNING("Unable to switch to low pt sys up sys!");
                continue;
            }
            if (m_sf_Tool->getEfficiencyScaleFactor((**mu_itr), sf) == CP::CorrectionCode::Error) {
                ATH_MSG_WARNING("Couldn't run efficiencies on muon with low pt sys up variation!");
                continue;
            }
            ATH_MSG_INFO("       Scale Factor (low pt sys up) via getEfficiencyScaleFactor = " << sf);
            if (!m_sf_Tool->applySystematicVariation(CP::SystematicSet())) {
                ATH_MSG_WARNING("Unable to switch to nominal sys!");
                continue;
            }
            if (m_sf_Tool->getEfficiencyScaleFactor((**mu_itr), sf) == CP::CorrectionCode::Error) {
                ATH_MSG_WARNING("Couldn't run efficiencies on muon!");
                continue;
            }
            ATH_MSG_INFO("       Scale Factor (central value) via getEfficiencyScaleFactor = " << sf);
            // decorate the muon with SF info (alternative to the above)
            if (m_sf_Tool->applyEfficiencyScaleFactor(**mu_itr) == CP::CorrectionCode::Error || m_sf_Tool->applyDataEfficiency(**mu_itr) == CP::CorrectionCode::Error) {
                ATH_MSG_WARNING("Couldn't run efficiencies on muon!");
                continue;
            }

            // read the efficiency we decorated the muon with
            ATH_MSG_INFO("       efficiency from decorated Muon = "
                            << ((**mu_itr).isAvailable<float>("Efficiency") ? (**mu_itr).auxdata<float>("Efficiency") : -999.));
            ATH_MSG_INFO("       Scale Factor from decorated Muon = "
                            << (**mu_itr).auxdata<float>("EfficiencyScaleFactor"));
            // to estimate the stat uncertainty in a 'correct' way, roll replicas
            // and assign the variation of the final observable across the replicas as a systematic
            if (m_sf_Tool->applyEfficiencyScaleFactorReplicas((**mu_itr), 20) == CP::CorrectionCode::Error) {
                ATH_MSG_WARNING("Couldn't run efficiency replica generation on muon!");
                continue;
            }

            // here we load one of the replicas
            ATH_MSG_INFO("       Scale Factor Replica #2 = "
                            << (**mu_itr).auxdata < std::vector<float> > ("EfficiencyScaleFactorReplicas")[1]);

            ////////////////////////////////////////
            //// do the same thing for TTVA SFs
            ////////////////////////////////////////
            float ttvasf = -1;
            if (!m_ttvasf_Tool->applySystematicVariation(statup)) {
                ATH_MSG_WARNING("Unable to switch to stat up sys!");
                continue;
            }
            if (m_ttvasf_Tool->getEfficiencyScaleFactor((**mu_itr), ttvasf) == CP::CorrectionCode::Error) {
                ATH_MSG_WARNING("Couldn't run efficiencies on muon with stat up variation!");
                continue;
            }
            ATH_MSG_INFO("       Scale Factor (stat up) via getEfficiencyScaleFactor = " << ttvasf);
            if (!m_ttvasf_Tool->applySystematicVariation(statupLowPt)) {
                ATH_MSG_WARNING("Unable to switch to low-pt stat up sys!");
                continue;
            }
            if (m_ttvasf_Tool->getEfficiencyScaleFactor((**mu_itr), ttvasf) == CP::CorrectionCode::Error) {
                ATH_MSG_WARNING("Couldn't run efficiencies on muon with stat up variation!");
                continue;
            }
            ATH_MSG_INFO("       TTVA Scale Factor (low-pt stat up) via getEfficiencyScaleFactor = " << ttvasf);
            if (!m_ttvasf_Tool->applySystematicVariation(CP::SystematicSet())) {
                ATH_MSG_WARNING("Unable to switch to nominal sys!");
                continue;
            }
            if (!m_ttvasf_Tool->applySystematicVariation(sysup)) {
                ATH_MSG_WARNING("Unable to switch to sys up sys!");
                continue;
            }
            if (m_ttvasf_Tool->getEfficiencyScaleFactor((**mu_itr), ttvasf) == CP::CorrectionCode::Error) {
                ATH_MSG_WARNING("Couldn't run efficiencies on muon with sys up variation!");
                continue;
            }
            ATH_MSG_INFO("       TTVA Scale Factor (sys up) via getEfficiencyScaleFactor = " << ttvasf);
            if (!m_ttvasf_Tool->applySystematicVariation(sysupLowPt)) {
                ATH_MSG_WARNING("Unable to switch to low pt sys up sys!");
                continue;
            }
            if (m_ttvasf_Tool->getEfficiencyScaleFactor((**mu_itr), ttvasf) == CP::CorrectionCode::Error) {
                ATH_MSG_WARNING("Couldn't run efficiencies on muon with low pt sys up variation!");
                continue;
            }
            ATH_MSG_INFO("       TTVA Scale Factor (low pt sys up) via getEfficiencyScaleFactor = " << ttvasf);
            if (!m_ttvasf_Tool->applySystematicVariation(CP::SystematicSet())) {
                ATH_MSG_WARNING("Unable to switch to nominal sys!");
                continue;
            }
            if (m_ttvasf_Tool->getEfficiencyScaleFactor((**mu_itr), ttvasf) == CP::CorrectionCode::Error) {
                ATH_MSG_WARNING("Couldn't run efficiencies on muon!");
                continue;
            }
            ATH_MSG_INFO("       TTVA Scale Factor (central value) via getEfficiencyScaleFactor = " << ttvasf);
            // decorate the muon with SF info (alternative to the above)
            if (m_ttvasf_Tool->applyEfficiencyScaleFactor(**mu_itr) == CP::CorrectionCode::Error || m_ttvasf_Tool->applyDataEfficiency(**mu_itr) == CP::CorrectionCode::Error) {
                ATH_MSG_WARNING("Couldn't run TTVA efficiencies on muon!");
                continue;
            }

            ////////////////////////////////////////
            //// do the same thing for isolation
            ////////////////////////////////////////

            // obtain the isolation scale factor directly from the muon
            float isosf = -1;
            if (!m_isosf_Tool->applySystematicVariation(isosystdown)) {
                ATH_MSG_WARNING("Unable to switch to iso syst down sys!");
                continue;
            }
            if (m_isosf_Tool->getEfficiencyScaleFactor((**mu_itr), isosf) == CP::CorrectionCode::Error) {
                ATH_MSG_WARNING("Couldn't run ISO efficiencies on muon with syst down variation!");
                continue;
            }
            ATH_MSG_INFO("      Isolation Scale Factor (syst down) via getEfficiencyScaleFactor = "
                            << isosf);
            if (!m_isosf_Tool->applySystematicVariation(CP::SystematicSet())) {
                ATH_MSG_WARNING("Unable to switch to nominal sys for Isolation!");
                continue;
            }
            if (m_isosf_Tool->getEfficiencyScaleFactor((**mu_itr), isosf) == CP::CorrectionCode::Error) {
                ATH_MSG_WARNING("Couldn't run isolation efficiencies on muon!");
                continue;
            }
            ATH_MSG_INFO("       Isolation Scale Factor (central value) via getEfficiencyScaleFactor = "
                            << isosf);
            // decorate the muon with SF info (alternative to the above)
            if (m_isosf_Tool->applyEfficiencyScaleFactor(**mu_itr) == CP::CorrectionCode::Error || m_isosf_Tool->applyDataEfficiency(**mu_itr) == CP::CorrectionCode::Error) {
                ATH_MSG_WARNING("Couldn't run isolation efficiencies on muon!");
                continue;
            }

            // read the efficiency we decorated the muon with
            ATH_MSG_INFO("       isolation efficiency from decorated Muon = "
                            << ((**mu_itr).isAvailable<float>("ISOEfficiency") ? (**mu_itr).auxdata<float>("ISOEfficiency") : -999.));
            ATH_MSG_INFO("       Isolation Scale Factor from decorated Muon = "
                            << (**mu_itr).auxdata<float>("ISOEfficiencyScaleFactor"));
            // to estimate the stat uncertainty in a 'correct' way, roll replicas
            // and assign the variation of the final observable across the replicas as a systematic
            if (m_isosf_Tool->applyEfficiencyScaleFactorReplicas((**mu_itr), 20) == CP::CorrectionCode::Error) {
                ATH_MSG_WARNING("Couldn't run isolation efficiency replica generation on muon!");
                continue;
            }

            // here we load one of the replicas
            ATH_MSG_INFO("       Isolation Scale Factor Replica #2 = "
                            << (**mu_itr).auxdata < std::vector<float> > ("ISOEfficiencyScaleFactorReplicas")[1]);

            ///////// Isolation done! //////

            if ((**mu_itr).quality() != 1) continue; // 0:Tight, 1:Medium, 2:Loose, 3:VeryLoose
            xAOD::Muon* newMuon = new xAOD::Muon;
            SelectedMuon->push_back(newMuon);
            *newMuon = (**mu_itr);
        }

        xAOD::MuonContainer* MediumMuons = 0;
        ATH_CHECK(evtStore()->retrieve(MediumMuons, "SelectedMuon"));
        ATH_MSG_INFO("Number of selected muons: " << MediumMuons->size());
        Double_t triggerSF = 0;

        std::string singletrig = "HLT_mu20_iloose_L1MU15_OR_HLT_mu50";
        std::string multitrig = "HLT_mu20_iloose_L1MU15_OR_HLT_mu50_HLT_2mu14";
        
        // Systematic up
        CP::SystematicSet trigsfsys_up;
        trigsfsys_up.insert(CP::SystematicVariation("MUON_EFF_TrigUncertainty", 1));

        if (!m_trigsf_Tool->applySystematicVariation(trigsfsys_up)) {
            ATH_MSG_WARNING("Unable to switch to sys up!");
        }
        if (m_trigsf_Tool->getTriggerScaleFactor(*MediumMuons, triggerSF, singletrig) == CP::CorrectionCode::Error) {
            ATH_MSG_WARNING("Couldn't trigger efficiency! trigger scale factor = 0");
            triggerSF = 0;
        }
        ATH_MSG_INFO("Trigger Scale Factor (sys up) via getTriggerScaleFactor = " << triggerSF);

        // Systematic down
        CP::SystematicSet trigsfsys_down;
        trigsfsys_down.insert(CP::SystematicVariation("MUON_EFF_TrigUncertainty", -1));
        if (!m_trigsf_Tool->applySystematicVariation(trigsfsys_down)) {
            ATH_MSG_WARNING("Unable to switch to sys down!");
        }
        if (m_trigsf_Tool->getTriggerScaleFactor(*MediumMuons, triggerSF, singletrig) == CP::CorrectionCode::Error) {
            ATH_MSG_WARNING("Couldn't trigger efficiency! trigger scale factor = 0");
            triggerSF = 0;
        }
        ATH_MSG_INFO("Trigger Scale Factor (sys down) via getTriggerScaleFactor = " << triggerSF);

        // Return gracefully:
        return StatusCode::SUCCESS;
    }

} // namespace CP
