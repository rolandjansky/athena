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

// Local include(s):
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
    xAOD::TStore store;
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
    ASG_CHECK_SA(APP_NAME, m_trig_sf.setProperty("MuonQuality", "Medium")); // Loose,Medium,Tight,HighPt
    ASG_CHECK_SA(APP_NAME, m_trig_sf.setProperty("Isolation", "")); // isolation workinig points have been merged as they have no big difference
    ASG_CHECK_SA(APP_NAME, m_trig_sf.setProperty("Binning", "fine")); // fine(default) or coarse

    //ASG_CHECK_SA(APP_NAME,m_trig_sf.setProperty("Year", "2015")); 

    //ASG_CHECK_SA(APP_NAME,m_trig_sf.setProperty("CalibrationRelease", "160619_ICHEP")); 
    //ASG_CHECK_SA(APP_NAME,m_trig_sf.setProperty("filename", "muon_trigger_eff_nov21.root")); //**LIDIA** Change this!
    ASG_CHECK_SA(APP_NAME, m_trig_sf.setProperty("CustomInputFolder", "/afs/cern.ch/user/d/dellasta/public/MuonTriggerSF")); //**LIDIA** Change this!
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
    //for( Long64_t entry = 0; entry < entries; ++entry ) {
    for (Long64_t entry = 0; entry < 10; ++entry) {

        // Tell the object which entry to look at:
        event.getEntry(entry);

        // Print some event information for fun:
        const xAOD::EventInfo* ei = 0;
        RETURN_CHECK(APP_NAME, event.retrieve(ei, "EventInfo"));
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

        //std::string singletrig = "HLT_mu40";
        //std::string singletrig = "HLT_mu50";
        //std::string singletrig = "HLT_mu24_iloose_L1MU15_OR_HLT_mu24_iloose";
        std::string singletrig = "HLT_mu24_imedium";
        //std::string singletrig = "HLT_mu26_ivarmedium";
        //std::string singletrig = "HLT_mu24_iloose_L1MU15_OR_HLT_mu24_iloose_OR_HLT_mu50"; //wrong string
        //std::string singletrig = "HLT_mu24_iloose_OR_HLT_mu24_iloose_L1MU15_OR_HLT_mu50";
        //std::string singletrig = "HLT_mu8noL1";
        //std::string singletrig = "HLT_mu20_iloose_L1MU15_OR_HLT_mu50";
        //std::string multitrig=  "HLT_mu20_iloose_L1MU15_OR_HLT_mu50_OR_HLT_2mu14";

        for (unsigned int i = 0; i < m_sysList.size(); i++) {
            CHECK_CPSys(m_trig_sf.applySystematicVariation(m_sysList.at(i)));
            std::cout << "Systematic type : " << m_sysList.at(i).name() << std::endl;

            // Create new muoncontainer for the trigger sf tool
            xAOD::MuonContainer *SelectedMuon = new xAOD::MuonContainer;
            xAOD::MuonAuxContainer *SelectedMuonAux = new xAOD::MuonAuxContainer;
            SelectedMuon->setStore(SelectedMuonAux);

            xAOD::MuonContainer::const_iterator mu_itr = muons->begin();
            xAOD::MuonContainer::const_iterator mu_end = muons->end();
            for (; mu_itr != mu_end; ++mu_itr) {

                // Print some info about the selected muon:
                Info(APP_NAME, "  Selected muon: eta = %g, phi = %g, pt = %g",
                                (*mu_itr)->eta(), (*mu_itr)->phi(), (*mu_itr)->pt());

                Double_t eff;

                /*
                 2015 periods AC : runNumber >= 266904 && runNumber <= 272531
                 2015 period  D  : runNumber >= 276073 && runNumber <= 276954
                 2015 period  E  : runNumber >= 278727 && runNumber <= 279928
                 2015 period  F  : runNumber >= 279932 && runNumber <= 280422
                 2015 period  G  : runNumber >= 280423 && runNumber <= 281075
                 2015 period  H  : runNumber >= 281130 && runNumber <= 281411
                 2015 period  J  : runNumber >= 282625 && runNumber <= 284484
                 2016 period  A  : runNumber >= 296939 && runNumber <= 300287
                 2016 period  B  : runNumber >= 300345
                 */

                //int runNumber = 266904; // period 2015 AC
                //int runNumber = 276073; // period 2015 D
                //int runNumber = 278727; // period 2015 E
                //int runNumber = 279932; // period 2015 F
                //int runNumber = 280423; // period 2015 G
                //int runNumber = 281130; // period 2015 H
                //int runNumber = 282625; // period 2015 J
                //int runNumber = 296939; // period 2016 A
                //int runNumber = 300345; // period 2016 B
                int runNumber = 302831; // period 2016 D1D3
                //int runNumber = 310015; // period 2016 G

                //int runNumber = 276072; // WRONG period 2015 D
                //int runNumber = 296938; // WRONG period 2016 A

                CHECK_CPCorr(m_trig_sf.setRunNumber(runNumber));

                if (!(m_sysList.at(i).name().find("TrigSystUncertainty") != std::string::npos)) {
                    CHECK_CPCorr(m_trig_sf.getTriggerEfficiency(**mu_itr, eff, singletrig, true));
                    std::cout << "Trigger efficiency in data of this muon location : " << eff << std::endl;
                }

                CHECK_CPCorr(m_trig_sf.getTriggerEfficiency(**mu_itr, eff, singletrig, false));
                std::cout << "Trigger efficiency in MC of this muon location : " << eff << std::endl;

                xAOD::Muon* newMuon = new xAOD::Muon;
                newMuon->makePrivateStore(**mu_itr);
                SelectedMuon->push_back(newMuon);
            }

            Double_t triggerSF = 0;
            Info(APP_NAME, "Number of selected muons: %i", static_cast<int>(SelectedMuon->size()));
            CHECK_CPCorr(m_trig_sf.getTriggerScaleFactor(*SelectedMuon, triggerSF, singletrig));
            Info(APP_NAME, "trigger scaleFactor(single lepton trigger) = %g", triggerSF);

            /*
             if(SelectedMuon->size() == 2){
             CHECK_CPCorr(m_trig_sf.getTriggerScaleFactor(*SelectedMuon,triggerSF, multitrig));
             Info( APP_NAME, "trigger scaleFactor(single + di-muon trigger) = %g", triggerSF );
             }
             */

        }
        CHECK_CPSys(m_trig_sf.applySystematicVariation(CP::SystematicSet()));

        // Close with a message:
        Info(APP_NAME,
                        "===>>>  done processing event #%i, "
                                        "run #%i %i events processed so far  <<<===",
                        static_cast<int>(ei->eventNumber()),
                        static_cast<int>(ei->runNumber()),
                        static_cast<int>(entry + 1));
    }

    // Return gracefully:
    return 0;
}
