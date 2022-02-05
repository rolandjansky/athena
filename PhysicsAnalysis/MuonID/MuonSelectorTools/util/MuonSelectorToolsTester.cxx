/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/// a simple testing macro for the MuonSelectorTools_xAOD package
/// shamelessly stolen from CPToolTests.cxx

// System include(s):
#include <cstdlib>
#include <iomanip>
#include <map>
#include <memory>
#include <string>

// ROOT include(s):
#include <TError.h>
#include <TFile.h>
#include <TStopwatch.h>
#include <TString.h>

// Infrastructure include(s):
#ifdef XAOD_STANDALONE
#include "xAODRootAccess/Init.h"
#include "xAODRootAccess/TEvent.h"
#else
#include "POOLRootAccess/TEvent.h"
#include "StoreGate/StoreGateSvc.h"
#endif

// EDM include(s):
#include "xAODEventInfo/EventInfo.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackingPrimitives.h"

// Local include(s):
#include "MuonSelectorTools/MuonSelectionTool.h"

// Needed for Smart Slimming
#include "xAODCore/tools/IOStats.h"
#include "xAODCore/tools/ReadStats.h"

// Truth classification
#include "MCTruthClassifier/MCTruthClassifierDefs.h"

/// Example of how to run the MuonSelectorTools package to obtain information from muons

