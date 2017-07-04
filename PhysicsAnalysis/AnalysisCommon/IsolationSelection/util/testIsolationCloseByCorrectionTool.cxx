/*
 Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

#ifndef CPANALYSISEXAMPLES_ERRORCHECK_H
#define CPANALYSISEXAMPLES_ERRORCHECK_H

#define CHECK( ARG )                                     \
   do {                                                  \
      const bool result = ARG;                           \
      if( ! result ) {                                   \
         ::Error( APP_NAME, "Failed to execute: \"%s\"", \
                  #ARG );                                \
         return 1;                                       \
      }                                                  \
   } while( false )

#endif // CPANALYSISEXAMPLES_ERRORCHECK_H

// ROOT include(s):
#include <TFile.h>
#include <TTree.h>

// Infrastructure include(s):
#ifdef ROOTCORE
#include "xAODRootAccess/Init.h"
#include "xAODRootAccess/TEvent.h"
#include "xAODRootAccess/tools/ReturnCheck.h"
#endif // ROOTCORE

// EDM include(s):
#include "xAODEventInfo/EventInfo.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODMuon/MuonContainer.h"

// Tools
#include "IsolationSelection/IsolationCloseByCorrectionTool.h"
#include "IsolationSelection/TestMacroHelpers.h"

#include "IsolationSelection/IsolationSelectionTool.h"

/// For statistics
#include "xAODCore/tools/IOStats.h"
#include "xAODCore/tools/ReadStats.h"

using namespace std;

const float GeV = 1000.;
const float iGeV = 0.001;
const float PI = 3.1416;

int main(int argc, char** argv) {

    gErrorIgnoreLevel = 0;
    const char* APP_NAME = "test_IsolationCloseByCorrectionTool";
    CHECK(xAOD::Init());

    TString outputTree = "output_testIsolationCloseByCorrectionTool_ttbar.root";

    TString fileName = "root://eosatlas//eos/atlas/atlastier0/tzero/prod/valid1/PowhegPythia_P2011C_ttbar/117050/valid1.117050.PowhegPythia_P2011C_ttbar.recon.AOD.e2658_s1967_s1964_r5787_v111/valid1.117050.PowhegPythia_P2011C_ttbar.recon.AOD.e2658_s1967_s1964_r5787_v111._000001.4";

    if (argc > 1) {
        fileName = argv[1];
    }

    Info(APP_NAME, "Opening file: %s", fileName.Data());
    std::auto_ptr<TFile> ifile(TFile::Open(fileName, "READ"));
    ifile.get();

    TFile* ofile;
    if (produceOutput) {
        ofile = TFile::Open(outputTree, "RECREATE");
    }

    TTree *tree = new TTree("otree", "otree");

    // Create a TEvent object:
    xAOD::TEvent event(xAOD::TEvent::kClassAccess);
    CHECK(event.readFrom(ifile.get()));
    CP::IsolationSelectionTool* m_isoSelTool = new CP::IsolationSelectionTool("isoSelTool_Muon");
    CHECK(m_isoSelTool->setProperty("MuonWP", "FixedCutLoose"));
    CHECK(m_isoSelTool->setProperty("ElectronWP", "Loose"));
    CHECK(m_isoSelTool->setProperty("PhotonWP", "FixedCutTightCaloOnly"));
//    m_isoSelTool->msg().setLevel(MSG::INFO);
    CHECK(m_isoSelTool->initialize());

    // Creating tools.
    CP::IsolationCloseByCorrectionTool* m_isoCloseByTool_Muon = new CP::IsolationCloseByCorrectionTool("isoCloseByTool_Muon");
//     m_isoCloseByTool_Muon->msg().setLevel(MSG::DEBUG);

    Long64_t maxEVT = -1;
    Long64_t entries = event.getEntries();
    if ((entries < maxEVT) || (maxEVT <= 0)) {
        maxEVT = entries;
    }
    Info(APP_NAME, "%lld events found, %lld events will be processed.", entries, maxEVT);

    const int INTERVAL = maxEVT > 20000 ? 10000 : maxEVT / 10;

    for (Long64_t entry = 0; entry < maxEVT; ++entry) {

        event.getEntry(entry);
        const xAOD::EventInfo* ei = 0;
        CHECK(event.retrieve(ei, "EventInfo"));

        if (entry % INTERVAL == 0) {
            Info(APP_NAME, "%lld events processed, on event %llu of run %u", entry, ei->eventNumber(), ei->runNumber());
        }
        eventNumber = ei->eventNumber();

        // get muon container of interest
        const xAOD::MuonContainer* muons = 0;
        CHECK(event.retrieve(muons, "Muons"));


        // get electron container of interest
        const xAOD::ElectronContainer* electrons = 0;
        CHECK(event.retrieve(electrons, "Electrons"));



        // get photon container of interest
        const xAOD::PhotonContainer* photons = 0;
        CHECK(event.retrieve(photons, "Photons"));

        tree->Fill();

    }

    ofile->cd();
    tree->Write();
    ofile->Close();

    Info(APP_NAME, "Finished successfully!");

    xAOD::IOStats::instance().stats().printSmartSlimmingBranchList();

    delete m_isoCloseByTool_Muon;

    return 0;

}
