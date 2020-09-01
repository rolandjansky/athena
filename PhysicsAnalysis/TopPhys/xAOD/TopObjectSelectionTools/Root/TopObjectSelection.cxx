/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

#include "TopObjectSelectionTools/TopObjectSelection.h"

#include "TopConfiguration/TopConfig.h"

#include "TopEvent/EventTools.h"
#include "TopEvent/SystematicEvent.h"

#include "xAODEgamma/PhotonContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODTau/TauJetContainer.h"
#include "xAODJet/JetContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODMissingET/MissingETContainer.h"
#include "xAODEventInfo/EventInfo.h"

#include "FourMomUtils/xAODP4Helpers.h"
#include "PATCore/TAccept.h"

#include "TopParticleLevel/TruthTools.h"
#include "xAODTruth/TruthParticle.h"
#include "xAODTruth/TruthParticleContainer.h"

#include "TopDataPreparation/SampleXsection.h"

namespace top {
  TopObjectSelection::TopObjectSelection(const std::string& name) :
    asg::AsgTool(name),
    m_config(nullptr),
    m_electronSelection(nullptr),
    m_fwdElectronSelection(nullptr),
    m_muonSelection(nullptr),
    m_softmuonSelection(nullptr),
    m_tauSelection(nullptr),
    m_jetSelection(nullptr),
    m_photonSelection(nullptr),
    m_largeJetSelection(nullptr),
    m_trackJetSelection(nullptr),
    m_jetGhostTrackSelection(nullptr),
    m_trackSelection(nullptr),
    m_overlapRemovalToolPostSelection(nullptr),
    m_electronInJetSubtractor(nullptr),
    m_passPreORSelection("passPreORSelection"),
    m_passPreORSelectionLoose("passPreORSelectionLoose"),
    // the following two are used to give failing JVT and failing fJVT jets a lower priority in the OR
    m_ORToolDecoration("ORToolDecoration"),
    m_ORToolDecorationLoose("ORToolDecorationLoose"),
    m_doLooseCuts(false),
    m_overlapRemovalTool_softMuons_PFjets("OverlapRemovalTool_softMuons_PFjets"),
    m_overlapRemovalTool_softMuons_Alljets("OverlapRemovalTool_softMuons_Alljets")
  {
    declareProperty("config", m_config);
  }

  StatusCode TopObjectSelection::initialize() {
    // If there are loose selection requirements, then we need additional computations
    // Tight leptons are not constrained to be a sub-set of loose
    // We will call the selections in the following order:
    //   (1) passSelection();
    //   (2) passSelectionLoose();
    //
    // If your tight is not a sub-set of your loose, then you have 2 different functions
    // If your tight is a sub-set of your loose, then I suggest you set a loose flag
    // when calling passSelection(), then have passSelectionLoose() return this flag
    //
    // Requests to process loose selections can come from:
    //   (1) Data event
    //   (2) User request overlap removal be done on loose objects
    //         - The top recommendation is that you do OR on tight objects
    //   (3) Determination of Fakes control regions in MC - expert fakes mode
    //
   
    if (m_config->useSoftMuons()) {
      top::check(m_overlapRemovalTool_softMuons_PFjets.retrieve(), "Failed to retrieve overlap removal tool for soft muons - PF jets");
      top::check(m_overlapRemovalTool_softMuons_Alljets.retrieve(), "Failed to retrieve overlap removal tool for soft muons - all jets");
    }

    if (!m_config->isMC()) m_doLooseCuts = true;

    if (m_config->doOverlapRemovalOnLooseLeptonDef()) m_doLooseCuts = true;

    if (m_config->doLooseEvents()) m_doLooseCuts = true;
    
    if(m_config->useLooseObjectsInMETInLooseTree() || m_config->useLooseObjectsInMETInNominalTree() || m_config->writeMETBuiltWithLooseObjects()) m_doLooseCuts = true;

    if (m_config->applyElectronInJetSubtraction()) {
      m_electronInJetSubtractor = std::unique_ptr<top::ElectronInJetSubtractionCollectionMaker>
                                    (new top::ElectronInJetSubtractionCollectionMaker(
                                      "top::ElectronInJetSubtractionCollectionMaker"));
      top::check(m_electronInJetSubtractor->setProperty("config",
                                                        m_config),
                 "Failed to setProperty of top::ElectronInJetSubtractionCollectionMaker");
      top::check(
        m_electronInJetSubtractor->initialize(), "Failed to initialize top::ElectronInJetSubtractionCollectionMaker");
    }

    // b-tagging stuff
    // for calo jets
    std::vector<std::string> availableWPs = m_config->bTagWP_available();
    for (auto& WP : availableWPs) {
      m_btagSelTools[WP] = "BTaggingSelectionTool_" + WP + "_" + m_config->sgKeyJets();
      top::check(m_btagSelTools[WP].retrieve(), "Failed to retrieve b-tagging Selection tool");
    }
    // for track jets
    availableWPs = m_config->bTagWP_available_trkJet();
    for (auto& WP : availableWPs) {
      m_trkjet_btagSelTools[WP] = "BTaggingSelectionTool_" + WP + "_" + m_config->sgKeyTrackJets();
      top::check(m_trkjet_btagSelTools[WP].retrieve(), "Failed to retrieve b-tagging Selection tool");
    }

    return StatusCode::SUCCESS;
  }

  void TopObjectSelection::electronSelection(ElectronSelectionBase* ptr) {
    m_electronSelection.reset(ptr);
  }

  void TopObjectSelection::fwdElectronSelection(FwdElectronSelectionBase* ptr) {
    m_fwdElectronSelection.reset(ptr);
  }

  void TopObjectSelection::muonSelection(MuonSelectionBase* ptr) {
    m_muonSelection.reset(ptr);
  }

  void TopObjectSelection::softmuonSelection(SoftMuonSelectionBase* ptr) {
    m_softmuonSelection.reset(ptr);
  }

  void TopObjectSelection::tauSelection(TauSelectionBase* ptr) {
    m_tauSelection.reset(ptr);
  }

  void TopObjectSelection::jetSelection(JetSelectionBase* ptr) {
    m_jetSelection.reset(ptr);
  }

  void TopObjectSelection::photonSelection(PhotonSelectionBase* ptr) {
    m_photonSelection.reset(ptr);
  }

  void TopObjectSelection::largeJetSelection(JetSelectionBase* ptr) {
    m_largeJetSelection.reset(ptr);
  }

  void TopObjectSelection::trackJetSelection(JetSelectionBase* ptr) {
    m_trackJetSelection.reset(ptr);
  }
  
  void TopObjectSelection::jetGhostTrackSelection(JetGhostTrackSelectionBase* ptr) {
    m_jetGhostTrackSelection.reset(ptr);
  }

  void TopObjectSelection::trackSelection(TrackSelectionBase* ptr) {
    m_trackSelection.reset(ptr);
  }


  void TopObjectSelection::overlapRemovalPostSelection(OverlapRemovalBase* ptr) {
    m_overlapRemovalToolPostSelection.reset(ptr);
  }

  StatusCode TopObjectSelection::execute(bool executeNominal) {
    // Set variable to tell us if we are doing this execution on nominal or systematic events
    m_executeNominal = executeNominal;
    applySelectionPreOverlapRemoval();
    top::check(applyOverlapRemoval(), "Failed to apply overlap removal");

    return StatusCode::SUCCESS;
  }

