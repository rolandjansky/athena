/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

#include "TopAnalysis/ObjectLoaderStandardCuts.h"
#include "TopEvent/EventTools.h"
#include "TopConfiguration/TopConfig.h"

#include "TopObjectSelectionTools/TopObjectSelection.h"
#include "TopObjectSelectionTools/ElectronLikelihoodMC15.h"
#include "TopObjectSelectionTools/ElectronCutBasedMC15.h"
#include "TopObjectSelectionTools/FwdElectronMC15.h"
#include "TopObjectSelectionTools/IsolationTools.h"
#include "TopObjectSelectionTools/MuonMC15.h"
#include "TopObjectSelectionTools/SoftMuonMC15.h"
#include "TopObjectSelectionTools/AntiMuonMC15.h"
#include "TopObjectSelectionTools/TauMC15.h"
#include "TopObjectSelectionTools/JetMC15.h"
#include "TopObjectSelectionTools/TrackJetMC15.h"
#include "TopObjectSelectionTools/JetGhostTrackSelection.h"
#include "TopObjectSelectionTools/TrackSelection.h"
#include "TopObjectSelectionTools/OverlapRemovalASG.h"
// R21 specific
#include "TopObjectSelectionTools/PhotonMC16.h"

#include "TopAnalysis/MsgCategory.h"
// use ATH_MSG macros defined in the namespace TopAnalysis
using namespace TopAnalysis;

namespace top {
  top::TopObjectSelection* ObjectLoaderStandardCuts::init(std::shared_ptr<top::TopConfig> topConfig) {
    top::TopObjectSelection* objectSelection = new top::TopObjectSelection(topConfig->objectSelectionName());
    top::check(objectSelection->setProperty("config", topConfig), "Failed to setProperty for top::TopObjectSelection");
    top::check(objectSelection->initialize(), "Failed to initialize top::TopObjectSelection");

    // Debug messages?
    // objectSelection->msg().setLevel(MSG::DEBUG);

    ///-- Photons --//
    if (topConfig->usePhotons()) {
      ATH_MSG_INFO("top::ObjectLoaderStandardCuts::init - Using new photon object for Release 21 - PhotonMC16");
      objectSelection->photonSelection(new top::PhotonMC16(topConfig->photonPtcut(),
                                                           topConfig->photonEtacut(),
                                                           topConfig->photonIdentification(),
                                                           topConfig->photonIdentificationLoose(),
                                                           new top::StandardIsolation(topConfig->photonIsolation(),
                                                                                      topConfig->photonIsolationLoose()),
                                                           topConfig->recomputeCPvars()));
    }

    ///-- Electrons --///
    if (topConfig->useElectrons()) {
      if (topConfig->electronID().find("LH") == std::string::npos &&
          topConfig->electronIDLoose().find("LH") == std::string::npos) {
        //both the tight and loose user settings do not contain LH -> cut based
        objectSelection->electronSelection(new top::ElectronCutBasedMC15(topConfig->electronPtcut(),
                                                                         topConfig->electronVetoLArCrack(),
                                                                         topConfig->electronID(),
                                                                         topConfig->electronIDLoose(),
                                                                         new top::StandardIsolation(
                                                                           topConfig->electronIsolation(),
                                                                           topConfig->electronIsolationLoose())));
      } else if (topConfig->electronID().find("LH") != std::string::npos &&
                 topConfig->electronIDLoose().find("LH") != std::string::npos) {
        //user wants likelihood electrons
        objectSelection->electronSelection(new top::ElectronLikelihoodMC15(topConfig->isPrimaryxAOD(),
                                                                           topConfig->electronPtcut(),
                                                                           topConfig->electronVetoLArCrack(),
                                                                           topConfig->electronID(),
                                                                           topConfig->electronIDLoose(),
                                                                           new top::StandardIsolation(
                                                                             topConfig->electronIsolation(),
                                                                             topConfig->electronIsolationLoose()),
                                                                           topConfig->applyTTVACut(),
                                                                           topConfig->useElectronChargeIDSelection()
                                                                           ));
      } else {
        ATH_MSG_ERROR("Not sure it makes sense to use a mix of LH and cut-based electrons for the tight/loose definitions\n"
          << "Tight electron definition is " << topConfig->electronID() << "\n"
          << "Loose electron definition is " << topConfig->electronIDLoose() << "\n"
          << "If it does make sense, feel free to fix this");
        throw std::runtime_error("Mixing LH and cut-based electron definitions for tight/loose");
      }
    }

    ///-- Fwd Electrons --///
    if (topConfig->useFwdElectrons()) {
      objectSelection->fwdElectronSelection(new top::FwdElectronMC15(topConfig->fwdElectronPtcut(),
                                                                     topConfig->fwdElectronMinEtacut(),
                                                                     topConfig->fwdElectronMaxEtacut(), topConfig));
    }
    ///-- Muons --///
    if (topConfig->useMuons()) {
      if (topConfig->useAntiMuons()) objectSelection->muonSelection(new top::AntiMuonMC15(topConfig->muonPtcut(),
                                                                                          new top::StandardIsolation(
                                                                                            topConfig->muonIsolation(),
                                                                                            topConfig->
        muonIsolationLoose())));
      else objectSelection->muonSelection(new top::MuonMC15(topConfig->muonPtcut(),
                                                            new top::StandardIsolation(topConfig->muonIsolation(),
                                                                                       topConfig->muonIsolationLoose()),
                                                            topConfig->applyTTVACut()));
    }

    ///-- Soft Muons --///
    if (topConfig->useSoftMuons()) {
      objectSelection->softmuonSelection(new top::SoftMuonMC15(topConfig->softmuonPtcut()));
    }


    ///-- Taus --///
    if (topConfig->useTaus()) {
      objectSelection->tauSelection(new top::TauMC15());
    }

    ///-- Jets --///
    if (topConfig->useJets()) {
      objectSelection->jetSelection(new top::JetMC15(topConfig->jetPtcut(), topConfig->jetEtacut()));
    }

    ///-- Large R Jets --///
    if (topConfig->useLargeRJets()) {// not doing JVT cut for large-R jets
      objectSelection->largeJetSelection(new top::JetMC15(topConfig->largeRJetPtcut(), topConfig->largeRJetEtacut(),
                                                          false));
    }

    ///-- Track Jets --///
    if (topConfig->useTrackJets()) {
      objectSelection->trackJetSelection(new top::TrackJetMC15(topConfig->trackJetPtcut(),
                                                               topConfig->trackJetEtacut()));
    }

    ///-- Ghost Track Jets --///
    if (topConfig->useJetGhostTrack()) {
      objectSelection->jetGhostTrackSelection(new top::JetGhostTrackSelection(topConfig->ghostTrackspT(),
                         2.5,topConfig->ghostTracksVertexAssociation(),topConfig->jetPtGhostTracks(), 2.5));
    }
    
    ///-- Tracks --///                                                                                                                                                                              
    if (topConfig->useTracks()) {
      objectSelection->trackSelection(new top::TrackSelection(topConfig->trackPtcut(), topConfig->trackEtacut()));
    }

    ///-- Overlap removal --///
    /// single parameter: boolean to do OR with large-R jets
    if (!topConfig->isTruthDxAOD()) {
      objectSelection->overlapRemovalPostSelection(new top::OverlapRemovalASG((topConfig->doLargeJetOverlapRemoval() &&
                                                                               topConfig->useLargeRJets())));
    }

    return objectSelection;
  }
}
