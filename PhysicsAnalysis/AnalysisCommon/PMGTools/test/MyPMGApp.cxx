/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/// Example standalone executable using POOL to read an xAOD
/// Tests several tools, showing example usage

#ifndef PMGTOOLS_MYPMGAPP_H
#define PMGTOOLS_MYPMGAPP_H

// EDM includes
#include "AsgTools/AnaToolHandle.h"
#include "AsgTools/MessageCheck.h"  // for messaging
#include "POOLRootAccess/TEvent.h"  // event looping
#include "xAODJet/JetContainer.h"   // for jet studies
#include "GaudiKernel/ToolHandle.h" // for better working with tools
#include "PATInterfaces/IWeightTool.h"

// ROOT includes
#include "TString.h"
#include "TSystem.h"
#include "TH1F.h"
#include "TFile.h"

// Local includes
#include "PMGTools/PMGSherpa22VJetsWeightTool.h"
#include "PMGTools/PMGTruthWeightTool.h"

// For convenience messaging macros
using namespace asg::msgUserCode;


int main(int argc, char *argv[])
{
  // for access to everything including EVNT
  IAppMgrUI *app = POOL::Init(); // important to do this first!
  // just for xAOD file type access - improved performance due to
  // fast xAOD reading mechanism rather than generic POOL reading mechanism
  // IAppMgrUI* app = POOL::Init("POOLRootAccess/basixAOD.opts");

  // test the interface
  ANA_MSG_INFO("Creating the PMGSherpa22VJetsWeightTool...");
  asg::AnaToolHandle< IWeightTool > pmgTool("PMGTools::PMGSherpa22VJetsWeightTool/PMGSherpa22VJetsWeightTool");
  ANA_CHECK(pmgTool.setProperty("TruthJetContainer", "AntiKt4TruthWZJets"));  // default
  ANA_CHECK(pmgTool.initialize());
  // can make the subtool directly or via this cast
  ANA_MSG_INFO("Casting to PMGSherpa22VJetsWeightTool...");
  PMGTools::PMGSherpa22VJetsWeightTool* sherpaTool = dynamic_cast<PMGTools::PMGSherpa22VJetsWeightTool*>(&*pmgTool);

  // Create the truth weight tool:
  // ... could create directly with
  // ANA_MSG_INFO("Creating the PMGTruthWeightTool...");
  // PMGTools::PMGTruthWeightTool truthWeightTool("PMGTruthWeightTool");
  // ANA_CHECK(truthWeightTool.setProperty("OutputLevel", MSG::INFO));
  // ANA_CHECK(truthWeightTool.sysInitialize()); // must call sysInitialize to get the callbacks registered properly for an AsgMetadataTool
  // ... but better to do this through a ToolHandle
  asg::AnaToolHandle< PMGTools::IPMGTruthWeightTool > truthWeightTool("PMGTools::PMGTruthWeightTool/PMGTruthWeightTool");
  ANA_CHECK(truthWeightTool.initialize());

  // Open the input file:
  TString fileName = "$ASG_TEST_FILE_MC";
  if (argc < 2) {
    ANA_MSG_WARNING("No file name received, using $ASG_TEST_FILE_MC");
  } else {
    fileName = argv[1]; // use the user provided file
  }
  ANA_MSG_INFO("Opening file: " << gSystem->ExpandPathName(fileName.Data()));


  // loop over input file with POOL
  POOL::TEvent evt;
  evt.readFrom(fileName);
  ANA_MSG_INFO("... with " << evt.getEntries() << " entries");

  // book some histograms
  TH1F *h_njetTruth = new TH1F("jetmult_AntiKt4TruthJets", "jetmult_AntiKt4TruthJets", 10, -0.5, 9.5);
  TH1F *h_njetTruthWZ = new TH1F("jetmult_AntiKt4TruthWZJets", "jetmult_AntiKt4TruthWZJets", 10, -0.5, 9.5);
  TH1F *h_njetTruth_Tool = new TH1F("jetmult_AntiKt4TruthJets_Tool", "jetmult_AntiKt4TruthJets_Tool", 10, -0.5, 9.5);
  TH1F *h_njetTruthWZ_Tool = new TH1F("jetmult_AntiKt4TruthWZJets_Tool", "jetmult_AntiKt4TruthWZJets_Tool", 10, -0.5, 9.5);
  TFile *histfile = TFile::Open("hists.root", "RECREATE");

  bool debug = false;

  for (int i = 0; i < evt.getEntries(); i++) {
    if (evt.getEntry(i) < 0) { ANA_MSG_ERROR("Failed to read event " << i); continue; }

    const xAOD::JetContainer *truthWZJets = 0;
    evt.retrieve(truthWZJets, "AntiKt4TruthWZJets");
    int nTruthWZJets20 = 0;
    int nTruthWZJets30 = 0;

    // loop over all AntiKtTruthWZ jets
    for (unsigned i = 0; i != truthWZJets->size(); i++) {
      const xAOD::Jet *truthjet = (*truthWZJets)[i];
      // if(m_debug) { std::cout << "Index " << i << " truthjet pointer = " << truthjet << std::endl; }
      double Pt = truthjet->pt();
      double AbsEta = fabs(truthjet->eta());

      if (Pt > 20000.0 && AbsEta < 4.5) {nTruthWZJets20++;}
      if (Pt > 30000.0 && AbsEta < 4.5) {nTruthWZJets30++;}
    } //end loop over truth jets

    if (debug) {
      std::cout << "nTruthWZJets20=" << nTruthWZJets20 << std::endl;
      std::cout << "nTruthWZJets30=" << nTruthWZJets30 << std::endl;
    }
    h_njetTruthWZ->Fill(nTruthWZJets20);

    // same for AntiKt4TruthJets
    const xAOD::JetContainer *truthJets = 0;
    evt.retrieve(truthJets, "AntiKt4TruthJets");
    int nTruthJets20 = 0;

    // loop over all truth jets
    for (unsigned i = 0; i != truthJets->size(); i++) {
      const xAOD::Jet *truthjet = (*truthJets)[i];
      double Pt = truthjet->pt();
      double AbsEta = fabs(truthjet->eta());

      if (Pt > 20000.0 && AbsEta < 4.5) {nTruthJets20++;}
    } // end loop over truth jets
    h_njetTruth->Fill(nTruthJets20);

    // Now use the PMG tool directly over AntiKt4TruthWZJets
    int njet = sherpaTool->getSherpa22VJets_NJet("AntiKt4TruthWZJets");
    if (debug) { std::cout << "njets TruthWZJets " << njet << std::endl; }
    h_njetTruthWZ_Tool->Fill(njet);

    double reweight(0);
    int nuse = njet;
    reweight = sherpaTool->getSherpa22VJets_NJetCorrection(nuse);
    if (debug) { std::cout << "correction with njet input(check) " << nuse << " " << reweight  << std::endl; }

    // check
    reweight = sherpaTool->getSherpa22VJets_NJetCorrection("AntiKt4TruthWZJets");
    if (debug) { std::cout << "correction with string " << reweight << std::endl; }

    // and do again for TruthJets
    njet = sherpaTool->getSherpa22VJets_NJet("AntiKt4TruthJets");
    if (debug) { std::cout << "njets (TruthJets) " << njet << std::endl; }
    h_njetTruth_Tool->Fill(njet);

    nuse = njet;
    reweight = sherpaTool->getSherpa22VJets_NJetCorrection(nuse) ;
    if (debug) { std::cout << "correction with njet input (TruthJets) (check) " << nuse << " " << reweight  << std::endl; }

    // check
    reweight = sherpaTool->getSherpa22VJets_NJetCorrection("AntiKt4TruthJets");

    if (debug) { std::cout << "correction with string (TruthJets) " << reweight << std::endl; }

    // and test the interface itself
    reweight = sherpaTool->getWeight();
    if (debug) { std::cout << "correction from Interface" << reweight << std::endl; }

    // Test the PMGTruthWeightTool interface
    auto weightNames = truthWeightTool->getWeightNames();
    ANA_MSG_INFO("Event #" << i << ": found " << weightNames.size() << " weights for this event");

  }

  app->finalize(); // trigger finalization of all services and tools created by the Gaudi Application

  histfile->cd();
  h_njetTruthWZ->Write();
  h_njetTruthWZ_Tool->Write();
  h_njetTruth->Write();
  h_njetTruth_Tool->Write();
  histfile->Close();

  return 0;
}

#endif //> !PMGTOOLS_MYPMGAPP_H