  void TopObjectSelection::applySelectionPreOverlapRemoval() {
    // If we are doing Electron in Jet subtraction,
    // We need to:
    //  (1) Apply object selection to the electrons
    //  (2) Subtract the electron from the jet, which:
    //      - Changes the jet 4-momentum
    //      - Reject additional electrons
    //  (3) Apply object selection to the modified jets

    if (m_config->useElectrons()) {
      applySelectionPreOverlapRemovalElectrons();
    }
    if (m_config->useFwdElectrons()) {
      applySelectionPreOverlapRemovalFwdElectrons();
    }

    if (m_config->applyElectronInJetSubtraction()) {
      top::check(m_electronInJetSubtractor->execute(
                   m_executeNominal), "Failed to execute top::ElectronInJetSubtractionCollectionMaker");
    }


    if (m_config->usePhotons() && m_photonSelection != nullptr) {
      applySelectionPreOverlapRemovalPhotons();
    }
    if (m_config->useMuons() && m_muonSelection != nullptr) {
      applySelectionPreOverlapRemovalMuons();
    }
    if (m_config->useSoftMuons() && m_softmuonSelection != nullptr) {
      applySelectionPreOverlapRemovalSoftMuons();
    }
    if (m_config->useTaus() && m_tauSelection != nullptr) {
      applySelectionPreOverlapRemovalTaus();
    }
    if (m_config->useJets() && m_jetSelection != nullptr) {
      applySelectionPreOverlapRemovalJets();
    }
    if (m_config->useLargeRJets() && m_largeJetSelection != nullptr) {
      applySelectionPreOverlapRemovalLargeRJets();
    }
    if (m_config->useTrackJets() && m_trackJetSelection != nullptr) {
      applySelectionPreOverlapRemovalTrackJets();
    }
    if (m_config->useJetGhostTrack() && m_jetGhostTrackSelection != nullptr) {
      applySelectionPreOverlapRemovalJetGhostTracks();
    }
    if (m_config->useTracks() && m_trackSelection != nullptr) {
      applySelectionPreOverlapRemovalTracks();
    }

  }

  void TopObjectSelection::applySelectionPreOverlapRemovalPhotons() {
    for (auto currentSystematic : *m_config->systSgKeyMapPhotons()) {
      ///-- if executeNominal, skip other systematics (and vice-versa) --///
      if (m_executeNominal && !m_config->isSystNominal(m_config->systematicName(currentSystematic.first))) continue;
      if (!m_executeNominal && m_config->isSystNominal(m_config->systematicName(currentSystematic.first))) continue;

      const xAOD::PhotonContainer* photons(nullptr);
      top::check(evtStore()->retrieve(photons,
                                      currentSystematic.second),
                 "TopObjectSelection::applySelectionPreOverlapRemovalPhotons() failed to retrieve photons");
      ATH_MSG_DEBUG(" Cut on Photons with key = " << currentSystematic.second);

      for (auto photonPtr : *photons) {
        photonPtr->auxdecor<char>(m_passPreORSelection) = m_photonSelection->passSelection(*photonPtr);
        photonPtr->auxdecor<char>(m_ORToolDecoration) = photonPtr->auxdataConst<char>(m_passPreORSelection) * 2;
        if (m_doLooseCuts) {
          photonPtr->auxdecor<char>(m_passPreORSelectionLoose) = m_photonSelection->passSelectionLoose(*photonPtr);
          photonPtr->auxdecor<char>(m_ORToolDecorationLoose) =
          photonPtr->auxdataConst<char>(m_passPreORSelectionLoose) * 2;
        }
      }
    }
  }

/**
 * @brief For each systematic load each of the electrons and test if they pass
 * the object selection.
 */
  void TopObjectSelection::applySelectionPreOverlapRemovalElectrons() {
    for (auto currentSystematic : *m_config->systSgKeyMapElectrons()) {
      ///-- if executeNominal, skip other systematics (and vice-versa) --///
      if (m_executeNominal && !m_config->isSystNominal(m_config->systematicName(currentSystematic.first))) continue;
      if (!m_executeNominal && m_config->isSystNominal(m_config->systematicName(currentSystematic.first))) continue;

      const xAOD::ElectronContainer* electrons(nullptr);
      top::check(evtStore()->retrieve(electrons,
                                      currentSystematic.second),
                 "TopObjectSelection::applySelectionPreOverlapRemovalElectrons() failed to retrieve electrons");
      ATH_MSG_DEBUG(" Cut on Electrons with key = " << currentSystematic.second);

      for (auto electronPtr : *electrons) {
        electronPtr->auxdecor<char>(m_passPreORSelection) = m_electronSelection->passSelection(*electronPtr);
        electronPtr->auxdecor<char>(m_ORToolDecoration) = electronPtr->auxdataConst<char>(m_passPreORSelection) * 2;
        if (m_doLooseCuts) {
          electronPtr->auxdecor<char>(m_passPreORSelectionLoose) =
            m_electronSelection->passSelectionLoose(*electronPtr);
          electronPtr->auxdecor<char>(m_ORToolDecorationLoose) = electronPtr->auxdataConst<char>(
            m_passPreORSelectionLoose) * 2;
        }
      }
    }
  }

/**
 * @brief For each systematic load each of the fwd electrons and test if they pass
 * the object selection.
 */
  void TopObjectSelection::applySelectionPreOverlapRemovalFwdElectrons() {
    for (const auto& currentSystematic : *m_config->systSgKeyMapFwdElectrons()) {
      ///-- if executeNominal, skip other systematics (and vice-versa) --///
      if (m_executeNominal && !m_config->isSystNominal(m_config->systematicName(currentSystematic.first))) continue;
      if (!m_executeNominal && m_config->isSystNominal(m_config->systematicName(currentSystematic.first))) continue;

      const xAOD::ElectronContainer* fwdelectrons(nullptr);
      top::check(evtStore()->retrieve(fwdelectrons,
                                      currentSystematic.second),
                 "TopObjectSelection::applySelectionPreOverlapRemovalFwdElectrons() failed to retrieve fwd electrons");
      ATH_MSG_DEBUG(" Cut on Electrons with key = " << currentSystematic.second);

      int bcid = 0;
      int runNumber = 0;
      if (!m_config->isMC()) {
        const xAOD::EventInfo* eventInfo(nullptr);
        top::check(evtStore()->retrieve(eventInfo,
                                        m_config->sgKeyEventInfo()),
                   "Failed to retrieve EventInfo in TopObjectSelection::applySelectionPreOverlapRemovalFwdElectrons()");
        bcid = eventInfo->bcid();
        runNumber = eventInfo->runNumber();
      }

      for (auto electronPtr : *fwdelectrons) {
        electronPtr->auxdecor<char>(m_passPreORSelection) = m_fwdElectronSelection->passSelection(*electronPtr, bcid,
                                                                                                  runNumber);
        electronPtr->auxdecor<char>(m_ORToolDecoration) = electronPtr->auxdataConst<char>(m_passPreORSelection) * 2;
        if (m_doLooseCuts) {
          electronPtr->auxdecor<char>(m_passPreORSelectionLoose) = m_fwdElectronSelection->passSelectionLoose(
            *electronPtr, bcid, runNumber);
          electronPtr->auxdecor<char>(m_ORToolDecorationLoose) = electronPtr->auxdataConst<char>(
            m_passPreORSelectionLoose) * 2;
        }
      }//end of loop on electrons
    }//end of loop on systematics
  }

