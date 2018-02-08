/*
 Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

/// a simple testing macro for the MuonEfficiencyCorrections_xAOD package
/// shamelessly stolen from CPToolTests.cxx
// System include(s):
#include <memory>
#include <cstdlib>
#include <string>
#include <map>

// ROOT include(s):
#include <TFile.h>
#include <TError.h>
#include <TStopwatch.h>
#include <TString.h>
#include <TH2F.h>

// Infrastructure include(s):
#ifdef ROOTCORE
#   include "xAODRootAccess/Init.h"
#   include "xAODRootAccess/TEvent.h"
#   include "xAODRootAccess/tools/ReturnCheck.h"
#   include "AsgTools/Check.h"
#endif // ROOTCORE

// EDM include(s):
#include "xAODEventInfo/EventInfo.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODMuon/MuonAuxContainer.h"

// Local include(s):http://www.bloomberg.com/graphics/2015-paul-ford-what-is-code/
#include "MuonEfficiencyCorrections/MuonTriggerScaleFactors.h"
#include "PATInterfaces/ISystematicsTool.h"

#define CHECK_CPCorr(Arg) \
    if (Arg.code() == CP::CorrectionCode::Error){    \
        Error(#Arg,"Correction Code 'Error' (returned in line %i) ",__LINE__); \
        return 1;   \
    }
#define CHECK_CPSys(Arg) \
    if (Arg.code() == CP::SystematicCode::Unsupported){    \
        Warning(#Arg,"Unsupported systematic (in line %i) ",__LINE__); \
    }      

/// Example of how to run the MuonEfficiencyCorrections package to obtain information from muons

int main(int argc, char* argv[]) {

    const char* APP_NAME = argv[0];

    TH2F *h_mask = new TH2F("Mask Regions", "Mask Regions", 200, -5, 5, 32, -3.14, 3.14);
    TH2F *h_masked = new TH2F("Masked Regions", "Masked View", 200, -5, 5, 32, -3.14, 3.14);
    if (argc < 2) {
        Error(APP_NAME, "No file name received!");
        Error(APP_NAME, "  Usage: %s [xAOD file name]", APP_NAME);
        return 1;
    }

    RETURN_CHECK(APP_NAME, xAOD::Init(APP_NAME));

    const TString fileName = argv[1];
    Info(APP_NAME, "Opening file: %s", fileName.Data());
    std::auto_ptr<TFile> ifile(TFile::Open(fileName, "READ"));
    if (!ifile.get()) {
        Error(APP_NAME, " Unable to load xAOD input file");
    }

    // Create a TEvent object:
    xAOD::TEvent event;
    RETURN_CHECK(APP_NAME, event.readFrom(ifile.get(), xAOD::TEvent::kClassAccess));
    Info(APP_NAME, "Number of events in the file: %i",
                    static_cast<int>(event.getEntries()));

    // Decide how many events to run over:
    Long64_t entries = event.getEntries();
    if (argc > 2) {
        const Long64_t e = atoll(argv[2]);
        if (e < entries) {
            entries = e;
        }
    }

    // instance the trigger scale factor tool
    CP::MuonTriggerScaleFactors m_trig_sf("TrigSFClass");
    // Configure it :
    ASG_CHECK_SA(APP_NAME, m_trig_sf.initialize());

    std::vector<CP::SystematicSet> m_sysList; //!
    const CP::SystematicRegistry& registry = CP::SystematicRegistry::getInstance();
    const CP::SystematicSet& recommendedSystematics = registry.recommendedSystematics();

    m_sysList.push_back(CP::SystematicSet());

    for (CP::SystematicSet::const_iterator sysItr = recommendedSystematics.begin(); sysItr != recommendedSystematics.end(); ++sysItr) {
        m_sysList.push_back(CP::SystematicSet());
        m_sysList.back().insert(*sysItr);
    }

    std::vector<float> replicas(50);
    for (Long64_t entry = 0; entry < entries; ++entry) {

        // Tell the object which entry to look at:
        event.getEntry(entry);

        // Print some event information for fun:
        const xAOD::EventInfo* ei = 0;
        RETURN_CHECK(APP_NAME, event.retrieve(ei, "EventInfo"));

        //if(ei->eventNumber() % 1000 == 0)
        Info(APP_NAME,
                        "===>>>  start processing event #%i, "
                                        "run #%i %i events processed so far  <<<===",
                        static_cast<int>(ei->eventNumber()),
                        static_cast<int>(ei->runNumber()),
                        static_cast<int>(entry));

        // Get the Muons from the event:
        const xAOD::MuonContainer* muons = 0;
        RETURN_CHECK(APP_NAME, event.retrieve(muons, "Muons"));
        Info(APP_NAME, "Number of muons: %i",
                        static_cast<int>(muons->size()));

        // Print their properties, using the tools:
        xAOD::MuonContainer::const_iterator mu_itr = muons->begin();
        xAOD::MuonContainer::const_iterator mu_end = muons->end();

        // Create new muoncontainer for the trigger sf tool
        xAOD::MuonContainer *SelectedMuon = new xAOD::MuonContainer;
        xAOD::MuonAuxContainer *SelectedMuonAux = new xAOD::MuonAuxContainer;
        SelectedMuon->setStore(SelectedMuonAux);

        std::string singletrig = "HLT_mu20_iloose_L1MU15_OR_HLT_mu50";
        std::string multitrig = "HLT_mu20_iloose_L1MU15_OR_HLT_mu50_OR_HLT_2mu14";

        for (unsigned int i = 0; i < m_sysList.size(); i++) {
            CHECK_CPSys(m_trig_sf.applySystematicVariation(m_sysList.at(i)));
            std::cout << "Systematic type : " << m_sysList.at(i).name() << std::endl;

            for (; mu_itr != mu_end; ++mu_itr) {

                // Print some info about the selected muon:
                Info(APP_NAME, "  Selected muon: eta = %g, phi = %g, pt = %g",
                                (*mu_itr)->eta(), (*mu_itr)->phi(), (*mu_itr)->pt());

                Double_t eff;
                Int_t runNumber = 267639;
                CHECK_CPCorr(m_trig_sf.setRunNumber(runNumber));

                CHECK_CPCorr(m_trig_sf.getTriggerEfficiency(**mu_itr, eff, singletrig, true));
                std::cout << "Trigger efficiency in data of this muon location : " << eff << std::endl;
                CHECK_CPCorr(m_trig_sf.getTriggerEfficiency(**mu_itr, eff, singletrig, false));
                std::cout << "Trigger efficiency in MC of this muon location : " << eff << std::endl;

                xAOD::Muon* newMuon = new xAOD::Muon;
                newMuon->makePrivateStore(**mu_itr);
                SelectedMuon->push_back(newMuon);

                //if(abs((*mu_itr)->eta()) > 2.5)
                //std::cout << "PT_NODE: eta = " << (*mu_itr)->eta() << ", pt = " <<(*mu_itr)->pt()<<std::endl;

                bool toMask = m_trig_sf.maskFeetRegions(**mu_itr, 0.2);
                if ((*mu_itr)->pt() > 15000.0) {
                    //std::cout << (*mu_itr)->pt() << std::endl;
                    if (toMask)
                    {
                        h_mask->Fill((*mu_itr)->eta(), (*mu_itr)->phi());
                    }
                    else
                    {
                        h_masked->Fill((*mu_itr)->eta(), (*mu_itr)->phi());
                    }
                }
            }

            Double_t triggerSF = 0;
            Info(APP_NAME, "Number of selected muons: %i", static_cast<int>(SelectedMuon->size()));
            CHECK_CPCorr(m_trig_sf.getTriggerScaleFactor(*SelectedMuon, triggerSF, singletrig));
            Info(APP_NAME, "trigger scaleFactor(single lepton trigger) = %g", triggerSF);
            if (SelectedMuon->size() == 2) {
                CHECK_CPCorr(m_trig_sf.getTriggerScaleFactor(*SelectedMuon, triggerSF, multitrig));
                Info(APP_NAME, "trigger scaleFactor(single + di-muon trigger) = %g", triggerSF);
            }
        }
        CHECK_CPSys(m_trig_sf.applySystematicVariation(CP::SystematicSet()));

        // Close with a message:
        if (ei->eventNumber() % 1000 == 0)
            Info(APP_NAME,
                            "===>>>  done processing event #%i, "
                                            "run #%i %i events processed so far  <<<===",
                            static_cast<int>(ei->eventNumber()),
                            static_cast<int>(ei->runNumber()),
                            static_cast<int>(entry + 1));
    }
    h_mask->SetOption("COLZ");
    h_masked->SetOption("COLZ");
    //h_mask->SaveAs("output_mask.root");
    //h_masked->SaveAs("output_masked.root");
    // Return gracefully:
    return 0;
}
