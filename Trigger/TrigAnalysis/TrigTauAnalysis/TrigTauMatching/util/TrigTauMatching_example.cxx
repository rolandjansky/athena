/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifdef ROOTCORE
#include "xAODRootAccess/Init.h"
#include "xAODRootAccess/tools/ReturnCheck.h"
#include "xAODRootAccess/TEvent.h"
#include "xAODCore/ShallowCopy.h"
#include "AsgMessaging/Check.h"
#endif 

#include "TrigConfxAOD/xAODConfigTool.h"
#include "TrigDecisionTool/TrigDecisionTool.h"

#include "xAODTau/TauJetContainer.h"
#include "xAODTau/TauJetAuxContainer.h"

#include "TrigTauMatching/TrigTauMatching.h"

#include "TChain.h"
#include "TError.h"
#include "TFile.h"
#include "TH1F.h"
#include "TLorentzVector.h"

#include <iostream>
#include <memory>

// Config
const char* APP_NAME = "TrigTauMatching_example";

//const std::string prescaledTauTrigger = "HLT_tau25_medium1_tracktwo";
const std::string prescaledTauTrigger = "L1_TAU12IM";
const std::string tauTrigger = "HLT_tau125_medium1_tracktwo";
const std::string diTauTrigger = "HLT_tau35_medium1_tracktwo_tau25_medium1_tracktwo";

// Helpers
std::unique_ptr< TChain > getFilesFromCommandLine( int argc, char* argv[] );

