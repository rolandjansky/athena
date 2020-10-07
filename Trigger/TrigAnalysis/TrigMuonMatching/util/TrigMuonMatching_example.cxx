/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifdef ROOTCORE
#include "xAODRootAccess/Init.h"
#include "xAODRootAccess/tools/ReturnCheck.h"
#include "xAODRootAccess/TEvent.h"
#include "AsgMessaging/Check.h"
#endif 

#include "TrigConfxAOD/xAODConfigTool.h"
#include "TrigDecisionTool/TrigDecisionTool.h"

#include "xAODMuon/MuonContainer.h"
#include "xAODMuon/MuonAuxContainer.h"

#include "TrigMuonMatching/TrigMuonMatching.h"

#include "TChain.h"
#include "TError.h"
#include "TFile.h"
#include "TH1F.h"
#include "TLorentzVector.h"
#include "TCanvas.h"

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

  auto hmu_offline_pt = new TH1F("mu_offline_pt","mu_offline_pt", 100, 0, 300);
  auto hmu_hltmatch_pt = new TH1F("mu_hltmatch_pt","mu_hltmatch_pt", 100, 0, 300);
  auto hmu_l1match_pt = new TH1F("mu_l1match_pt","mu_l1match_pt", 100, 0, 300);
  auto hmu_L2SAmatch_pt = new TH1F("mu_L2SAmatch_pt","mu_L2SAmatch_pt", 100, 0, 300);
  auto hmu_L2CBmatch_pt = new TH1F("mu_L2CBmatch_pt","mu_L2CBmatch_pt", 100, 0, 300);
  
  // Run the files
  size_t nEntries = chain->GetEntries();
  for (size_t entry = 0; entry < nEntries; entry++) {
    event.getEntry(entry);

    xAOD::MuonContainer* goodMuons = new xAOD::MuonContainer();
    xAOD::MuonAuxContainer* goodMuonsAux = new xAOD::MuonAuxContainer();
    goodMuons->setStore(goodMuonsAux);
    
    const xAOD::MuonContainer* muons = 0;
    RETURN_CHECK(APP_NAME, event.retrieve(muons,"Muons"));
    std::cout << "Number of muons  " << muons->size() << std::endl;
    for(const auto* mu : *muons){
      std::cout << "muon pt " << mu->pt() << std::endl;
      hmu_offline_pt->Fill(mu->pt()*0.001);
      Bool_t ismu26_imedium = false;
      Bool_t ismu50 = false;
      ismu26_imedium = m_match_Tool.match(mu,"HLT_mu26_imedium");
      ismu50 = m_match_Tool.match(mu,"HLT_mu50");
      if(ismu26_imedium){
	std::cout << "HLT_mu26_imedium matched." << std::endl;
	hmu_hltmatch_pt->Fill(mu->pt()*0.001);
      }
      if(ismu50)std::cout << "HLT_mu50 matched." << std::endl;

      // L1 
      Bool_t isMU20 = false;
      isMU20 = m_match_Tool.matchL1(mu,"L1_MU20");
      if(isMU20){
	std::cout << "L1_MU20 matched." << std::endl;
	hmu_l1match_pt->Fill(mu->pt()*0.001);
      }
      
      // L2SA
      Bool_t isL2SA = false;
      isL2SA = m_match_Tool.matchL2SA(mu,"L1_MU20","HLT_mu26_imedium");
      if(isL2SA){
	std::cout << "L2SA matched" << std::endl;
	hmu_L2SAmatch_pt->Fill(mu->pt()*0.001);
      }

      // L2CB
      Bool_t isL2CB = false;
      isL2CB = m_match_Tool.matchL2CB(mu,"HLT_mu26_imedium");
      if(isL2CB){
	std::cout << "L2CB matched" << std::endl;
	hmu_L2CBmatch_pt->Fill(mu->pt()*0.001);
      }
      
      xAOD::Muon* muon = new xAOD::Muon();
      muon->makePrivateStore(*mu);
      goodMuons->push_back(muon);
    }

    Int_t index = 0;
    const xAOD::Muon *mu1 = 0;
    const xAOD::Muon *mu2 = 0;
    if(goodMuons->size()==2){
      for(const auto mu : *goodMuons){
        if(index == 0) mu1 = mu;
	if(index == 1) mu2 = mu;
	index = index + 1;
      }
      std::pair<Bool_t,Bool_t> result1, result2;  
      Bool_t valid;
      valid = m_match_Tool.matchDimuon(mu1, mu2, "HLT_2mu14", result1, result2);
      if(!valid) std::cout << "not supported 2mu14" << std::endl;
      if(result1.first && result2.first) std::cout << "HLT_2mu14 matched" << std::endl;

      std::pair<Bool_t,Bool_t> result3, result4;  
      valid = m_match_Tool.matchDimuon(mu1, mu2, "HLT_mu18_mu8noL1", result3, result4);
      if(!valid) std::cout << "not supported mu18_mu8noL1" << std::endl;
      if((result1.first && result2.second) || (result1.second && result2.first)) std::cout << "HLT_mu18_mu8noL1 matched" << std::endl;

    }

  }

  TCanvas* c1 = new TCanvas();
  hmu_offline_pt -> Draw();
  hmu_offline_pt ->SetLineColor(1);
  hmu_hltmatch_pt -> Draw("same");
  hmu_hltmatch_pt ->SetLineColor(2);
  hmu_l1match_pt -> Draw("same");
  hmu_l1match_pt ->SetLineColor(3);
  hmu_L2SAmatch_pt -> Draw("same");
  hmu_L2SAmatch_pt ->SetLineColor(4);
  hmu_L2CBmatch_pt -> Draw("same");
  hmu_L2CBmatch_pt ->SetLineColor(5);
  c1->Write();
  
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