  void TopObjectSelection::applySelectionPreOverlapRemovalMuons() {
    for (auto currentSystematic : *m_config->systSgKeyMapMuons()) {
      ///-- if executeNominal, skip other systematics (and vice-versa) --///
      if (m_executeNominal && !m_config->isSystNominal(m_config->systematicName(currentSystematic.first))) continue;
      if (!m_executeNominal && m_config->isSystNominal(m_config->systematicName(currentSystematic.first))) continue;
      const xAOD::MuonContainer* muons(nullptr);
      top::check(evtStore()->retrieve(muons,
                                      currentSystematic.second),
                 "TopObjectSelection::applySelectionPreOverlapRemovalMuons() failed to retrieve muons");

      for (auto muonPtr : *muons) {
        muonPtr->auxdecor<char>(m_passPreORSelection) = m_muonSelection->passSelection(*muonPtr);
        muonPtr->auxdecor<char>(m_ORToolDecoration) = muonPtr->auxdataConst<char>(m_passPreORSelection) * 2;
        if (m_doLooseCuts) {
          muonPtr->auxdecor<char>(m_passPreORSelectionLoose) = m_muonSelection->passSelectionLoose(*muonPtr);
          muonPtr->auxdecor<char>(m_ORToolDecorationLoose) = muonPtr->auxdataConst<char>(m_passPreORSelectionLoose) * 2;
        }
      }
    }
  }

  void TopObjectSelection::applySelectionPreOverlapRemovalSoftMuons() {
    for (const std::pair<std::size_t, std::string>& currentSystematic : *m_config->systSgKeyMapSoftMuons()) {
      ///-- if executeNominal, skip other systematics (and vice-versa) --///
      if (m_executeNominal && !m_config->isSystNominal(m_config->systematicName(currentSystematic.first))) continue;
      if (!m_executeNominal && m_config->isSystNominal(m_config->systematicName(currentSystematic.first))) continue;
      const xAOD::MuonContainer* softmuons(nullptr);
      top::check(evtStore()->retrieve(softmuons,
                                      currentSystematic.second),
                 "TopObjectSelection::applySelectionPreOverlapRemovalSoftMuons() failed to retrieve soft muons");

      for (const xAOD::Muon* softmuonPtr : *softmuons) {
        softmuonPtr->auxdecor<char>(m_passPreORSelection) = m_softmuonSelection->passSelection(*softmuonPtr);
        softmuonPtr->auxdecor<char>(m_ORToolDecoration) = softmuonPtr->auxdataConst<char>(m_passPreORSelection) * 2;
      }
    }
  }

  void TopObjectSelection::applySelectionPreOverlapRemovalTaus() {
    for (auto currentSystematic : *m_config->systSgKeyMapTaus()) {
      ///-- if executeNominal, skip other systematics (and vice-versa) --///
      if (m_executeNominal && !m_config->isSystNominal(m_config->systematicName(currentSystematic.first))) continue;
      if (!m_executeNominal && m_config->isSystNominal(m_config->systematicName(currentSystematic.first))) continue;
      const xAOD::TauJetContainer* taus(nullptr);
      top::check(evtStore()->retrieve(taus,
                                      currentSystematic.second),
                 "TopObjectSelection::applySelectionPreOverlapRemovalTaus() failed to retrieve taus");

      for (auto tauPtr : *taus) {
        tauPtr->auxdecor<char>(m_passPreORSelection) = m_tauSelection->passSelection(*tauPtr);
        tauPtr->auxdecor<char>(m_ORToolDecoration) = tauPtr->auxdataConst<char>(m_passPreORSelection) * 2;
        if (m_doLooseCuts) {
          tauPtr->auxdecor<char>(m_passPreORSelectionLoose) = m_tauSelection->passSelectionLoose(*tauPtr);
          tauPtr->auxdecor<char>(m_ORToolDecorationLoose) = tauPtr->auxdataConst<char>(m_passPreORSelectionLoose) * 2;
        }
      }
    }
  }

  void TopObjectSelection::applySelectionPreOverlapRemovalJets() {
    // Boosted analysis subtract electrons from jets
    // If we use Loose electrons we end up with loose jet collections

    bool isLooseJets(false);

    for (auto currentSystematic : *m_config->systSgKeyMapJets(isLooseJets)) {
      ///-- if executeNominal, skip other systematics (and vice-versa) --///
      if (m_executeNominal && !m_config->isSystNominal(m_config->systematicName(currentSystematic.first))) continue;
      if (!m_executeNominal && m_config->isSystNominal(m_config->systematicName(currentSystematic.first))) continue;

      const xAOD::JetContainer* jets(nullptr);
      top::check(evtStore()->retrieve(jets,
                                      currentSystematic.second),
                 "TopObjectSelection::applySelectionPreOverlapRemovalJets() failed to retrieve jets");
      ATH_MSG_DEBUG(" Cut on Jets with key = " << currentSystematic.second);

      for (auto jetPtr : *jets) {
        ATH_MSG_DEBUG("   Jet pt = " << (jetPtr)->pt());
        bool passed = m_jetSelection->passSelection(*jetPtr);

	//Forward jets always get JVT=1, Central jets always get fJVT=1
	bool passedJVT_and_fJVT = true;
	if (jetPtr->isAvailable<char>("passJVT")) {
	  if (jetPtr->isAvailable<char>("AnalysisTop_fJVTdecision")) {
	    passedJVT_and_fJVT = jetPtr->auxdataConst<char>("passJVT") && jetPtr->auxdataConst<char>("AnalysisTop_fJVTdecision"); 
	  }
	  else {
	    passedJVT_and_fJVT = jetPtr->auxdataConst<char>("passJVT");
	  } 
	}
	else if (jetPtr->isAvailable<char>("AnalysisTop_fJVTdecision")) { //Possibly redundant, fJVT shouldn't really be able to run if passJVT isn't avaliable
	  passedJVT_and_fJVT = jetPtr->auxdataConst<char>("AnalysisTop_fJVTdecision");
	}

        if (m_config->applyElectronInJetSubtraction()) {
          if (jetPtr->isAvailable<char>("passesFancyOR")) {
            if (!jetPtr->auxdecor<char>("passesFancyOR")) passed = false;
          }
        }
	// if JVT or fJVT cut enabled and valid: jets that pass (f)JVT get passPreORSelection * 2, jets that fail get the same as passPreORSelection
	// if no JVT cut and central jet, or no fJVT and forward jet, jet gets: passPreORSelection * 2
        jetPtr->auxdecor<char>(m_passPreORSelection) = passed;
        jetPtr->auxdecor<char>(m_ORToolDecoration) = (passed ? (passedJVT_and_fJVT ? 2 : 1) : 0);
        if (m_doLooseCuts) {
          jetPtr->auxdecor<char>(m_passPreORSelectionLoose) = passed;
          jetPtr->auxdecor<char>(m_ORToolDecorationLoose) = (passed ? (passedJVT_and_fJVT ? 2 : 1) : 0);
        }
        //decorate with b-tagging flags
        std::vector<std::string> availableWPs = m_config->bTagWP_available();
        for (auto& WP : availableWPs) {
          if (WP.find("Continuous") == std::string::npos) {
            bool isTagged = false;
            if (std::fabs(jetPtr->eta()) <= 2.5) {
              ToolHandle<IBTaggingSelectionTool>& btagsel = m_btagSelTools[WP];
              isTagged = btagsel->accept(*jetPtr);
            }
            jetPtr->auxdecor<char>("isbtagged_" + WP) = isTagged;
          } else {
            int tagWeightBin = -2; // AT default
            if (std::fabs(jetPtr->eta()) <= 2.5) {
              ToolHandle<IBTaggingSelectionTool>& btagsel = m_btagSelTools[WP];
              tagWeightBin = btagsel->getQuantile(*jetPtr);
            }
            jetPtr->auxdecor<int>("tagWeightBin_" + WP) = tagWeightBin;
          }
        }
      }
    }

    // Are we using the ElectronInJetSubtraction and running with loose lepton definitons
    if (m_doLooseCuts && m_config->applyElectronInJetSubtraction()) {
      for (auto currentSystematic : *m_config->systSgKeyMapJets(m_doLooseCuts)) {
        ///-- if executeNominal, skip other systematics (and vice-versa) --///
        if (m_executeNominal && !m_config->isSystNominal(m_config->systematicName(currentSystematic.first))) continue;
        if (!m_executeNominal && m_config->isSystNominal(m_config->systematicName(currentSystematic.first))) continue;

        const xAOD::JetContainer* jets(nullptr);
        top::check(evtStore()->retrieve(jets,
                                        currentSystematic.second),
                   "TopObjectSelection::applySelectionPreOverlapRemovalJets() failed to retrieve jets");
        ATH_MSG_DEBUG(" Cut on Jets with key = " << currentSystematic.second);

        for (auto jetPtr : *jets) {
          bool decoration = m_jetSelection->passSelection(*jetPtr);

	  //Forward jets always get JVT=1, Central jets always get fJVT=1 
	  bool passedJVT_and_fJVT = true;
	  if (jetPtr->isAvailable<char>("passJVT")) {
	    if (jetPtr->isAvailable<char>("AnalysisTop_fJVTdecision")) {
	      passedJVT_and_fJVT = jetPtr->auxdataConst<char>("passJVT") && jetPtr->auxdataConst<char>("AnalysisTop_fJVTdecision");
	    }
	    else {
	      passedJVT_and_fJVT = jetPtr->auxdataConst<char>("passJVT");
	    } 
	  }
	  else if (jetPtr->isAvailable<char>("AnalysisTop_fJVTdecision")) { //Possibly redundant, fJVT shouldn't really be able to run if passJVT isn't avaliable
	    passedJVT_and_fJVT = jetPtr->auxdataConst<char>("AnalysisTop_fJVTdecision");
	  }

          // if JVT or fJVT cut enabled: jets that pass (f)JVT get passPreORSelection*2, jets which fail get the same as passPreORSelection
          // if no JVT cut and central jet, or no fJVT cut and forward jet, jet gets: passPreORSelection * 2
          jetPtr->auxdecor<char>(m_passPreORSelection) = decoration;
          jetPtr->auxdecor<char>(m_passPreORSelectionLoose) = decoration;

	  jetPtr->auxdecor<char>(m_ORToolDecoration) = (decoration ? (passedJVT_and_fJVT ? 2 : 1) : 0);
	  jetPtr->auxdecor<char>(m_ORToolDecorationLoose) = (decoration ? (passedJVT_and_fJVT ? 2 : 1) : 0);;

          //decorate with b-tagging flags
          std::vector<std::string> availableWPs = m_config->bTagWP_available();
          for (auto& WP : availableWPs) {
            if (WP.find("Continuous") == std::string::npos) {
              bool isTagged = false;
              if (std::fabs(jetPtr->eta()) < 2.5) {
                ToolHandle<IBTaggingSelectionTool>& btagsel = m_btagSelTools[WP];
                isTagged = btagsel->accept(*jetPtr);
              }
              jetPtr->auxdecor<char>("isbtagged_" + WP) = isTagged;
            } else {
              int tagWeightBin = -2; // AT default
              if (std::fabs(jetPtr->eta()) < 2.5) {
                ToolHandle<IBTaggingSelectionTool>& btagsel = m_btagSelTools[WP];
                tagWeightBin = btagsel->getQuantile(*jetPtr);
              }
              jetPtr->auxdecor<int>("tagWeightBin_" + WP) = tagWeightBin;
            }
          }
        }
      }
    }
  }

