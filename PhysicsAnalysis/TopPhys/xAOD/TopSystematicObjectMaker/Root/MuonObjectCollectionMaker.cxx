/*
   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 */

// $Id: MuonObjectCollectionMaker.cxx 810751 2017-09-29 14:41:39Z iconnell $
#include "TopSystematicObjectMaker/MuonObjectCollectionMaker.h"
#include "TopConfiguration/TopConfig.h"
#include "TopConfiguration/TreeFilter.h"
#include "TopEvent/EventTools.h"

#include "AthContainers/AuxElement.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODMuon/MuonAuxContainer.h"
#include "xAODCore/ShallowCopy.h"
#include "xAODBase/IParticleHelpers.h"
#include "xAODTracking/TrackParticlexAODHelpers.h"
#include "PATInterfaces/SystematicsUtil.h"

namespace top {
  MuonObjectCollectionMaker::MuonObjectCollectionMaker(const std::string& name) :
    asg::AsgTool(name),
    m_config(nullptr),

    m_specifiedSystematics(),
    m_recommendedSystematics(),

    m_calibrationPeriodTool("MuonCalibrationPeriodTool"),

    m_muonSelectionToolVeryLooseVeto("MuonSelectionToolVeryLooseVeto") {
    declareProperty("config", m_config);

    declareProperty("MuonCalibrationPeriodTool", m_calibrationPeriodTool);
    declareProperty("MuonSelectionToolVeryLooseVeto", m_muonSelectionToolVeryLooseVeto);
  }

  StatusCode MuonObjectCollectionMaker::initialize() {
    ATH_MSG_INFO(" top::MuonObjectCollectionMaker initialize");

    for (const std::string& WP : m_config->muonIsolationWPs()) {
      m_muonIsolationTools[WP] = ToolHandle<CP::IIsolationSelectionTool>("IsolationTool_MuonWP_" + WP);
      top::check(m_muonIsolationTools[WP].retrieve(), "Failed to retrieve muon isolation tool for WP " + WP);
    }

    top::check(m_muonSelectionToolVeryLooseVeto.retrieve(), "Failed to retrieve Selection Tool");

    ///-- Set Systematics Information --///
    const std:: string& syststr = m_config->systematics();
    std::set<std::string> syst;

    if (!m_config->isSystNominal(syststr) && !m_config->isSystAll(syststr)) {
      bool ok = m_config->getSystematicsList(syststr, syst);
      if (!ok) {
        ATH_MSG_ERROR(" top::MuonObjectCollectionMaker could not determine systematic list");
        return StatusCode::FAILURE;
      }
      //here the idea is that if the user specifies AllXXX, we leave syst as an empty string, so that all recommended CP
      // systematics are then used
      if (m_config->contains(syst, "AllMuons")) {
        syst.clear();
      }
    }

    specifiedSystematics(syst);

    m_config->systematicsMuons(specifiedSystematics());

    m_isFirstEvent = true;

    ATH_MSG_INFO(" top::MuonObjectCollectionMaker completed initialize");
    return StatusCode::SUCCESS;
  }

