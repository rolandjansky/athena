/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <memory>
#include <cstdlib>

// ROOT include(s):
#include <TFile.h>
#include <TError.h>
#include <TString.h>

// Infrastructure include(s):
#ifdef ROOTCORE
#   include "xAODRootAccess/Init.h"
#   include "xAODRootAccess/TEvent.h"
#   include "xAODRootAccess/TStore.h"
#endif // ROOTCORE

// EDM include(s):
#include "xAODEventInfo/EventInfo.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODEgamma/Egamma.h"
#include "xAODEgamma/EgammaxAODHelpers.h"
#include "PhotonEfficiencyCorrection/AsgPhotonEfficiencyCorrectionTool.h"
#include "xAODCore/ShallowCopy.h"
#include "PathResolver/PathResolver.h"
#include <PATInterfaces/SystematicsUtil.h>

#include <iostream>
#include <string>

#define CHECK( ARG )                                  \
do {                                                  \
    const bool result = ARG;                          \
  if( ! result ) {                                    \
      ::Error( APP_NAME, "Failed to execute: \"%s\"", \
#ARG );                                               \
      return 1;                                       \
  }                                                   \
 } while( false )


int main( int argc, char* argv[] ) {

   // The application's name:
   const char* APP_NAME = argv[ 0 ];

   // Check if we received a file name:
   if( argc < 2 ) {
     Error( APP_NAME, "No file name received!" );
     Error( APP_NAME, "  Usage: %s [xAOD file name]", APP_NAME );
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
//   xAOD::TEvent event( xAOD::TEvent::kBranchAccess ); //will work for a sample produced in devval
 	xAOD::TEvent event( xAOD::TEvent::kClassAccess );   
   CHECK( event.readFrom( ifile.get() ) );
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

	// Initialize photonFS tool
   AsgPhotonEfficiencyCorrectionTool photonSF_ID("AsgPhotonEfficiencyCorrectionTool_idSF");
   AsgPhotonEfficiencyCorrectionTool photonSF_Iso("AsgPhotonEfficiencyCorrectionTool_isoSF");
  
   //  photonSF_ID.msg().setLevel( MSG::DEBUG ); 


   //Set Properties for photonID_SF tool
   CHECK(photonSF_ID.setProperty("MapFilePath","PhotonEfficiencyCorrection/map0.txt"));
   CHECK(photonSF_ID.setProperty("ForceDataType",1)); 
   
   //Set Properties for photonISO_SF tool
   CHECK(photonSF_Iso.setProperty("MapFilePath","PhotonEfficiencyCorrection/map0.txt"));
   CHECK(photonSF_Iso.setProperty("IsoWP","Loose"));	// Set isolation WP: Loose,Tight,TightCaloOnly
   CHECK(photonSF_Iso.setProperty("Threshold_lowPT",25.0));	// this is a default value, no need to set if using it
   CHECK(photonSF_Iso.setProperty("Threshold_highPT",100.0));	// this is a default value, no need to set if using it
   CHECK(photonSF_Iso.setProperty("UseRadiativeZSF_mediumPT",false));	// default=false, set to true to use RadZSF up to Threshold_highPT
   CHECK(photonSF_Iso.setProperty("ForceDataType",1)); //set data type: 1 for FULLSIM, 3 for AF2
   
   
   
   if(!photonSF_ID.initialize()){
     std::cout <<"Failed to initialize the tool, check for errors"<<std::endl;
     return 0;
   }
   if(!photonSF_Iso.initialize()){
     std::cout <<"Failed to initialize the tool, check for errors"<<std::endl;
     return 0;
   }
   
   // Test that recommended systematics properly bieng registered:
   std::vector<CP::SystematicSet> sysList;
   const CP::SystematicRegistry& registry = CP::SystematicRegistry::getInstance();
   const CP::SystematicSet& recommendedSystematics = registry.recommendedSystematics();
   sysList = CP::make_systematics_vector(recommendedSystematics); // replaces continuous systematics with the +/-1 sigma variation
   std::cout << "List of recommended systematics from the registry:"<<std::endl;
   for (auto sysListItr = recommendedSystematics.begin(); sysListItr != recommendedSystematics.end(); ++sysListItr){
     std::cout <<(*sysListItr).name()<<std::endl;
   }
   
   // restructure all recommended systematic variations for the SF tool
   // for +/- nsigma variation see https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/PhotonEfficiencyCorrection#Systematic_variations
   std::cout << "restructure all recommended systematic variations for the SF tool"<<std::endl;
   std::vector<CP::SystematicSet> syst_PhotonID, syst_PhotonIso;
   for (auto SystematicsVariation : CP::make_systematics_vector(photonSF_ID.recommendedSystematics()))
   {
     syst_PhotonID.push_back(CP::SystematicSet());
   	 syst_PhotonID.back().insert(SystematicsVariation);
   }
   for (auto SystematicsVariation : CP::make_systematics_vector(photonSF_Iso.recommendedSystematics()))
   {
     syst_PhotonIso.push_back(CP::SystematicSet());
   	 syst_PhotonIso.back().insert(SystematicsVariation);
   }

   //Print all recomended systemtaics
   for (auto sSystematicSet: syst_PhotonID){
	  Info(APP_NAME,"PhotonEfficiencyCorrectionTool ID instance has next systematic variation  %s ",sSystematicSet.name().c_str());
   }
   for (auto sSystematicSet: syst_PhotonIso){
	  Info(APP_NAME,"PhotonEfficiencyCorrectionTool Iso instance has next systematic variation  %s ",sSystematicSet.name().c_str());
   }   

   double efficiencyScaleFactor=0, efficiencyScaleFactorError=0;
   // Loop over the events:
   std::cout << "loop on " << entries << " entries"<<std::endl;
   for( int entry = 0; entry < entries; ++entry ) {

     // Tell the object which entry to look at:
     event.getEntry( entry );   
	
	// Get the EventInfo (run number...):
	const xAOD::EventInfo* ei = 0;
	CHECK( event.retrieve( ei, "EventInfo" ) );
	//std::cout << "Event number = " << ei->eventNumber() << std::endl;
	
	// Get the Photon container from the event:
    const xAOD::PhotonContainer *photons = 0;
	CHECK( event.retrieve( photons, "Photons" ) );
	
	//Clone 
	std::pair< xAOD::PhotonContainer*, xAOD::ShallowAuxContainer* > photons_shallowCopy = xAOD::shallowCopyContainer( *photons );

	//Iterate over the shallow copy
    xAOD::PhotonContainer* phsCorr = photons_shallowCopy.first;
    xAOD::PhotonContainer::iterator ph_itr      = phsCorr->begin();
    xAOD::PhotonContainer::iterator ph_end      = phsCorr->end();
	
    unsigned int i = 0;
	for( ; ph_itr != ph_end; ++ph_itr, ++i ) {
	  xAOD::Photon* ph = *ph_itr;
	  
	  // skip photons with pt outsize the acceptance
	  if(ph->pt()<10000.0) continue;
	  if( fabs(ph->eta())>2.47) continue;
	   Info (APP_NAME,"Event #%d, Photon #%d", entry, i); 
	   Info (APP_NAME,"xAOD/raw pt = %f, eta = %f ", ph->pt(), ph->eta() ); 
	   
	   // will set the systematic variation to nominal - no need to do it if applySystematicVariation("UP/DOWN") is not called
	   CHECK(photonSF_ID.applySystematicVariation(syst_PhotonID.at(0)));
	   CHECK(photonSF_Iso.applySystematicVariation(syst_PhotonIso.at(0)));
   
	   // Get photon ID SF and the uncertainty
	   CHECK(photonSF_ID.getEfficiencyScaleFactor(*ph,efficiencyScaleFactor));
	   CHECK(photonSF_ID.getEfficiencyScaleFactorError(*ph,efficiencyScaleFactorError));
	   
       Info( APP_NAME,"===>>> Result ID: ScaleFactor %f, TotalUncertainty %f ",efficiencyScaleFactor,efficiencyScaleFactorError);

	   // Get photon SF and the uncertainty
	   CHECK(photonSF_Iso.getEfficiencyScaleFactor(*ph,efficiencyScaleFactor));
	   CHECK(photonSF_Iso.getEfficiencyScaleFactorError(*ph,efficiencyScaleFactorError));
	   
       Info( APP_NAME,"===>>> Result Iso: ScaleFactor %f, TotalUncertainty %f ",efficiencyScaleFactor,efficiencyScaleFactorError);

	   
	   // decorate photon (for different name use photonSF_ID.setProperty("ResultName","ID_"); or photonSF_Iso.setProperty("ResultName","Iso_");)
	   CHECK(photonSF_ID.applyEfficiencyScaleFactor(*ph));
       Info( "applyEfficiencyScaleFactor()","===>>> new decoration: (xAOD::Photon*)ph->auxdata<float>(\"SF\")=%f",ph->auxdata<float>("SF"));
	   CHECK(photonSF_Iso.applyEfficiencyScaleFactor(*ph));
       Info( "applyEfficiencyScaleFactor()","===>>> new decoration: (xAOD::Photon*)ph->auxdata<float>(\"SF\")=%f",ph->auxdata<float>("SF"));

	   
	   // get SF for all recommended systematic variations (nominal is also included):
	   for (const auto sSystematicSet: syst_PhotonID){
		CHECK(photonSF_ID.applySystematicVariation(sSystematicSet));
	    CHECK(photonSF_ID.getEfficiencyScaleFactor(*ph,efficiencyScaleFactor));
		Info( APP_NAME,"===>>> apply %s: ScaleFactor = %f",photonSF_ID.appliedSystematics().name().c_str(),efficiencyScaleFactor);
       }
	   for (const auto sSystematicSet: syst_PhotonIso){
		CHECK(photonSF_Iso.applySystematicVariation(sSystematicSet));
	    CHECK(photonSF_Iso.getEfficiencyScaleFactor(*ph,efficiencyScaleFactor));
		Info( APP_NAME,"===>>> apply %s: ScaleFactor = %f",photonSF_Iso.appliedSystematics().name().c_str(),efficiencyScaleFactor);
       }	   

	}  // END LOOP ON PHOTONS
     
   } // END LOOP ON EVENTS
   CHECK(photonSF_ID.finalize());
   CHECK(photonSF_Iso.finalize());
   
   // Return gracefully:
   return 1;
   
} // END PROGRAM

/*
	if(argc!=2){
		printf("input parameters:\nTestxAODPhotonTool [path]\n");
		printf("example: TestxAODPhotonTool /afs/cern.ch/work/k/krasznaa/public/xAOD/19.0.X_rel_4/mc12_8TeV.105200.McAtNloJimmy_CT10_ttbar_LeptonFilter.AOD.19.0.X_rel_4.pool.root\n");
		return 0;
	}	
*/
