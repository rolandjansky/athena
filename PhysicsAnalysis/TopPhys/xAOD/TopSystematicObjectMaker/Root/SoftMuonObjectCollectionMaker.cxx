/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

#include "TopSystematicObjectMaker/SoftMuonObjectCollectionMaker.h"
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
  SoftMuonObjectCollectionMaker::SoftMuonObjectCollectionMaker(const std::string& name) :
    asg::AsgTool(name),
    m_config(nullptr),

    m_specifiedSystematics(),
    m_recommendedSystematics(),

    m_calibrationPeriodTool("CP::MuonCalibrationPeriodTool"),
    m_muonSelectionToolVeryLooseVeto("CP::MuonSelectionToolVeryLooseVeto") {
    declareProperty("config", m_config);

    declareProperty("MuonCalibrationPeriodTool", m_calibrationPeriodTool);
    declareProperty("MuonSelectionToolVeryLooseVeto", m_muonSelectionToolVeryLooseVeto);
  }

  StatusCode SoftMuonObjectCollectionMaker::initialize() {
    ATH_MSG_INFO(" top::SoftMuonObjectCollectionMaker initialize");

    top::check(m_calibrationPeriodTool.retrieve(), "Failed to retrieve muon calibration tool");
    top::check(m_muonSelectionToolVeryLooseVeto.retrieve(), "Failed to retrieve Selection Tool");

    ///-- Set Systematics Information --///
    const std:: string& syststr = m_config->systematics();
    std::set<std::string> syst;

    if (!m_config->isSystNominal(syststr) && !m_config->isSystAll(syststr)) {
      bool ok = m_config->getSystematicsList(syststr, syst);
      if (!ok) {
        ATH_MSG_ERROR(" top::SoftMuonObjectCollectionMaker could not determine systematic list");
        return StatusCode::FAILURE;
      }
      //here the idea is that if the user specifies AllXXX, we leave syst as an empty string, so that all recommended CP
      // systematics are then used
      if (m_config->contains(syst, "AllMuons")) {
        syst.clear();
      }
      if (m_config->contains(syst, "AllSoftMuons")) {
        syst.clear();
      }
    }

    specifiedSystematics(syst);

    m_config->systematicsSoftMuons(specifiedSystematics());

    ATH_MSG_INFO(" top::SoftMuonObjectCollectionMaker completed initialize");
    return StatusCode::SUCCESS;
  }

  StatusCode SoftMuonObjectCollectionMaker::execute(bool executeNominal) {
    const xAOD::EventInfo* eventInfo(nullptr);

    top::check(evtStore()->retrieve(eventInfo, m_config->sgKeyEventInfo()), "Failed to retrieve EventInfo");
    const float beam_pos_sigma_x = eventInfo->beamPosSigmaX();
    const float beam_pos_sigma_y = eventInfo->beamPosSigmaY();
    const float beam_pos_sigma_xy = eventInfo->beamPosSigmaXY();

    ///-- Get base muons and tracks from xAOD --///
    const xAOD::MuonContainer* xaod(nullptr);

    top::check(evtStore()->retrieve(xaod, m_config->sgKeyMuons()), "Failed to retrieve Soft Muons");  //we use sgKeyMuons and not sgKeySoftMuons on purpose to use the same collection for muons and soft muons

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
      for (xAOD::Muon* muon : *(shallow_xaod_copy.first)) {
        
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
        }//end of if (muon->primaryTrackParticle())
        
      }//end of loop on muons

      ///-- set links to original objects ///
      bool setLinks = xAOD::setOriginalObjectLink(*xaod, *shallow_xaod_copy.first);
      if (!setLinks) {
        ATH_MSG_ERROR(" Cannot set original object links for soft muons");
        return StatusCode::FAILURE;
      }

      ///-- Save corrected xAOD Container to StoreGate / TStore --///
      std::string outputSGKey = m_config->sgKeySoftMuons(systematic.hash());
      std::string outputSGKeyAux = outputSGKey + "Aux.";

      xAOD::TReturnCode save = evtStore()->tds()->record(shallow_xaod_copy.first, outputSGKey);
      xAOD::TReturnCode saveAux = evtStore()->tds()->record(shallow_xaod_copy.second, outputSGKeyAux);
      if (!save || !saveAux) {
        return StatusCode::FAILURE;
      }
    }  // Loop over all systematics

    return StatusCode::SUCCESS;
  }

  StatusCode SoftMuonObjectCollectionMaker::printout() {
    ///-- Loop over all systematics --///
    for (auto systematic : m_specifiedSystematics) {
      const xAOD::MuonContainer* xaod(nullptr);
      top::check(evtStore()->retrieve(xaod, m_config->sgKeySoftMuons(
                                        systematic.hash())), "Failed to retrieve Soft Muons");

      ATH_MSG_INFO(" Soft Muons with sgKey = " << m_config->sgKeySoftMuons(systematic.hash()));
      for (auto muon : *xaod) {
        ATH_MSG_INFO("   SOFT MU pT , eta  = " << muon->pt() << " , " << muon->eta());
      }
    }

    return StatusCode::SUCCESS;
  }

  void SoftMuonObjectCollectionMaker::specifiedSystematics(const std::set<std::string>& specifiedSystematics) {
    ///-- Get the recommended systematics from the tool, in std::vector format --///
    const std::vector<CP::SystematicSet> systList = CP::make_systematics_vector(
      m_calibrationPeriodTool->recommendedSystematics());

    for (const CP::SystematicSet& s : systList) {
      
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
