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

#include "AsgTools/AnaToolHandle.h"

#define SET_DUAL_TOOL( TOOLHANDLE, TOOLTYPE, TOOLNAME )                \
  ASG_SET_ANA_TOOL_TYPE(TOOLHANDLE, TOOLTYPE);                        \
  TOOLHANDLE.setName(TOOLNAME);

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

    // Creating the tools.

    //Define first the isolation selection tool with all WP
    asg::AnaToolHandle<CP::IIsolationSelectionTool> m_isoSelTool;
    SET_DUAL_TOOL(m_isoSelTool, CP::IsolationSelectionTool, "IsolationSelectionTool");
    CHECK(m_isoSelTool.setProperty("MuonWP", "FixedCutLoose"));
    CHECK(m_isoSelTool.setProperty("ElectronWP", "Loose"));
    CHECK(m_isoSelTool.setProperty("PhotonWP", "FixedCutTightCaloOnly"));
    CHECK(m_isoSelTool.retrieve());

    //Now let's come to the IsolaionCloseByCorrecitonTool
    asg::AnaToolHandle<CP::IIsolationCloseByCorrectionTool> m_isoCloseByTool;
    SET_DUAL_TOOL(m_isoCloseByTool, CP::IsolationCloseByCorrectionTool, "IsolationCloseByCorrectionTool");

    //pass the instance of the created isolation tool
    CHECK(m_isoCloseByTool.setProperty("IsolationSelectionTool", m_isoSelTool.getHandle()));

    //Name of the quality decorator defining all nearby particles used to correct the isolation of a given particle
    CHECK(m_isoCloseByTool.setProperty("SelectionDecorator", "quality"));

    //If you want to use only particles survivving the overlap removal. Then just add this line. Only particles with auxdata<char>("passOR") == 1 are used
    //CHECK(m_isoCloseByTool.setProperty("PassOverlapDecorator","passOR"));

    //What is the name of the final isolation decorator. The tool internally calls P->auxdata<char>("CorrectedIsol") = m_IsoTool->accept(*P)
    CHECK(m_isoCloseByTool.setProperty("IsolationSelectionDecorator", "CorrectedIsol"));

    //By default all particles in the container are corrected. For the purpose of saving processing time one can optionally
    //define this property. Then the isolation of the particle is only corrected only if the particle passes the input quality or if this decorator is set to true
    //CHECK(m_isoCloseByTool.setProperty("CorrectIsolationOf", "CorrectTheThing"));

    //The closeByIsoCorrectionTool accesses the default variables of a particle via the original container links
    //Optionally one can backup the isolation values before correction. Then the tool creates an auxelement called <BackupPrefix>_<IsoVariable> This might be interesting if the people are interested in writing
    //out the default values using CxAODs
    CHECK(m_isoCloseByTool.setProperty("BackupPrefix", "Default"));
    CHECK(m_isoCloseByTool.retrieve());

    //Define  the output
    TFile* ofile;
    ofile = TFile::Open(outputTree, "RECREATE");

    TTree *tree = new TTree("otree", "otree");
    CP::IsoCorrectionTestHelper eleBranches(tree, "Electrons", m_isoSelTool->getElectronWPs());
    CP::IsoCorrectionTestHelper muoBranches(tree, "Muons", m_isoSelTool->getMuonWPs());
    CP::IsoCorrectionTestHelper phoBranches(tree, "Photons", m_isoSelTool->getPhotonWPs());

    // Create a TEvent object:
    xAOD::TEvent event(xAOD::TEvent::kClassAccess);
    CHECK(event.readFrom(ifile.get()));

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
//        eventNumber = ei->eventNumber();

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
