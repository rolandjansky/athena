/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TopAnalysis/ObjectLoaderStandardCuts.h"
#include "TopEvent/EventTools.h"
#include "TopConfiguration/TopConfig.h"

#include "TopObjectSelectionTools/TopObjectSelection.h"
#include "TopObjectSelectionTools/ElectronLikelihoodMC15.h"
#include "TopObjectSelectionTools/ElectronCutBasedMC15.h"
#include "TopObjectSelectionTools/IsolationTools.h"
#include "TopObjectSelectionTools/MuonMC15.h"
#include "TopObjectSelectionTools/AntiMuonMC15.h"
#include "TopObjectSelectionTools/TauMC15.h"
#include "TopObjectSelectionTools/JetMC15.h"
#include "TopObjectSelectionTools/TrackJetMC15.h"
#include "TopObjectSelectionTools/PhotonMC15.h"
#include "TopObjectSelectionTools/OverlapRemovalASG.h"
// R21 specific
#include "TopObjectSelectionTools/PhotonMC16.h"

namespace top {

  top::TopObjectSelection* ObjectLoaderStandardCuts::init(std::shared_ptr<top::TopConfig> topConfig)
  {
    top::TopObjectSelection* objectSelection = new top::TopObjectSelection(topConfig->objectSelectionName());
    top::check(objectSelection->setProperty( "config" , topConfig ) , "Failed to setProperty for top::TopObjectSelection" );
    top::check(objectSelection->initialize() , "Failed to initialize top::TopObjectSelection" );

    // Debug messages?
    // objectSelection->msg().setLevel(MSG::DEBUG);

    ///-- Photons --//
    if(topConfig->usePhotons()) {
      if(topConfig->getReleaseSeries() == 25){
	std::cout << "top::ObjectLoaderStandardCuts::init - Using new photon object for Release 21 - PhotonMC16" << std::endl;
	objectSelection->photonSelection(new top::PhotonMC16(topConfig->photonPtcut(),
							     topConfig->photonEtacut(),
							     topConfig->photonIdentification(),
							     topConfig->photonIdentificationLoose(),
							     new top::StandardIsolation(
							      topConfig->photonIsolation(),
							      topConfig->photonIsolationLoose() ),
							     topConfig->recomputeCPvars()) );
      }
      else{
	objectSelection->photonSelection(new top::PhotonMC15(topConfig->photonPtcut(),
							     topConfig->photonEtacut(),
							     topConfig->photonIdentification(),
							     topConfig->photonIdentificationLoose(),
							     new top::StandardIsolation(
							      topConfig->photonIsolation(),
							      topConfig->photonIsolationLoose() )) );
      }
    }

    ///-- Electrons --///
    if (topConfig->useElectrons()) {
      if (topConfig->electronID().find("LH") == std::string::npos && topConfig->electronIDLoose().find("LH") == std::string::npos) {
          //both the tight and loose user settings do not contain LH -> cut based
        objectSelection->electronSelection(new top::ElectronCutBasedMC15(topConfig->electronPtcut(),
                                                                         topConfig->electronVetoLArCrack(),
                                                                         topConfig->electronID(),
                                                                         topConfig->electronIDLoose(),
                                                                         new top::StandardIsolation(
                                                                           topConfig->electronIsolation() ,
                                                                           topConfig->electronIsolationLoose() )) );

      } else if (topConfig->electronID().find("LH") != std::string::npos && topConfig->electronIDLoose().find("LH") != std::string::npos) {
          //user wants likelihood electrons
          objectSelection->electronSelection(new top::ElectronLikelihoodMC15(topConfig->isPrimaryxAOD(),
                                                                             topConfig->electronPtcut(),
                                                                             topConfig->electronVetoLArCrack(),
                                                                             topConfig->electronID(),
                                                                             topConfig->electronIDLoose(),
                                                                             new top::StandardIsolation(
                                                                               topConfig->electronIsolation() ,
                                                                               topConfig->electronIsolationLoose() ),
									     topConfig->applyTTVACut(),
									     topConfig->useElectronChargeIDSelection()
                                                                            ) );

      } else {
          std::cout << "\nHo hum\n";
          std::cout << "Not sure it makes sense to use a mix of LH and cut-based electrons for the tight/loose definitions\n";
          std::cout << "Tight electron definition is " << topConfig->electronID() << "\n";
          std::cout << "Loose electron definition is " << topConfig->electronIDLoose() << "\n";
          std::cout << "If it does make sense, feel free to fix this\n";
          exit(1);
      }
    }

    ///-- Muons --///
    if (topConfig->useMuons()) {


      if (topConfig->useAntiMuons())
	objectSelection -> muonSelection(new top::AntiMuonMC15(topConfig->muonPtcut(), new top::StandardIsolation(topConfig->muonIsolation(), topConfig->muonIsolationLoose()) ));
      else
	objectSelection -> muonSelection(new top::MuonMC15(topConfig->muonPtcut(), new top::StandardIsolation(topConfig->muonIsolation(), topConfig->muonIsolationLoose()), topConfig->applyTTVACut()) );

    }


    ///-- Taus --///
    if (topConfig->useTaus()) {
      objectSelection->tauSelection(new top::TauMC15());
    }

    ///-- Jets --///
    if (topConfig->useJets()) {
      objectSelection -> jetSelection(new top::JetMC15(topConfig->jetPtcut(), topConfig->jetEtacut(), topConfig->fwdJetAndMET()));
    }

    ///-- Large R Jets --///
    if (topConfig->useLargeRJets()) {// not doing JVT cut for large-R jets
      objectSelection -> largeJetSelection(new top::JetMC15(topConfig->largeRJetPtcut(), topConfig->largeRJetEtacut(), false));
    }

    ///-- Track Jets --///
    if (topConfig->useTrackJets()) {
      objectSelection -> trackJetSelection(new top::TrackJetMC15(topConfig->trackJetPtcut(), topConfig->trackJetEtacut()));
    }

    ///-- Overlap removal --///
    /// single parameter: boolean to do OR with large-R jets
    if (!topConfig->isTruthDxAOD()) {
      objectSelection->overlapRemovalPostSelection(new top::OverlapRemovalASG( (topConfig->doLargeJetOverlapRemoval() && topConfig->useLargeRJets())) );
    }

    return objectSelection;
  }

}