  void TopObjectSelection::applySelectionPreOverlapRemovalLargeRJets() {
    for (auto currentSystematic : *m_config->systSgKeyMapLargeRJets()) {
      ///-- if executeNominal, skip other systematics (and vice-versa) --///
      if (m_executeNominal && !m_config->isSystNominal(m_config->systematicName(currentSystematic.first))) continue;
      if (!m_executeNominal && m_config->isSystNominal(m_config->systematicName(currentSystematic.first))) continue;

      const xAOD::JetContainer* jets(nullptr);
      top::check(evtStore()->retrieve(jets,
                                      currentSystematic.second),
                 "TopObjectSelection::applySelectionPreOverlapRemovalLargeRJets() failed to retrieve large R jets");

      for (auto jetPtr : *jets) {
        char decoration = m_largeJetSelection->passSelection(*jetPtr);
        jetPtr->auxdecor<char>(m_passPreORSelection) = decoration;
        jetPtr->auxdecor<char>(m_ORToolDecoration) = decoration * 2;
        if (m_doLooseCuts) {
          jetPtr->auxdecor<char>(m_passPreORSelectionLoose) = decoration;
          jetPtr->auxdecor<char>(m_ORToolDecorationLoose) = decoration * 2;
        }
      }
    }
  }

  void TopObjectSelection::applySelectionPreOverlapRemovalTrackJets() {
    ///-- if executeNominal, skip other systematics (and vice-versa) --///
    if (!m_executeNominal) return;

    const xAOD::JetContainer* jets(nullptr);
    top::check(evtStore()->retrieve(jets,
                                    m_config->sgKeyTrackJets()),
               "TopObjectSelection::applySelectionPreOverlapRemovalTrackJets() failed to retrieve track jets");



    for (const xAOD::Jet* jetPtr : *jets) {
      char decoration = m_trackJetSelection->passSelection(*jetPtr);
      jetPtr->auxdecor<char>(m_passPreORSelection) = decoration;
      if (m_doLooseCuts) {
        jetPtr->auxdecor<char>(m_passPreORSelectionLoose) = decoration;
      }

      if (m_config->sgKeyTrackJetsType() == "AntiKtVR30Rmax4Rmin02TrackJets") { // Event cleaning for variable-R track jets
        float pt_baseline = 5e3;
        float radius1 = std::max(0.02, std::min(0.4, 30000. / jetPtr->pt()));

        float dr_jets;
        bool passDRcut = true;
        for (const xAOD::Jet* jet2 : *jets) {
          if (jet2->pt() < pt_baseline) continue;
          if (jetPtr == jet2) continue;

          float radius2 = std::max(0.02, std::min(0.4, 30000. / jet2->pt()));

          dr_jets = xAOD::P4Helpers::deltaR(jetPtr, jet2, false);
          if (dr_jets < std::min(radius1, radius2)) passDRcut = false;
        }
        jetPtr->auxdecor<char>("passDRcut") = passDRcut;
      }

      std::vector<std::string> availableWPs = m_config->bTagWP_available_trkJet();
      for (auto& WP : availableWPs) {
        if (WP.find("Continuous") == std::string::npos) {
          bool isTagged = false;
          if (std::fabs(jetPtr->eta()) < 2.5) {
            ToolHandle<IBTaggingSelectionTool>& btagsel = m_trkjet_btagSelTools[WP];
            isTagged = btagsel->accept(*jetPtr);
          }
          jetPtr->auxdecor<char>("isbtagged_" + WP) = isTagged;
        } else {
          int tagWeightBin = -2; // AT default
          if (std::fabs(jetPtr->eta()) < 2.5) {
            ToolHandle<IBTaggingSelectionTool>& btagsel = m_trkjet_btagSelTools[WP];
            tagWeightBin = btagsel->getQuantile(*jetPtr);
          }
          jetPtr->auxdecor<int>("tagWeightBin_" + WP) = tagWeightBin;
        }
      }
    }
  }

void TopObjectSelection::applySelectionPreOverlapRemovalJetGhostTracks() {
      
    auto jetsystematic = *m_config->systSgKeyMapJets(false);  
    CP::SystematicSet nominal;
    std::size_t m_nominalHashValue = nominal.hash();
    
    for (auto currentSystematic : *m_config->systMapJetGhostTrack()) {
      
    // At this point some specific jet collection doesn't exist for tracking systematic so use the nominal jet collection to retrieve the ghost tracks
      std::unordered_map<std::size_t, std::string>::const_iterator jetsyst_name = jetsystematic.find(currentSystematic.first);
      if (jetsyst_name == jetsystematic.end()) {
         jetsyst_name = jetsystematic.find(m_nominalHashValue); 
      }    
      
      ///-- if executeNominal, skip other systematics (and vice-versa) --///                                                                                                                            
      if (m_executeNominal && !m_config->isSystNominal(m_config->systematicName(currentSystematic.first))) continue;

      if (!m_executeNominal && m_config->isSystNominal(m_config->systematicName(currentSystematic.first))) continue;
    
      
      const xAOD::JetContainer* jets(nullptr);
      top::check(evtStore()->retrieve(jets,
                                      (*jetsyst_name).second),
                 "TopObjectSelection::applySelectionPreOverlapRemovalJetGhostTracks() failed to retrieve jets");
      ATH_MSG_DEBUG(" Cut on JetsGhostTracks with key = " << (*jetsyst_name).second);
      
     
      
      for (auto jetPtr : *jets)
      {
            if(!m_jetGhostTrackSelection->JetConsideredForGhostTrackSelection(jetPtr->pt(),jetPtr->eta())) continue;

            
            std::vector<const xAOD::TrackParticle*> jetTracks;
    
            jetTracks = jetPtr->getAssociatedObjects<xAOD::TrackParticle>(m_config->decoKeyJetGhostTrack(currentSystematic.first));
            
            if (jetTracks.size() == 0){
                ATH_MSG_WARNING("TopObjectSelection::applySelectionPreOverlapRemovalJetGhostTracks() failed to retrieve tracks, jet pT and eta:" << jetPtr->pt() << " " << std::fabs(jetPtr->eta()) );
                continue;
            }

            const xAOD::VertexContainer* vertices = nullptr;
            top::check(evtStore() -> retrieve( vertices, "PrimaryVertices" ), "Failed to get primary vertices");
            const auto it_pv = std::find_if(vertices->cbegin(), vertices->cend(),
                                      [](const xAOD::Vertex* vtx)
                                      {return vtx->vertexType() == xAOD::VxType::PriVtx;});
            const xAOD::Vertex* primaryVertex = (it_pv == vertices->cend()) ? nullptr : *it_pv;
            if (primaryVertex == nullptr) ATH_MSG_WARNING("TopObjectSelection No primary vertex found." );



            for (auto jetTrIt : jetTracks){

                //Decorate the tracks with a flag "passPreORSelection" to indicate if they passed the selection
                jetTrIt->auxdecor<char>(m_passPreORSelection) = m_jetGhostTrackSelection->passSelection(*jetTrIt, *primaryVertex);
                jetTrIt->auxdecor<char>(m_ORToolDecoration)   = jetTrIt->auxdataConst<char>(m_passPreORSelection) * 2;

            }
            
        }
    }
  }


