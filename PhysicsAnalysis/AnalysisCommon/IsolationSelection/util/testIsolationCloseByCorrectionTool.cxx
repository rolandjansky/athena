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
// Local include(s):
#include <xAODCore/ShallowCopy.h>
#include <xAODBase/ObjectType.h>
#include <xAODBase/IParticleHelpers.h>
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

template<typename Container> StatusCode RetrieveContainer(xAOD::TEvent &Ev, const std::string &Key, Container* &C, xAOD::ShallowAuxContainer* &Aux) {
    if (Aux) delete Aux;
    if (C) delete C;
    const Container* InCont(0);
    if (!Ev.retrieve(InCont, Key).isSuccess()) {
        Error("RetrieveContainer()", "Could not retrieve %s", Key.c_str());
        return StatusCode::FAILURE;
    }
    typename std::pair<Container*, xAOD::ShallowAuxContainer*> shallowcopy = xAOD::shallowCopyContainer(*InCont);
    Aux = shallowcopy.second;
    C = shallowcopy.first;
    if (!xAOD::setOriginalObjectLink(*InCont, *C)) {
        Error("RetrieveContainer()", "Failed to set Object links to %s", Key.c_str());
        delete Aux;
        return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;
}

int main(int argc, char** argv) {

    gErrorIgnoreLevel = 0;
    const char* APP_NAME = "test_IsolationCloseByCorrectionTool";
    CHECK(xAOD::Init());

    TString outputTree = "test_IsolationCloseByCorrectionTool.root";

    TString fileName = "root://eosatlas//eos/atlas/atlastier0/tzero/prod/valid1/PowhegPythia_P2011C_ttbar/117050/valid1.117050.PowhegPythia_P2011C_ttbar.recon.AOD.e2658_s1967_s1964_r5787_v111/valid1.117050.PowhegPythia_P2011C_ttbar.recon.AOD.e2658_s1967_s1964_r5787_v111._000001.4";

    if (argc > 1) {
        fileName = argv[1];
    }

    Info(APP_NAME, "Opening file: %s", fileName.Data());
    std::auto_ptr<TFile> ifile(TFile::Open(fileName, "READ"));
    ifile.get();
    // Create a TEvent object:
    xAOD::TEvent event(xAOD::TEvent::kClassAccess);
    CHECK(event.readFrom(ifile.get()));

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
    CHECK(m_isoCloseByTool.setProperty("SelectionDecorator", "isCloseByObject"));

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

    Long64_t maxEVT = -1;
    Long64_t entries = event.getEntries();
    if ((entries < maxEVT) || (maxEVT <= 0)) {
        maxEVT = entries;
    }
    Info(APP_NAME, "%lld events found, %lld events will be processed.", entries, maxEVT);

    const int INTERVAL = maxEVT > 20000 ? 10000 : maxEVT / 10;

    xAOD::ShallowAuxContainer* AuxMuons = nullptr;
    xAOD::ShallowAuxContainer* AuxElectrons = nullptr;
    xAOD::ShallowAuxContainer* AuxPhotons = nullptr;
    xAOD::MuonContainer* Muons = nullptr;
    xAOD::ElectronContainer* Electrons = nullptr;
    xAOD::PhotonContainer* Photons = nullptr;

    SG::AuxElement::Decorator<char> dec_PassQuality("isCloseByObject");
    SG::AuxElement::Decorator<char> dec_PassIsol("DefaultIso");

    for (Long64_t entry = 0; entry < maxEVT; ++entry) {

        event.getEntry(entry);
        const xAOD::EventInfo* ei = 0;
        CHECK(event.retrieve(ei, "EventInfo"));

        if (entry % INTERVAL == 0) {
            Info(APP_NAME, "%lld events processed, on event %llu of run %u", entry, ei->eventNumber(), ei->runNumber());
        }
//        eventNumber = ei->eventNumber();

        //Retrieve the Containers and create the ShallowAux links
        if (!RetrieveContainer(event, "Muons", Muons, AuxMuons).isSuccess()) break;
        if (!RetrieveContainer(event, "Electrons", Electrons, AuxElectrons).isSuccess()) break;
        if (!RetrieveContainer(event, "Photons", Photons, AuxPhotons).isSuccess()) break;
        for (const auto ielec : *Electrons) {
            //Store if the electron passes the isolation
            dec_PassIsol(*ielec) = m_isoSelTool->accept(*ielec);
            //Quality criteria only baseline kinematic selection
            dec_PassQuality(*ielec) = ielec->pt() > 10.e3 && fabs(ielec->eta()) < 2.47;
        }

        for (const auto iphot : *Photons) {
            //Store if the photon passes the isolation (only needed for later comparisons)
            dec_PassIsol(*iphot) = m_isoSelTool->accept(*iphot);
            //Quality criteria only baseline kinematic selection
            dec_PassQuality(*iphot) = iphot->pt() > 25.e3 && fabs(iphot->eta()) < 2.35;
        }
        for (const auto imuon : *Muons) {
            //Store if the muon passes the isolation
            dec_PassIsol(*imuon) = m_isoSelTool->accept(*imuon);
            //Quality criteria only baseline kinematic selection
            dec_PassQuality(*imuon) = imuon->pt() > 5.e3 && fabs(imuon->eta()) < 2.7;
        }
        //Okay everything is defined for the preselection of the algorithm. lets  pass the things  towards the IsoCorrectionTool

        if (m_isoCloseByTool->getCloseByIsoCorrection(Electrons, Muons, Photons).code() == CP::CorrectionCode::Error) {
            return EXIT_FAILURE;
        }
        // The isoCorrectionTool has now corrected everything using close-by objects satisfiyng the dec_PassQuality criteria
        // The name of the decorator is set via the 'SelectionDecorator' property of the tool
        // Optionally one can also define that the tool shall only objects surviving the overlap removal without  changing the initial decorator
        // Use therefore the 'PassOverlapDecorator' property to define the decorators name
        // If you define  the 'BackupPrefix' property then the original values are stored before correction <Prefix>_<IsolationCone>
        // The final result  whether the object  passes the isolation criteria now can be stored in the 'IsolationSelectionDecorator' e.g. 'CorrectedIso'

        //Store everything in the final ntuples
        CHECK(eleBranches.Fill(Electrons));
        CHECK(muoBranches.Fill(Muons));
        CHECK(phoBranches.Fill(Photons));
        tree->Fill();

    }

    ofile->cd();
    tree->Write();
    ofile->Close();

    Info(APP_NAME, "Finished successfully!");
    return EXIT_SUCCESS;

}