int main(int argc, char* argv[]) {
    // The application's name:
    const char* APP_NAME = argv[0];

    // Check if we received a file name:
    if (argc < 2) {
        Error(APP_NAME, "No file name received!");
        Error(APP_NAME, "  Usage: %s [xAOD file name] [Nevts to process]", APP_NAME);
        return 1;
    }

    // Create a TEvent object:
#ifdef XAOD_STANDALONE
    if ( xAOD::Init( APP_NAME ).isFailure() ) {
      Error( APP_NAME, "Failed to do xAOD::Init!" );
      return 1;
    }
    xAOD::TEvent event( xAOD::TEvent::kClassAccess );
#else
    POOL::TEvent event( POOL::TEvent::kClassAccess );
#endif

    // Open the input file:
    const TString fileName = argv[1];
    Info(APP_NAME, "Opening file: %s", fileName.Data());
    std::unique_ptr<TFile> ifile(TFile::Open(fileName, "READ"));
    if ( !ifile.get() ) {
      Error( APP_NAME, "Failed to open input file!" );
      return 1;
    }

    //Read from input file with TEvent
    if ( event.readFrom( ifile.get() ).isFailure() ) {
      Error( APP_NAME, "Failed to read from input file!" );
      return 1;
    }
    Info(APP_NAME, "Number of events in the file: %i", static_cast<int>(event.getEntries()));

    // Decide how many events to run over:
    Long64_t entries = event.getEntries();
    if (argc > 2) {
        const Long64_t e = atoll(argv[2]);
        if (e < entries) { entries = e; }
    }


    // Create a set of selector tools configured for each of the available working points

    std::vector<std::unique_ptr<CP::MuonSelectionTool> > selectorTools;
    selectorTools.clear();

    const int Nwp = 7;  // number of working points (tool instances)

    std::vector<std::string> WPnames = {"Tight", "Medium", "Loose", "VeryLoose", "HighPt", "LowPt", "LowPtMVA"};

    for (int wp = 0; wp < Nwp; wp++) {
        Info(APP_NAME, "Creating selector tool for working point: %s ...", WPnames[wp].c_str());

        CP::MuonSelectionTool* muonSelection = new CP::MuonSelectionTool("MuonSelection_" + WPnames[wp]);
        muonSelection->msg().setLevel(MSG::INFO);

	bool failed = false;

        failed = failed || muonSelection->setProperty("MaxEta", 2.7).isFailure();

	if (WPnames[wp] == "LowPtMVA") {
	  failed = failed || muonSelection->setProperty( "MuQuality", 5).isFailure();
	  failed = failed || muonSelection->setProperty( "UseMVALowPt", true).isFailure();
	}
	else
	  failed = failed || muonSelection->setProperty("MuQuality", wp).isFailure();

        failed = failed || muonSelection->setProperty("TurnOffMomCorr", true).isFailure();
        failed = failed || muonSelection->initialize().isFailure();

	if (failed) {
	  Error( APP_NAME, "Failed to set up MuonSelectorTool for working point: %s !", WPnames[wp].c_str() );
	  return 1;
	}

        selectorTools.emplace_back(muonSelection);
    }

    // done setting up selector tools

    // Create "padding" strings to facilitate easy table view of results
    std::vector<std::string> padding;
    padding.clear();

    unsigned int maxNameLength = 0;
    for (int wp = 0; wp < Nwp; wp++)
        if (WPnames[wp].size() > maxNameLength) maxNameLength = WPnames[wp].size();

    for (int wp = 0; wp < Nwp; wp++) {
        std::string pad = "";
        for (unsigned int i = 0; i < maxNameLength - WPnames[wp].size(); i++) pad += " ";

        padding.push_back(pad);
    }

    // string with names of working points for selection results display
    std::string namesString = "                           ";
    for (int wp = 0; wp < Nwp; wp++) namesString += WPnames[wp] + "   ";

    // Muon counters
    int allMuons = 0;
    int nPositive = 0;
    int nNegative = 0;

    // Summary information - how many muons passed each working point (with and without vetoing bad muons)
    int selectedMuons[Nwp];
    for (int wp = 0; wp < Nwp; wp++) selectedMuons[wp] = 0;

    int selectedMuonsNotBad[Nwp];
    for (int wp = 0; wp < Nwp; wp++) selectedMuonsNotBad[wp] = 0;

    // Obtain summary information also split by muon type
    const int Ntype = 5;

    std::string typeNames[Ntype];
    for (int type = 0; type < Ntype; type++) {
        if (type == xAOD::Muon::Combined)
            typeNames[type] = "combined";
        else if (type == xAOD::Muon::MuonStandAlone)
            typeNames[type] = "stand-alone";
        else if (type == xAOD::Muon::SegmentTagged)
            typeNames[type] = "segment-tagged";
        else if (type == xAOD::Muon::CaloTagged)
            typeNames[type] = "calo-tagged";
        else if (type == xAOD::Muon::SiliconAssociatedForwardMuon)
            typeNames[type] = "forward";
        else
            typeNames[type] = "unknown";
    }

    // Muon counters for each type
    int allMuonsType[Ntype];
    for (int type = 0; type < Ntype; type++) allMuonsType[type] = 0;

    // Muon counters for muons of each type passing each working point
    int selectedMuonsType[Ntype][Nwp];
    for (int type = 0; type < Ntype; type++)
        for (int wp = 0; wp < Nwp; wp++) selectedMuonsType[type][wp] = 0;

    int selectedMuonsTypeNotBad[Ntype][Nwp];
    for (int type = 0; type < Ntype; type++)
        for (int wp = 0; wp < Nwp; wp++) selectedMuonsTypeNotBad[type][wp] = 0;


    // Obtain summary information also split by primary author
    const int Nauthor = xAOD::Muon::NumberOfMuonAuthors;

    std::string authorNames[Nauthor];
    for (int author = 0; author < Nauthor; author++) {
        if (author == xAOD::Muon::MuidCo)
            authorNames[author] = "MuidCo";
        else if (author == xAOD::Muon::STACO)
            authorNames[author] = "STACO";
        else if (author == xAOD::Muon::MuTag)
            authorNames[author] = "MuTag";
        else if (author == xAOD::Muon::MuTagIMO)
            authorNames[author] = "MuTagIMO";
        else if (author == xAOD::Muon::MuidSA)
            authorNames[author] = "MuidSA";
        else if (author == xAOD::Muon::MuGirl)
            authorNames[author] = "MuGirl";
        else if (author == xAOD::Muon::MuGirlLowBeta)
            authorNames[author] = "MuGirlLowBeta";
        else if (author == xAOD::Muon::CaloTag)
            authorNames[author] = "CaloTag";
        else if (author == xAOD::Muon::CaloLikelihood)
            authorNames[author] = "CaloLikelihood";
        else if (author == xAOD::Muon::CaloScore)
            authorNames[author] = "CaloScore";
        else if (author == xAOD::Muon::ExtrapolateMuonToIP)
            authorNames[author] = "ExtrapolateMuonToIP";
        else
            authorNames[author] = "unknown";
    }

    // Muon counters for each author
    int allMuonsAuthor[Nauthor];
    for (int author = 0; author < Nauthor; author++) allMuonsAuthor[author] = 0;

    // Muon counters for muons of each author passing each working point
    int selectedMuonsAuthor[Nauthor][Nwp];
    for (int author = 0; author < Nauthor; author++)
        for (int wp = 0; wp < Nwp; wp++) selectedMuonsAuthor[author][wp] = 0;

    int selectedMuonsAuthorNotBad[Nauthor][Nwp];
    for (int author = 0; author < Nauthor; author++)
        for (int wp = 0; wp < Nwp; wp++) selectedMuonsAuthorNotBad[author][wp] = 0;


    // Obtain summary information also split by muon |eta|
    const int Neta = 4;
    double etaCuts[Neta - 1] = {1.0, 2.0, 2.5};

    std::string etaRegions = "|eta| < 1.0      1.0 < |eta| < 2.0   2.0 < |eta| < 2.5     |eta| > 2.5";

    // Muon counters for each eta region
    int allMuonsEta[Neta];
    for (int eta = 0; eta < Neta; eta++) allMuonsEta[eta] = 0;

    // Muon counters for muons in each eta region passing each working point
    int selectedMuonsEta[Neta][Nwp];
    for (int eta = 0; eta < Neta; eta++)
        for (int wp = 0; wp < Nwp; wp++) selectedMuonsEta[eta][wp] = 0;

    int selectedMuonsEtaNotBad[Neta][Nwp];
    for (int eta = 0; eta < Neta; eta++)
        for (int wp = 0; wp < Nwp; wp++) selectedMuonsEtaNotBad[eta][wp] = 0;

    // Obtain summary information also split by muon truth type
    const int NtruthType = 5;

    std::string truthTypeNames[NtruthType] = {"Prompt", "Non-isolated", "Hadron", "Background", "Other"};

    // Muon counters for each truth type
    int allMuonsTruthType[NtruthType];
    for (int truthType = 0; truthType < NtruthType; truthType++) allMuonsTruthType[truthType] = 0;

    // Muon counters for muons of each type passing each working point
    int selectedMuonsTruthType[NtruthType][Nwp];
    for (int truthType = 0; truthType < NtruthType; truthType++)
        for (int wp = 0; wp < Nwp; wp++) selectedMuonsTruthType[truthType][wp] = 0;

    int selectedMuonsTruthTypeNotBad[NtruthType][Nwp];
    for (int truthType = 0; truthType < NtruthType; truthType++)
        for (int wp = 0; wp < Nwp; wp++) selectedMuonsTruthTypeNotBad[truthType][wp] = 0;

    //To determine whether to print truth classification table
    bool isMC = false;

    // Loop over the events:
    for (Long64_t entry = 0; entry < entries; ++entry) {
        // Tell the object which entry to look at:
        event.getEntry(entry);

        // Counters for selected muons within each event
        int selectedMuonsEvent[Nwp];
        for (int wp = 0; wp < Nwp; wp++) selectedMuonsEvent[wp] = 0;

        int selectedMuonsEventNotBad[Nwp];
        for (int wp = 0; wp < Nwp; wp++) selectedMuonsEventNotBad[wp] = 0;

        // Print some event information for fun:
        const xAOD::EventInfo* ei = 0;
	if ( event.retrieve( ei, "EventInfo" ).isFailure() ) {
	  Error( APP_NAME, "Failed to read EventInfo!" );
	  return 1;
	}
        Info(APP_NAME,
             "===>>>  start processing event #%i, "
             "run #%i %i events processed so far  <<<===",
             static_cast<int>(ei->eventNumber()), static_cast<int>(ei->runNumber()), static_cast<int>(entry));

        // Get the Muons from the event:
        const xAOD::MuonContainer* muons = 0;
	if ( event.retrieve( muons, "Muons" ).isFailure() ) {
	  Error( APP_NAME, "Failed to read Muons container!" );
	  return 1;
	}
        Info(APP_NAME, "Number of muons: %i", static_cast<int>(muons->size()));

        xAOD::Muon::Quality my_quality;
        bool passesIDRequirements;
        bool passesPreselectionCuts;

        int muCounter = 0;  // muon counter for each event

        // Print their properties, using the tools:
        xAOD::MuonContainer::const_iterator mu_itr = muons->begin();
        xAOD::MuonContainer::const_iterator mu_end = muons->end();
        for (; mu_itr != mu_end; ++mu_itr) {
            int etaIndex = Neta - 1;
            for (int eta = 0; eta < Neta - 1; eta++)
                if (std::abs((*mu_itr)->eta()) < etaCuts[eta]) {
                    etaIndex = eta;
                    break;
                }

            allMuons++;
            allMuonsType[(*mu_itr)->muonType()]++;
            allMuonsAuthor[(*mu_itr)->author()]++;
            allMuonsEta[etaIndex]++;
            muCounter++;

            Info(APP_NAME, "===== Muon number: %i", static_cast<int>(muCounter));

            // Check truth origin
            isMC = ei->eventType(xAOD::EventInfo::IS_SIMULATION);
	    int truthClass = isMC ? (*mu_itr)->auxdata<int>("truthType") : -999;

            int truthType;
            if (truthClass == MCTruthPartClassifier::IsoMuon)
                truthType = 0;
            else if (truthClass == MCTruthPartClassifier::NonIsoMuon)
                truthType = 1;
            else if (truthClass == MCTruthPartClassifier::Hadron)
                truthType = 2;
            else if (truthClass == MCTruthPartClassifier::BkgMuon)
                truthType = 3;
            else
                truthType = 4;

            allMuonsTruthType[truthType]++;

            if ((*mu_itr)->charge() > 0)
                nPositive++;
            else
                nNegative++;

            passesIDRequirements = selectorTools[0]->passedIDCuts(**mu_itr);
            passesPreselectionCuts = selectorTools[0]->passedMuonCuts(**mu_itr);
            my_quality = selectorTools[0]->getQuality(**mu_itr);

            // Print some general information about the muon
            if (isMC) Info(APP_NAME, "Muon truthType:       %d (%s)", truthClass, truthTypeNames[truthType].c_str());
            Info(APP_NAME, "Muon pT [GeV]:        %g ", std::abs((*mu_itr)->pt()) / 1000.);
            Info(APP_NAME, "Muon eta, phi:        %g, %g ", (*mu_itr)->eta(), (*mu_itr)->phi());
            Info(APP_NAME, "Muon muonType:        %d (%s)", (*mu_itr)->muonType(), typeNames[(*mu_itr)->muonType()].c_str());
            Info(APP_NAME, "Muon primary author:  %d (%s)", (*mu_itr)->author(), authorNames[(*mu_itr)->author()].c_str());

            Info(APP_NAME, "Muon quality (from tool, from xAOD):      %d, %d", my_quality, (*mu_itr)->quality());
            Info(APP_NAME, "Muon passes cuts (ID hits, preselection): %d, %d", passesIDRequirements, passesPreselectionCuts);


            // Now, let's check whether the muon passes the different working points and also whether it is flagged as bad

            std::string selectionResults = "Muon selection acceptance:  ";
            std::string badMuonResults = "Bad muon flag:              ";

            for (int wp = 0; wp < Nwp; wp++) {
                if (selectorTools[wp]->accept(*mu_itr)) {
                    selectedMuons[wp]++;
                    selectedMuonsEvent[wp]++;
                    selectedMuonsType[(*mu_itr)->muonType()][wp]++;
                    selectedMuonsAuthor[(*mu_itr)->author()][wp]++;
                    selectedMuonsTruthType[truthType][wp]++;
                    selectedMuonsEta[etaIndex][wp]++;
                    selectionResults += "pass     ";

                    if (!selectorTools[wp]->isBadMuon(**mu_itr)) {
                        selectedMuonsNotBad[wp]++;
                        selectedMuonsEventNotBad[wp]++;
                        selectedMuonsTypeNotBad[(*mu_itr)->muonType()][wp]++;
                        selectedMuonsAuthorNotBad[(*mu_itr)->author()][wp]++;
                        selectedMuonsTruthTypeNotBad[truthType][wp]++;
                        selectedMuonsEtaNotBad[etaIndex][wp]++;
                    }
                } else
                    selectionResults += "fail     ";

                if (!selectorTools[wp]->isBadMuon(**mu_itr))
                    badMuonResults += "good     ";
                else
                    badMuonResults += "bad      ";
            }

            // Print table of selection results for this muon
            Info(APP_NAME, "%s", namesString.c_str());
            Info(APP_NAME, "%s", selectionResults.c_str());
            Info(APP_NAME, "%s", badMuonResults.c_str());

        }  // done loop over muons

        // Print table of number of selected muons in this event
        std::string NselectedString = "Number of selected muons:     ";
        std::string NselectedStringNotBad = "Including bad muon veto:      ";
        for (int wp = 0; wp < Nwp; wp++) {
            NselectedString += std::to_string(selectedMuonsEvent[wp]) + "        ";
            NselectedStringNotBad += std::to_string(selectedMuonsEventNotBad[wp]) + "        ";
        }

        Info(APP_NAME, "===== Event summary:");
        Info(APP_NAME, "%s", namesString.c_str());
        Info(APP_NAME, "%s", NselectedString.c_str());
        Info(APP_NAME, "%s", NselectedStringNotBad.c_str());

        // Close with a message:
        Info(APP_NAME,
             "===>>>  done processing event #%i, "
             "run #%i %i events processed so far  <<<===",
             static_cast<int>(ei->eventNumber()), static_cast<int>(ei->runNumber()), static_cast<int>(entry + 1));

    }  // done loop over events

    // Now, let's summarize all we have found in the processed events

    Info(APP_NAME, "======================================");
    Info(APP_NAME, "========= Full run summary ===========");
    Info(APP_NAME, "======================================");

    Info(APP_NAME, "Processed %i events and %i muons", static_cast<int>(entries), allMuons);

    Info(APP_NAME, "%i positive and %i negative muons", nPositive, nNegative);

    Info(APP_NAME, "Selected muons by working point (numbers in parenthesis include bad muon veto):");
    Info(APP_NAME, "--------------------------");
    for (int wp = 0; wp < Nwp; wp++)
        Info(APP_NAME, "%s: %s %i (%i)", WPnames[wp].c_str(), padding[wp].c_str(), selectedMuons[wp], selectedMuonsNotBad[wp]);
    Info(APP_NAME, "--------------------------");

    // Make table of selected muons by type and working point
    Info(APP_NAME, "Selected muons by type and working point (numbers in parenthesis include bad muon veto):");
    Info(APP_NAME, "---------------------------------------------------------------------------------------");
    for (int l = 0; l < Nwp + 2; l++) {
        std::string line = "";
        if (l == 0) {  // line with type names
            line += "              ";
            for (int type = 0; type < Ntype; type++) line += typeNames[type] + "     ";
        } else if (l == 1) {  // line for all muons inclusive
            line += "All muons:      ";
            for (int type = 0; type < Ntype; type++) {
                std::stringstream ss;
                ss << std::left << std::setw(16) << std::to_string(allMuonsType[type]);
                line += ss.str();
            }
        } else {  // lines for each of the working points
            int wp = l - 2;
            line += WPnames[wp] + ":" + padding[wp] + "     ";
            for (int type = 0; type < Ntype; type++) {
                std::stringstream ss;
                ss << std::left << std::setw(16)
                   << (std::to_string(selectedMuonsType[type][wp]) + " (" + std::to_string(selectedMuonsTypeNotBad[type][wp]) + ")");
                line += ss.str();
            }
        }

        Info(APP_NAME, "%s", line.c_str());
    }
    Info(APP_NAME, "---------------------------------------------------------------------------------------");


    // Make table of selected muons by author and working point
    Info(APP_NAME, "Selected muons by primary author and working point (numbers in parenthesis include bad muon veto):");
    Info(APP_NAME, "---------------------------------------------------------------------------------------");
    for (int l = 0; l < Nwp + 2; l++) {
        std::string line = "";
        if (l == 0) {  // line with author names
            line += "               ";
	    for (int author = 0; author < Nauthor; author++) {

	      //Do not print unrepresented authors, since the table can be quite wide
	      if (allMuonsAuthor[author] == 0) continue;

	      std::stringstream ss;
	      ss << std::left << std::setw(16);
	      ss << authorNames[author];
	      line += ss.str();
            }
        } else if (l == 1) {  // line for all muons inclusive
            line += "All muons:      ";
            for (int author = 0; author < Nauthor; author++) {

	      if (allMuonsAuthor[author] == 0) continue;

	      std::stringstream ss;
	      ss << std::left << std::setw(16);
	      ss << std::to_string(allMuonsAuthor[author]);
	      line += ss.str();
            }
        } else {  // lines for each of the working points
            int wp = l - 2;
            line += WPnames[wp] + ":" + padding[wp] + "     ";
            for (int author = 0; author < Nauthor; author++) {

	      if (allMuonsAuthor[author] == 0) continue;

	      std::stringstream ss;
	      ss << std::left << std::setw(16);
	      ss << (std::to_string(selectedMuonsAuthor[author][wp]) + " (" + std::to_string(selectedMuonsAuthorNotBad[author][wp]) + ")");
	      line += ss.str();
            }
        }

        Info(APP_NAME, "%s", line.c_str());
    }
    Info(APP_NAME, "---------------------------------------------------------------------------------------");


    // Make table of selected muons by |eta| and working point
    Info(APP_NAME, "Selected muons by |eta| and working point (numbers in parenthesis include bad muon veto):");
    Info(APP_NAME, "---------------------------------------------------------------------------------------");
    for (int l = 0; l < Nwp + 2; l++) {
        std::string line = "";
        if (l == 0) {  // line with eta regions
            line += "              ";
            line += etaRegions;
        } else if (l == 1) {  // line for all muons inclusive
            line += "All muons:      ";
            for (int eta = 0; eta < Neta; eta++) {
                std::stringstream ss;
                ss << std::left << std::setw(20) << std::to_string(allMuonsEta[eta]);
                line += ss.str();
            }
        } else {  // lines for each of the working points
            int wp = l - 2;
            line += WPnames[wp] + ":" + padding[wp] + "     ";
            for (int eta = 0; eta < Neta; eta++) {
                std::stringstream ss;
                ss << std::left << std::setw(20)
                   << (std::to_string(selectedMuonsEta[eta][wp]) + " (" + std::to_string(selectedMuonsEtaNotBad[eta][wp]) + ")");
                line += ss.str();
            }
        }

        Info(APP_NAME, "%s", line.c_str());
    }
    Info(APP_NAME, "---------------------------------------------------------------------------------------");

    // Make table of selected muons by truth type and working point
    if (isMC) {
        Info(APP_NAME, "Selected muons by truth classification and working point (numbers in parenthesis include bad muon veto):");
        Info(APP_NAME, "---------------------------------------------------------------------------------------");
        for (int l = 0; l < Nwp + 2; l++) {
            std::string line = "";
            if (l == 0) {  // line with truth classification labels
                line += "              ";
                for (int truthType = 0; truthType < NtruthType; truthType++) line += truthTypeNames[truthType] + "        ";
            } else if (l == 1) {  // line for all muons inclusive
                line += "All muons:      ";
                for (int truthType = 0; truthType < NtruthType; truthType++) {
                    std::stringstream ss;
                    ss << std::left << std::setw(16) << std::to_string(allMuonsTruthType[truthType]);
                    line += ss.str();
                }
            } else {  // lines for each of the working points
                int wp = l - 2;
                line += WPnames[wp] + ":" + padding[wp] + "     ";
                for (int truthType = 0; truthType < NtruthType; truthType++) {
                    std::stringstream ss;
                    ss << std::left << std::setw(16)
                       << (std::to_string(selectedMuonsTruthType[truthType][wp]) + " (" +
                           std::to_string(selectedMuonsTruthTypeNotBad[truthType][wp]) + ")");
                    line += ss.str();
                }
            }

            Info(APP_NAME, "%s", line.c_str());
        }
        Info(APP_NAME, "---------------------------------------------------------------------------------------");
    }

    // Needed for Smart Slimming
    xAOD::IOStats::instance().stats().printSmartSlimmingBranchList();

    // Return gracefully:
    return 0;
}