 void TopObjectSelection::applySelectionPreOverlapRemovalTracks() {
    
    for (auto currentSystematic : *m_config->systSgKeyMapTracks()) {

      ///-- if executeNominal, skip other systematics (and vice-versa) --///                                                                                                                            
      if (m_executeNominal && !m_config->isSystNominal(m_config->systematicName(currentSystematic.first))) continue;

      if (!m_executeNominal && m_config->isSystNominal(m_config->systematicName(currentSystematic.first))) continue;

      const xAOD::TrackParticleContainer* tracks(nullptr);
      top::check(evtStore()->retrieve(tracks,
                                      currentSystematic.second),
                 "TopObjectSelection::applySelectionPreOverlapRemovalTracks() failed to retrieve tracks");

      ATH_MSG_DEBUG(" Cut on Tracks with key = " << currentSystematic.second);

      const xAOD::VertexContainer* vertices = nullptr;
      
      top::check(evtStore() -> retrieve( vertices, "PrimaryVertices" ), "Failed to get primary vertices");
      const auto it_pv = std::find_if(vertices->cbegin(), vertices->cend(),
                                      [](const xAOD::Vertex* vtx)
                                      {return vtx->vertexType() == xAOD::VxType::PriVtx;});
      const xAOD::Vertex* primaryVertex = (it_pv == vertices->cend()) ? nullptr : *it_pv;
      if (primaryVertex == nullptr) ATH_MSG_WARNING("TopObjectSelection. No primary vertex found." );


      int counter = 0;

      for (auto trackPtr : *tracks) {

        trackPtr -> auxdecor<char>(m_passPreORSelection) = m_trackSelection->passSelection(*trackPtr, *primaryVertex);
        trackPtr -> auxdecor<char>(m_ORToolDecoration)   = trackPtr->auxdataConst<char>(m_passPreORSelection) * 2;

        counter++;


      }
    }

  }

  
  StatusCode TopObjectSelection::applyOverlapRemoval() {
    bool aLooseEvent(true), aTightEvent(false);

    if (m_config->doTightEvents()) {
      top::check(applyOverlapRemoval(aTightEvent,
                                     m_config->sgKeyTopSystematicEvents()), "Failed to apply overlap removal");
    }
    if (m_config->doLooseEvents()) {
      top::check(applyOverlapRemoval(aLooseEvent,
                                     m_config->sgKeyTopSystematicEventsLoose()), "Failed to apply overlap removal");
    }
    return StatusCode::SUCCESS;
  }

