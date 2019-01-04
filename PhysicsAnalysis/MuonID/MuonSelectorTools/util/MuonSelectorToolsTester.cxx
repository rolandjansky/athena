/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/// a simple testing macro for the MuonSelectorTools_xAOD package
/// shamelessly stolen from CPToolTests.cxx

// System include(s):
#include <memory>
#include <cstdlib>
#include <string>
#include <map>
#include <iomanip>

// ROOT include(s):
#include <TFile.h>
#include <TError.h>
#include <TStopwatch.h>
#include <TString.h>

// Infrastructure include(s):
#ifdef ROOTCORE
#   include "xAODRootAccess/Init.h"
#   include "xAODRootAccess/TEvent.h"
#endif // ROOTCORE

// EDM include(s):
#include "xAODEventInfo/EventInfo.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODTracking/TrackingPrimitives.h"
#include "xAODTracking/TrackParticleContainer.h"

// Local include(s):
#include "MuonSelectorTools/errorcheck.h"
#include "MuonSelectorTools/MuonSelectionTool.h"

// Needed for Smart Slimming
#include "xAODCore/tools/IOStats.h"
#include "xAODCore/tools/ReadStats.h"

/// Example of how to run the MuonSelectorTools package to obtain information from muons

int main( int argc, char* argv[] ) {


   // The application's name:
   const char* APP_NAME = argv[ 0 ];
   
   // Check if we received a file name:
   if( argc < 2 ) {
      Error( APP_NAME, "No file name received!" );
      Error( APP_NAME, "  Usage: %s [xAOD file name] [Nevts to process]", APP_NAME );
      return 1;
   }
   
   // Initialise the application:
   CHECK( xAOD::Init( APP_NAME ) );

   // Open the input file:
   const TString fileName = argv[ 1 ];
   Info( APP_NAME, "Opening file: %s", fileName.Data() );
   std::auto_ptr< TFile > ifile( TFile::Open( fileName, "READ" ) );
   CHECK( ifile.get() );

   // Create a TEvent object:
   xAOD::TEvent event;
   CHECK(event.readFrom( ifile.get(), xAOD::TEvent::kClassAccess ));
   Info( APP_NAME, "Number of events in the file: %i",
         static_cast< int >( event.getEntries() ) );

   // Decide how many events to run over:
   Long64_t entries = event.getEntries();
   if( argc > 2 ) {
      const Long64_t e = atoll( argv[ 2 ] );
      if( e < entries ) {
         entries = e;
      }
   }

   // Create a TStore object
   xAOD::TStore store;
   
   // Create a set of selector tools configured for each of the available working points

   std::vector<std::unique_ptr<CP::MuonSelectionTool> > selectorTools;
   selectorTools.clear();

   const int Nwp = 6; // number of working points (tool instances)

   std::vector<std::string> WPnames = {"Tight", "Medium", "Loose", "VeryLoose", "HighPt", "LowPt"};

   for (int wp = 0; wp < Nwp; wp++) {

     Info( APP_NAME, "Creating selector tool for working point: %s ...", WPnames[wp].c_str() );

     CP::MuonSelectionTool* muonSelection = new CP::MuonSelectionTool( "MuonSelection_"+WPnames[wp] );

     muonSelection->msg().setLevel( MSG::INFO );
     CHECK (muonSelection->setProperty( "MaxEta", 2.7 ));
     CHECK (muonSelection->setProperty( "MuQuality", wp));
     CHECK (muonSelection->setProperty( "TurnOffMomCorr", true ));
     CHECK (muonSelection->initialize());

     selectorTools.emplace_back(muonSelection);
   }

   //done setting up selector tools

   //Create "padding" strings to facilitate easy table view of results
   std::vector<std::string> padding;
   padding.clear();

   unsigned int maxNameLength = 0;
   for (int wp = 0; wp < Nwp; wp++)
     if (WPnames[wp].size() > maxNameLength)
       maxNameLength = WPnames[wp].size();

   for (int wp = 0; wp < Nwp; wp++) {

     std::string pad = "";
     for (unsigned int i = 0; i < maxNameLength - WPnames[wp].size(); i++)
       pad += " ";

     padding.push_back(pad);
   }

   //string with names of working points for selection results display
   std::string namesString = "                           ";
   for (int wp = 0; wp < Nwp; wp++) 
     namesString += WPnames[wp] + "   ";

   //Muon counters
   int allMuons = 0;
   int nPositive = 0;
   int nNegative = 0;

   //Summary information - how many muons passed each working point (with and without vetoing bad muons)
   int selectedMuons[Nwp];
   for (int wp = 0; wp < Nwp; wp++)
     selectedMuons[wp] = 0;

   int selectedMuonsNotBad[Nwp];
   for (int wp = 0; wp < Nwp; wp++)
     selectedMuonsNotBad[wp] = 0;

   //Obtain summary information also split by muon type
   const int Ntype = 5;

   std::string typeNames[Ntype];
   for (int type = 0; type < Ntype; type++) {
     
     if(type == xAOD::Muon::Combined)
       typeNames[type] = "combined";
     else if(type == xAOD::Muon::MuonStandAlone)
       typeNames[type] = "stand-alone";
     else if(type == xAOD::Muon::SegmentTagged)
       typeNames[type] = "segment-tagged";
     else if(type == xAOD::Muon::CaloTagged)
       typeNames[type] = "calo-tagged";
     else if(type == xAOD::Muon::SiliconAssociatedForwardMuon)
       typeNames[type] = "forward";
     else
       typeNames[type] = "unknown";
   }


   //Muon counters for each type
   int allMuonsType[Ntype];
   for (int type = 0; type < Ntype; type++)
     allMuonsType[type] = 0;

   //Muon counters for muons of each type passing each working point
   int selectedMuonsType[Ntype][Nwp];
   for (int type = 0; type < Ntype; type++)
     for (int wp = 0; wp < Nwp; wp++)
       selectedMuonsType[type][wp] = 0;

   int selectedMuonsTypeNotBad[Ntype][Nwp];
   for (int type = 0; type < Ntype; type++)
     for (int wp = 0; wp < Nwp; wp++)
       selectedMuonsTypeNotBad[type][wp] = 0;



   // Loop over the events:
   for( Long64_t entry = 0; entry < entries; ++entry ) {

      // Tell the object which entry to look at:
      event.getEntry( entry );

      //Counters for selected muons within each event
      int selectedMuonsEvent[Nwp];
      for (int wp = 0; wp < Nwp; wp++)
	selectedMuonsEvent[wp] = 0;

      int selectedMuonsEventNotBad[Nwp];
      for (int wp = 0; wp < Nwp; wp++)
	selectedMuonsEventNotBad[wp] = 0;

      // Print some event information for fun:
      const xAOD::EventInfo* ei = 0;
      CHECK( event.retrieve( ei, "EventInfo" ) );
      Info( APP_NAME,
            "===>>>  start processing event #%i, "
            "run #%i %i events processed so far  <<<===",
            static_cast< int >( ei->eventNumber() ),
            static_cast< int >( ei->runNumber() ),
            static_cast< int >( entry ) );

      // Get the Muons from the event:
      const xAOD::MuonContainer* muons = 0;
      CHECK( event.retrieve( muons, "Muons" ) );
      Info( APP_NAME, "Number of muons: %i",
            static_cast< int >( muons->size() ) );


      xAOD::Muon::Quality my_quality;
      bool passesIDRequirements;
      bool passesPreselectionCuts;

      int muCounter = 0; //muon counter for each event
      
      // Print their properties, using the tools:
      xAOD::MuonContainer::const_iterator mu_itr = muons->begin();
      xAOD::MuonContainer::const_iterator mu_end = muons->end();
      for( ; mu_itr != mu_end; ++mu_itr ) {


	allMuons++;
	allMuonsType[(*mu_itr)->muonType()]++;
	muCounter++;

	Info( APP_NAME, "===== Muon number: %i",
	      static_cast< int >( muCounter ) );


	if((*mu_itr)->charge() > 0)
	  nPositive++;
	else
	  nNegative++;


        passesIDRequirements = selectorTools[0]->passedIDCuts(**mu_itr);
        passesPreselectionCuts = selectorTools[0]->passedMuonCuts(**mu_itr);
	my_quality = selectorTools[0]->getQuality(**mu_itr);	

	//Print some general information about the muon
	Info( APP_NAME, "Muon pT [GeV]:       %g ", std::abs((*mu_itr)->pt())/1000.);
	Info( APP_NAME, "Muon eta, phi:       %g, %g ", (*mu_itr)->eta(),(*mu_itr)->phi());
	Info( APP_NAME, "Muon muonType:       %d (%s)", (*mu_itr)->muonType(), typeNames[(*mu_itr)->muonType()].c_str());

	// Info( APP_NAME, "Muon charge:         %g ", (*mu_itr)->charge());
	// Info( APP_NAME, "Muon allAuthors:     %d ", (*mu_itr)->allAuthors());
	// Info( APP_NAME, "Muon author:         %d ", (*mu_itr)->author());

	Info( APP_NAME, "Muon quality (from tool, from xAOD):      %d, %d", my_quality, (*mu_itr)->quality());
	Info( APP_NAME, "Muon passes cuts (ID hits, preselection): %d, %d", passesIDRequirements, passesPreselectionCuts);


	//Check availability of variables and dump a message if they are missing
	uint8_t PixHits = 0, PixDead = 0, SCTHits = 0, SCTDead = 0, PixHoles = 0, SCTHoles = 0, TRTHits = 0, TRTOut = 0;
	uint8_t nprecisionLayers = 0, nprecisionHoleLayers = 0, cscUnspoiledEtaHits = 0;

	if (!(*mu_itr)->primaryTrackParticle()->summaryValue(nprecisionLayers, xAOD::numberOfPrecisionLayers))
	   Info( APP_NAME, "no nprecisionlayers! ");
	
	if (!(*mu_itr)->primaryTrackParticle()->summaryValue(nprecisionHoleLayers, xAOD::numberOfPrecisionHoleLayers))
	  Info( APP_NAME, "no nprecisionholelayers! ");
	
	if (!(*mu_itr)->summaryValue(cscUnspoiledEtaHits, xAOD::MuonSummaryType::cscUnspoiledEtaHits))
	  Info( APP_NAME, "no cscUnspoiledEtaHits! ");

	if(!(*mu_itr)->primaryTrackParticle()->summaryValue(PixHits,xAOD::numberOfPixelHits))
	  Info( APP_NAME, "Missing info!");

	if(!(*mu_itr)->primaryTrackParticle()->summaryValue(PixDead,xAOD::numberOfPixelDeadSensors))
	  Info( APP_NAME, "Missing info!");

	if(!(*mu_itr)->primaryTrackParticle()->summaryValue(SCTHits,xAOD::numberOfSCTHits))
	  Info( APP_NAME, "Missing info!");

	if(!(*mu_itr)->primaryTrackParticle()->summaryValue(SCTDead,xAOD::numberOfSCTDeadSensors))
	  Info( APP_NAME, "Missing info!");

	if(!(*mu_itr)->primaryTrackParticle()->summaryValue(PixHoles,xAOD::numberOfPixelHoles))
	  Info( APP_NAME, "Missing info!");

	if(!(*mu_itr)->primaryTrackParticle()->summaryValue(SCTHoles,xAOD::numberOfSCTHoles))
	  Info( APP_NAME, "Missing info!");

	if(!(*mu_itr)->primaryTrackParticle()->summaryValue(TRTHits,xAOD::numberOfTRTHits))
	  Info( APP_NAME, "Missing info!");

	if(!(*mu_itr)->primaryTrackParticle()->summaryValue(TRTOut,xAOD::numberOfTRTOutliers))
	  Info( APP_NAME, "Missing info!");


	//Now, let's check whether the muon passes the different working points and also whether it is flagged as bad

	std::string selectionResults = "Muon selection acceptance:  ";
	std::string badMuonResults =   "Bad muon flag:              ";

	for (int wp = 0; wp < Nwp; wp++) {

	  if (selectorTools[wp]->accept(*mu_itr)) {
	    selectedMuons[wp]++;
	    selectedMuonsEvent[wp]++;
	    selectedMuonsType[(*mu_itr)->muonType()][wp]++;
	    selectionResults += "pass     ";

	    if (!selectorTools[wp]->isBadMuon(**mu_itr)) {
	      selectedMuonsNotBad[wp]++;
	      selectedMuonsEventNotBad[wp]++;
	      selectedMuonsTypeNotBad[(*mu_itr)->muonType()][wp]++;
	    }
	  }
	  else
	    selectionResults += "fail     ";

	  if (!selectorTools[wp]->isBadMuon(**mu_itr))
	    badMuonResults += "good     ";
	  else
	    badMuonResults += "bad      ";
	}

	//Print table of selection results for this muon
	Info( APP_NAME, "%s", namesString.c_str() );
	Info( APP_NAME, "%s", selectionResults.c_str() );
	Info( APP_NAME, "%s", badMuonResults.c_str() );

      } //done loop over muons


      //Print table of number of selected muons in this event
      std::string NselectedString = "Number of selected muons:     ";
      std::string NselectedStringNotBad = "Including bad muon veto:      ";
      for (int wp = 0; wp < Nwp; wp++) {
	NselectedString += std::to_string(selectedMuonsEvent[wp]) + "        ";
	NselectedStringNotBad += std::to_string(selectedMuonsEventNotBad[wp]) + "        ";
      }

      Info( APP_NAME, "===== Event summary:");
      Info( APP_NAME, "%s", namesString.c_str() );
      Info( APP_NAME, "%s", NselectedString.c_str() );
      Info( APP_NAME, "%s", NselectedStringNotBad.c_str() );

      // Close with a message:
      Info( APP_NAME,
            "===>>>  done processing event #%i, "
            "run #%i %i events processed so far  <<<===",
            static_cast< int >( ei->eventNumber() ),
            static_cast< int >( ei->runNumber() ),
            static_cast< int >( entry + 1 ) );

   } //done loop over events


   //Now, let's summarize all we have found in the processed events

   Info(APP_NAME, "======================================");
   Info(APP_NAME, "========= Full run summary ===========");
   Info(APP_NAME, "======================================");

   Info(APP_NAME, "Processed %i events and %i muons" , static_cast< int >(entries), allMuons);

   Info(APP_NAME, "%i positive and %i negative muons" , nPositive, nNegative);

   Info(APP_NAME, "Selected muons by working point (numbers in parenthesis include bad muon veto):");
   Info(APP_NAME, "--------------------------");
   for (int wp = 0; wp < Nwp; wp++)
     Info(APP_NAME, "%s: %s %i (%i)", WPnames[wp].c_str(), padding[wp].c_str(), selectedMuons[wp], selectedMuonsNotBad[wp]);
   Info(APP_NAME, "--------------------------");

   //Make table of selected muons by type and working point
   Info(APP_NAME, "Selected muons by type and working point (numbers in parenthesis include bad muon veto):");
   Info(APP_NAME, "---------------------------------------------------------------------------------------");
   for (int l = 0; l < Nwp+2; l++) {
     
     std::string line = "";
     if (l == 0) { //line with type names
       line += "              ";
       for (int type = 0; type < Ntype; type++)
	 line += typeNames[type] + "     ";
     }
     else if (l == 1) { //line for all muons inclusive
       line += "All muons:      ";
       for (int type = 0; type < Ntype; type++) {
	 std::stringstream ss;
	 ss << std::left << std::setw(16) << std::to_string(allMuonsType[type]);
	 line += ss.str();
       }
     }
     else { //lines for each of the working points
       int wp = l - 2;
       line += WPnames[wp] + ":" + padding[wp] + "     ";
       for (int type = 0; type < Ntype; type++) {
	 std::stringstream ss;
	 ss << std::left << std::setw(16) << (std::to_string(selectedMuonsType[type][wp]) + " (" + std::to_string(selectedMuonsTypeNotBad[type][wp]) + ")");
	 line += ss.str();
       }
     }

     Info(APP_NAME, "%s", line.c_str());
   }
   Info(APP_NAME, "---------------------------------------------------------------------------------------");

   // Needed for Smart Slimming
   xAOD::IOStats::instance().stats().printSmartSlimmingBranchList();

   // Return gracefully:
   return 0;
}