int main( int argc, char* argv[] ) {

  // Initialize (as done for all xAOD standalone programs!)
  RETURN_CHECK (APP_NAME, xAOD::Init(APP_NAME));

  auto chain = getFilesFromCommandLine(argc, argv);
  if( ! chain ) {
    return 1;
  }

  // The TEvent object
  xAOD::TEvent event(xAOD::TEvent::kClassAccess);
  RETURN_CHECK( APP_NAME, event.readFrom(chain.get()));

  // The TStore object
  xAOD::TStore store;
  store.setActive();

  // The configuration tool.
  TrigConf::xAODConfigTool configTool("xAODConfigTool");
  ToolHandle<TrigConf::ITrigConfigTool> configHandle(&configTool);
  ASG_CHECK_SA(APP_NAME,configHandle->initialize());
   
  // The decision tool
  Trig::TrigDecisionTool trigDecTool("TrigDecTool");
  ASG_CHECK_SA(APP_NAME, trigDecTool.setProperty("ConfigTool", configHandle));
  ASG_CHECK_SA(APP_NAME, trigDecTool.setProperty("TrigDecisionKey", "xTrigDecision"));
  ASG_CHECK_SA(APP_NAME, trigDecTool.initialize());
  ToolHandle<Trig::TrigDecisionTool> decisionToolHandle(&trigDecTool);
  
  Trig::TrigTauMatchingTool matchTool("TrigTauMatching");
  ASG_CHECK_SA(APP_NAME, matchTool.setProperty("TrigDecisionTool", decisionToolHandle));
  ASG_CHECK_SA(APP_NAME, matchTool.initialize());

  Trig::TrigTauMatchingTool resurrectedMatchTool("TrigTauMatchingWithResurrection");
  ASG_CHECK_SA(APP_NAME, resurrectedMatchTool.setProperty("TrigDecisionTool", decisionToolHandle));
  ASG_CHECK_SA(APP_NAME, resurrectedMatchTool.setProperty("AllowResurrectedDecision", true));
  ASG_CHECK_SA(APP_NAME, resurrectedMatchTool.initialize());

  // Run the files
  std::pair<int, int> prescaledTauEvents(0, 0);
  std::pair<int, int> resurrectedTauEvents(0, 0);
  std::pair<int, int> tauEvents(0, 0);
  std::pair<int, int> diTauEvents(0, 0);
  for (Long64_t entry = 0; entry < chain->GetEntries(); entry++)
  {
    bool prescaledTauTriggered = false;
    bool resurrectedTauTriggered = false;
    bool tauTriggered = false;
    bool diTauTriggered = false;
    int prescaledTauMatches = 0;
    int resurrectedTauMatches = 0;
    int tauMatches = 0;
    int diTauMatches = 0;


    event.getEntry(entry);

    const xAOD::TauJetContainer *taus;
    RETURN_CHECK(APP_NAME, event.retrieve(taus, "TauJets"));
    const xAOD::EmTauRoIContainer *emTauRoIs;
    RETURN_CHECK(APP_NAME, event.retrieve(emTauRoIs, "LVL1EmTauRoIs"));

    prescaledTauTriggered = trigDecTool.isPassed(prescaledTauTrigger);
    resurrectedTauTriggered = trigDecTool.isPassed(
        prescaledTauTrigger,
        TrigDefs::Physics | TrigDefs::allowResurrectedDecision);
    tauTriggered = trigDecTool.isPassed(tauTrigger);
    diTauTriggered = trigDecTool.isPassed(diTauTrigger);

    for (auto it: *taus)
    {
      // Cut on tau pT >= 25 GeV, |eta| <= 2.5, and medium ID
      if (   it->pt() < 25000
          || std::abs(it->eta()) > 2.5
          || !it->isTau(xAOD::TauJetParameters::JetBDTSigMedium))
      {
        continue;
      }

      if (matchTool.matchL1(it, prescaledTauTrigger, emTauRoIs))
      {
        prescaledTauMatches++;
      }

      if (resurrectedMatchTool.matchL1(it, prescaledTauTrigger, emTauRoIs))
      {
        resurrectedTauMatches++;
      }

      if (matchTool.match(it, tauTrigger))
      {
        tauMatches++;
      }

      if (matchTool.match(it, diTauTrigger))
      {
        diTauMatches++;
      }
    }

    if (prescaledTauTriggered)
    {
      prescaledTauEvents.first++;
      prescaledTauEvents.second += (prescaledTauMatches > 0);
      std::cout << "[" << entry << "] " << prescaledTauMatches
          << " taus matched with "
          << prescaledTauTrigger << std::endl;
    }

    if (resurrectedTauTriggered)
    {
      resurrectedTauEvents.first++;
      resurrectedTauEvents.second += (resurrectedTauMatches > 0);
      std::cout << "[" << entry << "] " << resurrectedTauMatches
          << " taus matched with (resurrected) "
          << prescaledTauTrigger << std::endl;
    }

    if (tauTriggered)
    {
      tauEvents.first++;
      tauEvents.second += (tauMatches > 0);
      std::cout << "[" << entry << "] " << tauMatches << " taus matched with "
          << tauTrigger << std::endl;
    }

    if (diTauTriggered)
    {
      diTauEvents.first++;
      diTauEvents.second += (diTauMatches > 1);
      std::cout << "[" << entry << "] " << diTauMatches << " taus matched with "
          << diTauTrigger << std::endl;
    }
  }

  std::cout << prescaledTauEvents.second << "/" << prescaledTauEvents.first
      << " events for " << prescaledTauTrigger << std::endl;
  std::cout << resurrectedTauEvents.second << "/" << resurrectedTauEvents.first
      << " resurrected events for " << prescaledTauTrigger << std::endl;
  std::cout << tauEvents.second << "/" << tauEvents.first << " events for "
      << tauTrigger << std::endl;
  std::cout << diTauEvents.second << "/" << diTauEvents.first << " events for "
      << diTauTrigger << std::endl;
}

std::unique_ptr< TChain > getFilesFromCommandLine(int argc, char* argv[])
{
  if (argc < 2) {
    ::Error(APP_NAME, "Usage: %s <xAOD file> [xAOD file2]...", APP_NAME);
    return nullptr;
  }
  
  std::unique_ptr< TChain > chain( new TChain( "CollectionTree" ) );
  for (int i = 1; i < argc; i++) {
    chain->Add(argv[i]);
  }

  return chain;
}