  StatusCode TopObjectSelection::applyOverlapRemoval(const bool isLoose, const std::string& sgKey) {
    // create the xAOD::SystematicEventContainer
    xAOD::SystematicEventAuxContainer* systEventAuxCont = new xAOD::SystematicEventAuxContainer {};
    xAOD::SystematicEventContainer* systEventCont = new xAOD::SystematicEventContainer {};
    systEventCont->setStore(systEventAuxCont);

    // We need to be able to have the nominal systematic processed first
    // but we cannot alter this inside TopConfig as we use unordered_set
    // Using an ordered set will only order on hash and not on insertion
    // so we still cannot control it there
    // Best place to handle it is when the systematic object container is being filled (before it is const)
    // We read all hashes into a vector, and then custom sort to be nominal, then alphabetical
    std::vector<size_t> sortedSystHashAll(m_config->systHashAll()->begin(), m_config->systHashAll()->end());
    std::sort(sortedSystHashAll.begin(), sortedSystHashAll.end(),
              [this](const size_t& lhs, const size_t& rhs) {
      // if lhs is nominal, true - move
      if (this->m_config->isSystNominal(this->m_config->systematicName(lhs))) return true;
      // if rhs is nominal, false - fix
      else if (this->m_config->isSystNominal(this->m_config->systematicName(rhs))) return false;
      // otherwise alphabetical - sort
      else return(this->m_config->systematicName(lhs) < this->m_config->systematicName(rhs));
    });

    for (auto systematicNumber : sortedSystHashAll) {
      ATH_MSG_DEBUG(systematicNumber << " " << m_config->systematicName(systematicNumber));
      ///-- if executeNominal, skip other systematics (and vice-versa) --///
      if (m_executeNominal && !m_config->isSystNominal(m_config->systematicName(systematicNumber))) continue;
      if (!m_executeNominal && m_config->isSystNominal(m_config->systematicName(systematicNumber))) continue;

      if ((!m_config->doTightSysts() && !isLoose) &&
          !m_config->isSystNominal(m_config->systematicName(systematicNumber))) continue;
      if ((!m_config->doLooseSysts() && isLoose) &&
          !m_config->isSystNominal(m_config->systematicName(systematicNumber))) continue;

      xAOD::SystematicEvent* systEvent = new xAOD::SystematicEvent {};
      systEventCont->push_back(systEvent);
      systEvent->setHashValue(systematicNumber);
      systEvent->setIsLooseEvent(isLoose);
      if (!isLoose) {
        systEvent->setTtreeIndex(m_config->ttreeIndex(systematicNumber));
      }
      if (isLoose) {
        systEvent->setTtreeIndex(m_config->ttreeIndexLoose(systematicNumber));
      }
      systEvent->auxdecor<char> (m_config->passEventSelectionDecoration()) = 0;
      top::check(applyOverlapRemoval(systEvent), "Failed to apply overlap removal");
    }

    if (m_executeNominal) {
      // Create a new StoreGate key
      std::string sgKeyNominal = sgKey + "Nominal";

      // Save to StoreGate / TStore
      std::string outputSGKeyNominalAux = sgKeyNominal + "Aux.";

      xAOD::TReturnCode save = evtStore()->tds()->record(systEventCont, sgKeyNominal);
      xAOD::TReturnCode saveAux = evtStore()->tds()->record(systEventAuxCont, outputSGKeyNominalAux);
      if (!save || !saveAux) {
        return StatusCode::FAILURE;
      }
      return StatusCode::SUCCESS;
    } else {
      // Retrieve nominal container, get nominal event, copy into full systematic container
      xAOD::SystematicEventContainer* systEventContNominal = new xAOD::SystematicEventContainer {};
      top::check(evtStore()->retrieve(systEventContNominal, sgKey + "Nominal"), "Failed to retrieve nominal container");
      for (auto x: *systEventContNominal) {
        xAOD::SystematicEvent* systEventNominal = new xAOD::SystematicEvent {};
        systEventCont->push_back(systEventNominal);
        *systEventNominal = *x;
      }

      // Save to StoreGate / TStore
      std::string outputSGKeyAux = sgKey + "Aux.";

      xAOD::TReturnCode save = evtStore()->tds()->record(systEventCont, sgKey);
      xAOD::TReturnCode saveAux = evtStore()->tds()->record(systEventAuxCont, outputSGKeyAux);
      if (!save || !saveAux) {
        return StatusCode::FAILURE;
      }
      return StatusCode::SUCCESS;
    }
  }

