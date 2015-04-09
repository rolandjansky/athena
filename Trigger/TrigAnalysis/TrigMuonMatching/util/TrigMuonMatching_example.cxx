/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifdef ROOTCORE
#include "xAODRootAccess/Init.h"
#include "xAODRootAccess/tools/ReturnCheck.h"
#include "xAODRootAccess/TEvent.h"
#include "AsgTools/Check.h"
#endif 

#include "TrigConfxAOD/xAODConfigTool.h"
#include "TrigDecisionTool/TrigDecisionTool.h"

#include "xAODMuon/MuonContainer.h"

#include "TrigMuonMatching/TrigMuonMatching.h"

#include "TChain.h"
#include "TError.h"
#include "TFile.h"
#include "TH1F.h"
#include "TLorentzVector.h"

#include <iostream>

using namespace std;
using namespace Trig;
using namespace TrigConf;
using namespace xAOD;

// Config
const char* APP_NAME = "TrigMuonMatching_example";
const char* OUTPUT_FILE = "TrigMuonMatching_example.root";

// Helpers
TChain *getFilesFromCommandLine(int argc, char* argv[]);

int main( int argc, char* argv[] ) {

  // Initialize (as done for all xAOD standalone programs!)
  RETURN_CHECK (APP_NAME, xAOD::Init(APP_NAME));

  auto chain = getFilesFromCommandLine(argc, argv);
  if (chain == nullptr) {
    return 1;
  }

  // The TEvent object
  TEvent event(TEvent::kBranchAccess);
  RETURN_CHECK( APP_NAME, event.readFrom(chain));

  // The configuration tool.
  xAODConfigTool configTool("xAODConfigTool");
  ToolHandle<TrigConf::ITrigConfigTool> configHandle(&configTool);
  ASG_CHECK_SA(APP_NAME,configHandle->initialize());
   
  // The decision tool
  TrigDecisionTool trigDecTool("TrigDecTool");
  ASG_CHECK_SA(APP_NAME, trigDecTool.setProperty("ConfigTool",configHandle));
  ASG_CHECK_SA(APP_NAME, trigDecTool.setProperty("TrigDecisionKey","xTrigDecision"));
  ASG_CHECK_SA(APP_NAME, trigDecTool.initialize());
  ToolHandle<Trig::TrigDecisionTool> m_trigDec(&trigDecTool);
  
  Trig::TrigMuonMatching m_match_Tool("TestMatchClass");
  ASG_CHECK_SA(APP_NAME, m_match_Tool.setProperty("TriggerTool",m_trigDec));
  ASG_CHECK_SA(APP_NAME, m_match_Tool.initialize());

  //// Get an output file open, book events.
  auto outputFile = TFile::Open(OUTPUT_FILE, "RECREATE");
  if (outputFile == nullptr || !outputFile->IsOpen()) {
    return 1;
  }
  
  // Run the files
  size_t nEntries = chain->GetEntries();
  for (size_t entry = 0; entry < nEntries; entry++) {
    event.getEntry(entry);

    const xAOD::MuonContainer* muons = 0;
    RETURN_CHECK(APP_NAME, event.retrieve(muons,"Muons"));
    std::cout << "Number of muons  " << muons->size() << std::endl;
    for(const auto* mu : *muons){
      std::cout << "muon pt " << mu->pt() << std::endl;
      Bool_t ismu26_imedium = false;
      Bool_t ismu50 = false;
      ismu26_imedium = m_match_Tool.match(mu,"HLT_mu26_imedium");
      ismu50 = m_match_Tool.match(mu,"HLT_mu50");
      if(ismu26_imedium)std::cout << "HLT_mu26_imedium matched." << std::endl;
      if(ismu50)std::cout << "HLT_mu50 matched." << std::endl;
    }
    
  }
  outputFile->Write();
  outputFile->Close();
}

TChain *getFilesFromCommandLine(int argc, char* argv[])
{
  if (argc < 2) {
    ::Error(APP_NAME, "Usage: %s <xAOD file> [xAOD file2]...", APP_NAME);
    return nullptr;
  }
  
  auto chain = new TChain("CollectionTree");
  for (int i = 1; i < argc; i++) {
    chain->Add(argv[i]);
  }

  return chain;
}
