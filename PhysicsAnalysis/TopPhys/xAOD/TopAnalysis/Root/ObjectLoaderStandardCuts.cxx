/*
   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 */

#include "TopAnalysis/ObjectLoaderStandardCuts.h"
#include "TopEvent/EventTools.h"
#include "TopConfiguration/TopConfig.h"

#include "TopObjectSelectionTools/TopObjectSelection.h"
#include "TopObjectSelectionTools/ElectronLikelihood.h"
//#include "TopObjectSelectionTools/FwdElectron.h"
#include "TopObjectSelectionTools/IsolationTools.h"
#include "TopObjectSelectionTools/Muon.h"
#include "TopObjectSelectionTools/SoftMuon.h"
#include "TopObjectSelectionTools/AntiMuon.h"
#include "TopObjectSelectionTools/Tau.h"
#include "TopObjectSelectionTools/Jet.h"
#include "TopObjectSelectionTools/TrackJet.h"
#include "TopObjectSelectionTools/JetGhostTrackSelection.h"
#include "TopObjectSelectionTools/TrackSelection.h"
#include "TopObjectSelectionTools/OverlapRemovalASG.h"
// R21 specific
#include "TopObjectSelectionTools/Photon.h"

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
      ATH_MSG_INFO("top::ObjectLoaderStandardCuts::init - Using new photon object for Release 21 - Photon");
      objectSelection->photonSelection(new top::Photon(topConfig->photonPtcut(),
                                                       topConfig->photonEtacut(),
                                                       topConfig->photonIdentification(),
                                                       topConfig->photonIdentificationLoose(),
                                                       new top::StandardIsolation(topConfig->photonIsolation(),
                                                                                  topConfig->photonIsolationLoose())));
    }

    ///-- Electrons --///
    if (topConfig->useElectrons()) {
      if (topConfig->electronID().find("LH") != std::string::npos &&
          topConfig->electronIDLoose().find("LH") != std::string::npos) {
        //user wants likelihood electrons
        objectSelection->electronSelection(new top::ElectronLikelihood(topConfig->electronPtcut(),
                                                                       topConfig->electronVetoLArCrack(),
                                                                       topConfig->electronID(),
                                                                       topConfig->electronIDLoose(),
                                                                       new top::StandardIsolation(
                                                                         topConfig->electronIsolation(),
                                                                         topConfig->electronIsolationLoose()),
                                                                       topConfig->electrond0Sigcut(),
                                                                       topConfig->electrondeltaz0cut(),
                                                                       topConfig->applyTTVACut(),
                                                                       topConfig->useElectronChargeIDSelection()
                                                                       ));
      } else {
        ATH_MSG_ERROR("Only likelihood-based electron ID is currently supported. You have selected:\n"
          << "Tight electron definition: " << topConfig->electronID() << "\n"
          << "Loose electron definition: " << topConfig->electronIDLoose());
        throw std::runtime_error("Unsupported electron ID option");
      }
    }

    ///-- Fwd Electrons --///
    //if (topConfig->useFwdElectrons()) {
    //  objectSelection->fwdElectronSelection(new top::FwdElectron(topConfig->fwdElectronPtcut(),
    //                                                                 topConfig->fwdElectronMinEtacut(),
    //                                                                 topConfig->fwdElectronMaxEtacut(), topConfig));
    //}
    ///-- Muons --///
    if (topConfig->useMuons()) {
      if (topConfig->useAntiMuons()) objectSelection->muonSelection(new top::AntiMuon(topConfig->muonPtcut(),
                                                                                      new top::AntiMuonIsolation(
                                                                                        topConfig->muonIsolation())));
      else objectSelection->muonSelection(new top::Muon(topConfig->muonPtcut(),
                                                        new top::StandardIsolation(topConfig->muonIsolation(),
                                                                                   topConfig->muonIsolationLoose()),
                                                        topConfig->muond0Sigcut(),
                                                        topConfig->muondeltaz0cut(),
                                                        topConfig->applyTTVACut()));
    }

    ///-- Soft Muons --///
    if (topConfig->useSoftMuons()) {
      objectSelection->softmuonSelection(new top::SoftMuon(topConfig->softmuonPtcut()));
    }


    ///-- Taus --///
    if (topConfig->useTaus()) {
      objectSelection->tauSelection(new top::Tau());
    }

    ///-- Jets --///
    if (topConfig->useJets()) {
      objectSelection->jetSelection(new top::Jet(topConfig->jetPtcut(), topConfig->jetEtacut()));
    }

    ///-- Large R Jets --///
    if (topConfig->useLargeRJets()) {// not doing JVT cut for large-R jets
      objectSelection->largeJetSelection(new top::Jet(topConfig->largeRJetPtcut(),
                                                          topConfig->largeRJetEtacut(),
                                                          topConfig->largeRJetMasscut(),
                                                          false));
    }

    ///-- Track Jets --///
    if (topConfig->useTrackJets()) {
      objectSelection->trackJetSelection(new top::TrackJet(topConfig->trackJetPtcut(),
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