  StatusCode TopObjectSelection::applyOverlapRemoval(xAOD::SystematicEvent* currentSystematic) {
    // Which lepton definition are we using for the overlap removal?
    // Default for top analysis is "Tight"
    //
    // Use "Loose" for Fakes estimates
    //
    // Other physics groups choose to do overlap removal on "Loose" lepton definitions
    // As such, this mode is fully supported, simply set:
    //    OverlapRemovalLeptonDef Loose
    // In your configuration file

    bool looseLeptonOR(false); // default behaviour for top analysis - use the "Tight" definitions

    if (currentSystematic->isLooseEvent()) {
      looseLeptonOR = true;
    }
    if (m_config->doOverlapRemovalOnLooseLeptonDef()) {
      looseLeptonOR = true;
    }

    std::size_t hash = currentSystematic->hashValue();
    

    // Retrieve the relevant shallow copies
    const xAOD::PhotonContainer* xaod_photon(nullptr);
    if (m_config->usePhotons()) top::check(evtStore()->retrieve(xaod_photon, m_config->sgKeyPhotons(
                                                                  hash)),
                                           "TopObjectSelection::applyOverlapRemovalPostSelection() failed to retrieve photons");


    const xAOD::ElectronContainer* xaod_el(nullptr);
    if (m_config->useElectrons()) top::check(evtStore()->retrieve(xaod_el, m_config->sgKeyElectrons(
                                                                    hash)),
                                             "TopObjectSelection::applyOverlapRemovalPostSelection() failed to retrieve electrons");


    const xAOD::ElectronContainer* xaod_fwdel(nullptr);
    if (m_config->useFwdElectrons()) top::check(evtStore()->retrieve(xaod_fwdel, m_config->sgKeyFwdElectrons(
                                                                       hash)),
                                                "TopObjectSelection::applyOverlapRemovalPostSelection() failed to retrieve fwd electrons");


    const xAOD::MuonContainer* xaod_mu(nullptr);
    if (m_config->useMuons()) top::check(evtStore()->retrieve(xaod_mu, m_config->sgKeyMuons(
                                                                hash)),
                                         "TopObjectSelection::applyOverlapRemovalPostSelection() failed to retrieve muons");


    const xAOD::MuonContainer* xaod_softmu(nullptr);
    if (m_config->useSoftMuons()) top::check(evtStore()->retrieve(xaod_softmu, m_config->sgKeySoftMuons(
                                                                    hash)),
                                             "TopObjectSelection::applyOverlapRemovalPostSelection() failed to retrieve soft muons");


    const xAOD::TauJetContainer* xaod_tau(nullptr);
    if (m_config->useTaus()) top::check(evtStore()->retrieve(xaod_tau, m_config->sgKeyTaus(
                                                               hash)),
                                        "TopObjectSelection::applyOverlapRemovalPostSelection() failed to retrieve taus");


    const xAOD::JetContainer* xaod_jet(nullptr);
    if (m_config->useJets()) top::check(evtStore()->retrieve(xaod_jet, m_config->sgKeyJets(hash,
                                                                                           looseLeptonOR)),
                                        "TopObjectSelection::applyOverlapRemovalPostSelection() failed to retrieve jets");


    const xAOD::JetContainer* xaod_ljet(nullptr);
    if (m_config->useLargeRJets()) top::check(evtStore()->retrieve(xaod_ljet, m_config->sgKeyLargeRJets(
                                                                     hash)),
                                              "TopObjectSelection::applyOverlapRemovalPostSelection() failed to retrieve large-R jets");


    const xAOD::JetContainer* xaod_tjet(nullptr);
    if (m_config->useTrackJets()) top::check(evtStore()->retrieve(xaod_tjet,
                                                                  m_config->sgKeyTrackJets()),
                                             "TopObjectSelection::applyOverlapRemovalPostSelection() failed to retrieve track jets");

    const xAOD::TrackParticleContainer* xaod_tracks(nullptr);
    if (m_config->useTracks()) top::check(evtStore()->retrieve(xaod_tracks,
							       m_config->sgKeyTracks(hash)),
					  "TopObjectSelection::applyOverlapRemovalPostSelection() failed to retrieve tracks");


    // vectors to store the indices of objects passing selection and overlap removal
    std::vector<unsigned int> goodPhotons, goodElectrons, goodFwdElectrons, goodMuons, goodSoftMuons, goodTaus,
                              goodJets, goodLargeRJets, goodTrackJets, goodTracks;

    // Apply overlap removal
    m_overlapRemovalToolPostSelection->overlapremoval(xaod_photon, xaod_el, xaod_mu, xaod_tau,
                                                      xaod_jet, xaod_ljet,
                                                      goodPhotons, goodElectrons, goodMuons, goodTaus,
                                                      goodJets, goodLargeRJets, looseLeptonOR);

    // Additonal lepton information
    std::vector<unsigned int> overlapsEl, overlapsMu;
    if (m_overlapRemovalToolPostSelection->overlapsEl(overlapsEl)) {
      currentSystematic->auxdecor< std::vector<unsigned int> >("overlapsEl") = overlapsEl;
    }
    if (m_overlapRemovalToolPostSelection->overlapsMu(overlapsMu)) {
      currentSystematic->auxdecor< std::vector<unsigned int> >("overlapsMu") = overlapsMu;
    }

    // If we did use overlap removal on "Loose" lepton definitions
    // We take the remaining leptons and only keep those passing the "Tight" cuts
    if (!currentSystematic->isLooseEvent() && m_config->doOverlapRemovalOnLooseLeptonDef()) {
      applyTightSelectionPostOverlapRemoval(xaod_photon, goodPhotons);
      applyTightSelectionPostOverlapRemoval(xaod_el, goodElectrons);
      applyTightSelectionPostOverlapRemoval(xaod_mu, goodMuons);
      applyTightSelectionPostOverlapRemoval(xaod_tau, goodTaus);
      applyTightSelectionPostOverlapRemoval(xaod_jet, goodJets);
      applyTightSelectionPostOverlapRemoval(xaod_ljet, goodLargeRJets);
    }

    if (m_config->useTrackJets()) {
      for (unsigned int i = 0; i < xaod_tjet->size(); ++i) goodTrackJets.push_back(i);
      trackJetOverlapRemoval(xaod_el, xaod_mu, xaod_tjet, goodElectrons, goodMuons, goodTrackJets);
      applyTightSelectionPostOverlapRemoval(xaod_tjet, goodTrackJets);
    }

    // for the time being no OR applied on FwdElectrons
    if (xaod_fwdel) {
      int i(0);
      std::string passTopCuts("");
      if (!looseLeptonOR) {
        passTopCuts = "passPreORSelection";
      }
      if (looseLeptonOR) {
        passTopCuts = "passPreORSelectionLoose";
      }
      for (const xAOD::Electron* x: *xaod_fwdel) {
        if (x->auxdataConst< char >(passTopCuts) == 1) goodFwdElectrons.push_back(i);

        i++;
      }
    }

    // no OR applied on tracks                                                                                                                                                                          
    if (xaod_tracks) {
      int i(0);
      std::string passTopCuts = "passPreORSelection";

      for (const xAOD::TrackParticle* x: *xaod_tracks) {

        if (x->auxdataConst< char >(m_passPreORSelection) == 1) goodTracks.push_back(i);

        i++;
      }
    }


    // for the time being the only OR performed on soft muons is wrt prompt muons
    if (xaod_softmu) {
      
      if(m_config->useJets())
      {
        top::check(m_overlapRemovalTool_softMuons_Alljets->removeOverlaps(nullptr, xaod_softmu, xaod_jet, nullptr, nullptr), "Failed to identify overlap for soft muons - all jets");
        if(m_config->useParticleFlowJets())top::check(m_overlapRemovalTool_softMuons_PFjets->removeOverlaps(nullptr, xaod_softmu, xaod_jet, nullptr, nullptr), "Failed to identify overlap for soft muons - PFlow jets");
      }
      
      int i(0);
      std::string passTopCuts = "passPreORSelection";

      for (const xAOD::Muon* x: *xaod_softmu) {
        //OR with prompt muons
        bool promptMuOR = false;
        if (xaod_mu) {
          for (unsigned int iMu : goodMuons) {
            // Get muon iMu
            const xAOD::Muon* muPtr = xaod_mu->at(iMu);
            if (muPtr->p4().DeltaR(x->p4()) < 0.01) {
              promptMuOR = true;
              break;
            }
          }
        }

        float dRmin = 100; //nearest jet dR
        if(m_config->useJets()) dRmin=this->calculateMinDRMuonJet(*x, xaod_jet, goodJets, m_config->softmuonDRJetcutUseRapidity());

        if (x->auxdataConst< char >(passTopCuts) == 1 && !promptMuOR && (!m_config->useJets() || dRmin < m_config->softmuonDRJetcut()))
        {
           goodSoftMuons.push_back(i);//the dR selection must be done here, because we need the post-OR jets...
        }
        i++;
      }
    }//end of OR procedure for soft muons

    if(m_config->isMC() && m_config->useSoftMuons() && m_config->softmuonAdditionalTruthInfo()) decorateSoftMuonsPostOverlapRemoval(xaod_softmu,goodSoftMuons);


      //Change the collection of ghost tracks associated to jets
    //Store the selected ghost associated tracks
    //It doesn't work for the nominal systematic 
    if (m_config->useJetGhostTrack() && m_config->useJets()) {
        
        std::vector<const xAOD::TrackParticle*> jetTracks;
        
        xAOD::JetContainer* xaod_jet_ga(nullptr);
        top::check(evtStore()->retrieve(xaod_jet_ga, m_config->sgKeyJets(hash,looseLeptonOR)),
                                        "TopObjectSelection::applyOverlapRemovalPostSelection() failed to retrieve jets for ghost matching");
        
        unsigned int index= -1;
        
        for (const auto jetPtr : *xaod_jet_ga){
            
            index=index+1;
            
            if(!m_jetGhostTrackSelection->JetConsideredForGhostTrackSelection(jetPtr->pt(),jetPtr->eta()))
                continue;
            
            if (std::find(goodJets.begin(), goodJets.end(), index) == goodJets.end()){ 
                continue;
            }
            
            jetTracks.clear();
            
            std::vector<const xAOD::TrackParticle*> goodJetGhostTracks;
            jetTracks = jetPtr->getAssociatedObjects<xAOD::TrackParticle>(m_config->decoKeyJetGhostTrack(hash));
            
            if (jetTracks.size() != 0) {
                
                std::string passTopCuts = "passPreORSelection";

                for (auto& track: jetTracks) {

                    if (track->auxdataConst< char >(passTopCuts) == 1) {
                        goodJetGhostTracks.push_back(track);
                    }
        
                }
 
                jetPtr->setAssociatedObjects<xAOD::TrackParticle>(m_config->decoKeyJetGhostTrack(hash), goodJetGhostTracks);
                
            }
        }
        
    }
    
    // set the indices in the xAOD::SystematicEvent
    currentSystematic->setGoodPhotons(goodPhotons);
    currentSystematic->setGoodElectrons(goodElectrons);
    currentSystematic->setGoodFwdElectrons(goodFwdElectrons);
    currentSystematic->setGoodMuons(goodMuons);
    currentSystematic->setGoodSoftMuons(goodSoftMuons);
    currentSystematic->setGoodTaus(goodTaus);
    currentSystematic->setGoodJets(goodJets);
    currentSystematic->setGoodLargeRJets(goodLargeRJets);
    currentSystematic->setGoodTrackJets(goodTrackJets);
    currentSystematic->setGoodTracks(goodTracks);

    decorateEventInfoPostOverlapRemoval(goodJets.size(), currentSystematic->isLooseEvent());
    
    
    
    return StatusCode::SUCCESS;
  }
  