  StatusCode MuonObjectCollectionMaker::execute(bool executeNominal) {

    static const SG::AuxElement::ConstAccessor<int> chamberIndex("chamberIndex");
    static const SG::AuxElement::ConstAccessor<short> PLV_TrackJetNTrack("PromptLeptonInput_TrackJetNTrack");
    static const SG::AuxElement::ConstAccessor<float> PLV_DRlj("PromptLeptonInput_DRlj");
    static const SG::AuxElement::ConstAccessor<float> PLV_PtRel("PromptLeptonInput_PtRel");
    static const SG::AuxElement::ConstAccessor<float> PLV_PtFrac("PromptLeptonInput_PtFrac");
    static const SG::AuxElement::ConstAccessor<float> PLV_PromptLeptonVeto("PromptLeptonVeto");
//    static SG::AuxElement::Decorator<float> byhand_LowPtPLV("LowPtPLV");

    const xAOD::EventInfo* eventInfo(nullptr);

    top::check(evtStore()->retrieve(eventInfo, m_config->sgKeyEventInfo()), "Failed to retrieve EventInfo");
    float beam_pos_sigma_x = eventInfo->beamPosSigmaX();
    float beam_pos_sigma_y = eventInfo->beamPosSigmaY();
    float beam_pos_sigma_xy = eventInfo->beamPosSigmaXY();

    ///-- Get base muons and tracks from xAOD --///
    const xAOD::MuonContainer* xaod(nullptr);
    top::check(evtStore()->retrieve(xaod, m_config->sgKeyMuons()), "Failed to retrieve Muons");

    ///-- Loop over all systematics --///
    for (auto systematic : m_specifiedSystematics) {
      ///-- if executeNominal, skip other systematics (and vice-versa) --///
      if (executeNominal && !m_config->isSystNominal(m_config->systematicName(systematic.hash()))) continue;
      if (!executeNominal && m_config->isSystNominal(m_config->systematicName(systematic.hash()))) continue;

      ///-- Tell tool which systematic to use --///
      top::check(m_calibrationPeriodTool->applySystematicVariation(systematic), "Failed to applySystematicVariation");

      ///-- Shallow copy of the xAOD --///
      std::pair< xAOD::MuonContainer*,
                 xAOD::ShallowAuxContainer* > shallow_xaod_copy = xAOD::shallowCopyContainer(*xaod);

      ///-- Loop over the xAOD Container and apply corrections--///
      for (auto muon : *(shallow_xaod_copy.first)) {
	
        ///-- Apply momentum correction --///
        if (muon->primaryTrackParticle()) {
          top::check(m_calibrationPeriodTool->applyCorrection(*muon), "Failed to applyCorrection");

          // don't do the decorations unless the muons are at least Loose
          // this is because it may fail if the muons are at just VeryLoose
          if (m_muonSelectionToolVeryLooseVeto->accept(*muon)) {
            double d0sig = xAOD::TrackingHelpers::d0significance(muon->primaryTrackParticle(),
                                                                 beam_pos_sigma_x,
                                                                 beam_pos_sigma_y,
                                                                 beam_pos_sigma_xy);
            muon->auxdecor<float>("d0sig") = d0sig;

            if (eventInfo->isAvailable<float>("AnalysisTop_PRIVTX_z_position")) {
              float vtx_z = eventInfo->auxdata<float>("AnalysisTop_PRIVTX_z_position");
              float delta_z0 = muon->primaryTrackParticle()->z0() + muon->primaryTrackParticle()->vz() - vtx_z;
              muon->auxdecor<float>("delta_z0") = delta_z0;
              muon->auxdecor<float>("delta_z0_sintheta") = delta_z0 * std::sin(muon->primaryTrackParticle()->theta());
            }
          }
        }

        ///-- Isolation selection --///
        for (const auto& muonIsoWP : m_muonIsolationTools) {
          const char passIsol = (muonIsoWP.second->accept(*muon)) ? 1 : 0;
          muon->auxdecor<char>("AnalysisTop_Isol_" + muonIsoWP.first) = passIsol;
        }
      } // end loop over muons

      ///-- set links to original objects- needed for MET calculation --///
      bool setLinks = xAOD::setOriginalObjectLink(*xaod, *shallow_xaod_copy.first);
      if (!setLinks) ATH_MSG_ERROR(" Cannot set original object links for muons, MET recalculation may struggle");

      ///-- Save corrected xAOD Container to StoreGate / TStore --///
      std::string outputSGKey = m_config->sgKeyMuons(systematic.hash());
      std::string outputSGKeyAux = outputSGKey + "Aux.";

      StatusCode save = evtStore()->tds()->record(shallow_xaod_copy.first, outputSGKey);
      StatusCode saveAux = evtStore()->tds()->record(shallow_xaod_copy.second, outputSGKeyAux);
      if (!save || !saveAux) {
        return StatusCode::FAILURE;
      }
    }  // Loop over all systematics

    return StatusCode::SUCCESS;
  }

  StatusCode MuonObjectCollectionMaker::printout() {
    ///-- Loop over all systematics --///
    for (auto systematic : m_specifiedSystematics) {
      const xAOD::MuonContainer* xaod(nullptr);
      top::check(evtStore()->retrieve(xaod, m_config->sgKeyMuons(systematic.hash())), "Failed to retrieve Muons");

      ATH_MSG_INFO(" Muons with sgKey = " << m_config->sgKeyMuons(systematic.hash()));
      for (auto muon : *xaod) {
        ATH_MSG_INFO("   MU pT , eta  = " << muon->pt() << " , " << muon->eta());
      }
    }

    return StatusCode::SUCCESS;
  }

  void MuonObjectCollectionMaker::specifiedSystematics(const std::set<std::string>& specifiedSystematics) {
    ///-- Get the recommended systematics from the tool, in std::vector format --///
    const std::vector<CP::SystematicSet> systList = CP::make_systematics_vector(
      m_calibrationPeriodTool->recommendedSystematics());

    for (auto s : systList) {
      
      if(!m_config->getTreeFilter()->filterTree(s.name())) continue; // Applying tree filter
      m_recommendedSystematics.push_back(s);
      if (s.name() == "") {
        m_specifiedSystematics.push_back(s);
      }

      ///-- MC only --///
      if (m_config->isMC()) {
        ///-- Are we only doing Nominal? Did the user specify specific systematics to use? --///
        if (!m_config->isSystNominal(m_config->systematics())) {
          if (specifiedSystematics.size() == 0) {
            m_specifiedSystematics.push_back(s);
          }
          if (specifiedSystematics.size() > 0) {
            for (auto i : specifiedSystematics) {
              TreeFilter filter(i);
              if (!filter.filterTree(s.name())) {
                m_specifiedSystematics.push_back(s);
              }
            }
          }
        }
      }
    }
    m_recommendedSystematics.sort();
    m_recommendedSystematics.unique();
    m_specifiedSystematics.sort();
    m_specifiedSystematics.unique();
  }
}