  void TopObjectSelection::decorateSoftMuonsPostOverlapRemoval(const xAOD::MuonContainer* xaod_softmu,std::vector<unsigned int>& goodSoftMuons)
  {

    for (auto iMu : goodSoftMuons) {
      // Get muon iMu
      const xAOD::Muon* muon = xaod_softmu->at(iMu);
      
      top::truth::initRecoMuonHistoryInfo(muon,m_config->softmuonAdditionalTruthInfoCheckPartonOrigin()); //it's safer if we initialize everything to default for each muon before filling the muon history        
      top::truth::getRecoMuonHistory(muon,m_config->softmuonAdditionalTruthInfoCheckPartonOrigin(),m_config->getShoweringAlgorithm(),m_config->softmuonAdditionalTruthInfoDoVerbose());
    }//end of loop on soft muons
  }
  void TopObjectSelection::applyTightSelectionPostOverlapRemoval(const xAOD::IParticleContainer* xaod,
                                                                 std::vector<unsigned int>& indices) {
    // Copy the original indices of the xAOD objects in
    // the collection that pass the overlap removal
    std::vector<unsigned int> tmpCopy;
    for (auto i : indices) {
      tmpCopy.push_back(i);
    }

    // Clear the original indices
    indices.clear();

    // Only save indices that are in tmpCopy && passPreORSelection
    // This will keep the objects that pass the "Loose" overlap selection
    // and pass the "Tight" cuts
    for (auto i : tmpCopy) {
      if (xaod->at(i)->isAvailable< char> (m_passPreORSelection)) {
        if (xaod->at(i)->auxdataConst< char >(m_passPreORSelection) == 1) {
          indices.push_back(i);
        }
      }
    }
  }

  void TopObjectSelection::trackJetOverlapRemoval(const xAOD::IParticleContainer* xaod_el,
                                                  const xAOD::IParticleContainer* xaod_mu,
                                                  const xAOD::IParticleContainer* xaod_tjet,
                                                  std::vector<unsigned int>& goodElectrons,
                                                  std::vector<unsigned int>& goodMuons,
                                                  std::vector<unsigned int>& goodTrackJets) {
    // Copy the original indices of the xAOD objects in
    // the collection that pass the overlap removal
    std::vector<unsigned int> tmpGoodTrackJets;
    for (auto i : goodTrackJets) {
      tmpGoodTrackJets.push_back(i);
    }

    // Clear the original indices
    goodTrackJets.clear();

    int counterTrk = 0;
    for (auto jet : *xaod_tjet) {
      bool matchEl(false);
      int counterEl = 0;
      for (auto el : *xaod_el) {
        if ((std::find(goodElectrons.begin(), goodElectrons.end(), counterEl) != goodElectrons.end())) {
          if (el->p4().DeltaR(jet->p4()) < 0.2) {
            matchEl = true;
            break;
          }
        }
        counterEl++;
      }

      bool matchMu(false);
      int counterMu = 0;
      for (auto mu : *xaod_mu) {
        if ((std::find(goodMuons.begin(), goodMuons.end(), counterMu) != goodMuons.end())) {
          if (mu->p4().DeltaR(jet->p4()) < 0.2) {
            matchMu = true;
            break;
          }
        }
        counterMu++;
      }

      if (!(matchEl || matchMu)) goodTrackJets.push_back(counterTrk);
      counterTrk++;
    }
  }

  float TopObjectSelection::calculateMinDRMuonJet(const xAOD::Muon& mu, const xAOD::JetContainer* xaod_jet, std::vector<unsigned int>& goodJets, bool useRapidity) {
    
    float dRMin = 100.0;
    
    // Loop over jets, calculate dR and record smallest value
    for (auto iJet : goodJets) {
      const xAOD::Jet* jetPtr = xaod_jet->at(iJet);
      if(jetPtr->isAvailable<char>("passJVT") && !(jetPtr->auxdataConst<char>("passJVT") )) continue; //at this level we still have jets not passing the JVT cut in the ntuple
      float dR = xAOD::P4Helpers::deltaR(mu,*jetPtr,useRapidity);
      if (dR < dRMin) 
      {
        dRMin = dR;
      }
    }
    
    return dRMin;
  }

  void TopObjectSelection::decorateEventInfoPostOverlapRemoval(int nGoodJets, bool isLoose) {
    ///-- Decorate event info with number of nominal jets for flavour composition
    // Only continue if this is nominal processing
    if (!m_executeNominal) return;

    bool UseLooseNJets = false;
    // If we only run on loose nominal, we need to use loose event, otherwise use tight
    if (m_config->doLooseEvents() && !m_config->doTightEvents()) UseLooseNJets = true;
    // If we use loose, but this is tight, skip
    if (UseLooseNJets && !isLoose) return;

    // Get the EventInfo object
    const xAOD::EventInfo* eventInfo(nullptr);
    top::check(evtStore()->retrieve(eventInfo, m_config->sgKeyEventInfo()), "Failed to retrieve EventInfo");
    // Decorate with number of good jets
    eventInfo->auxdecor< int >("Njet") = nGoodJets;

    return;
  }

  void TopObjectSelection::print() const {
    asg::AsgTool::print();
  }

  void TopObjectSelection::print(std::ostream& os) const {
    os << "TopObjectSelection configuration\n";

    os << "\n";
    os << "Electrons\n";
    os << "  ContainerName: " << m_config->sgKeyElectrons() << "\n";
    if (m_config->useElectrons()) {
      os << "  Selection: ";
      if (!m_electronSelection) os << "All";
      else os << *m_electronSelection;
    }

    os << "\n";
    os << "FwdElectrons\n";
    os << "  ContainerName: " << m_config->sgKeyFwdElectrons() << "\n";
    if (m_config->useFwdElectrons()) {
      os << "  Selection: ";
      if (!m_fwdElectronSelection) os << "All";
      else os << *m_fwdElectronSelection;
    }

    os << "\n";
    os << "Photons\n";
    os << "  ContainerName: " << m_config->sgKeyPhotons() << "\n";
    if (m_config->usePhotons()) {
      os << "  Selection: ";
      if (!m_photonSelection) os << "All";
      else os << *m_photonSelection;
    }

    os << "\n";
    os << "Muons\n";
    os << "  ContainerName: " << m_config->sgKeyMuons() << "\n";
    if (m_config->useMuons()) {
      os << "  Selection: ";
      if (!m_muonSelection) os << "All";
      else os << *m_muonSelection;
    }

    os << "\n";
    os << "SoftMuons\n";
    os << "  ContainerName: " << m_config->sgKeySoftMuons() << "\n";
    if (m_config->useSoftMuons()) {
      os << "  Selection: ";
      if (!m_softmuonSelection) os << "All";
      else os << *m_softmuonSelection;
    }
//
//    os << "\n";
//    os << "Taus\n";
//    os << "  ContainerName: " << containerNames().tauCollectionName << "\n";
//    if (containerNames().tauCollectionName != "None") {
//        os << "  Selection: ";
//        if (!m_tauSelection)
//            os << "All";
//        else
//            os << *m_tauSelection;
//    }

    os << "\n";
    os << "Jets\n";
    os << "  ContainerName: " << m_config->sgKeyJets() << "\n";
    if (m_config->useJets()) {
      os << "  Selection: ";
      if (!m_jetSelection) os << "All";
      else os << *m_jetSelection;
    }

    os << "\n";
    os << "LargeJets\n";
    os << "  ContainerName: " << m_config->sgKeyLargeRJets() << "\n";
    if (m_config->useLargeRJets()) {
      os << "  Selection: ";
      if (!m_largeJetSelection) os << "All";
      else os << *m_largeJetSelection;
    }
    
    os << "\n";
    os << "GhostTracks\n";
    os << "  ContainerName: " << m_config->decoKeyJetGhostTrack() << "\n";
    if (m_config->useJetGhostTrack()) {
      os << "  Selection: ";
      if (!m_jetGhostTrackSelection) os << "All";
//       else m_jetGhostTrackSelection->print(os);
      else os << *m_jetGhostTrackSelection;
    }
    

    os << "\n";
    os << "MET\n";
    os << "  ContainerName: " << m_config->sgKeyMissingEt() << "\n";

    os << "\n\n";
    os << "OverlapRemoval after object selection: ";
    if (!m_overlapRemovalToolPostSelection) os << "None";
    else os << *m_overlapRemovalToolPostSelection;

    os << "\n\n";
  }
}
