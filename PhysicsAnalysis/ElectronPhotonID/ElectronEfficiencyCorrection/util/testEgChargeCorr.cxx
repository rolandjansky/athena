/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <memory>
#include <cstdlib>
#include <string>

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
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/Egamma.h"
#include "ElectronEfficiencyCorrection/ElectronChargeEfficiencyCorrectionTool.h"
#include "ElectronEfficiencyCorrection/ElectronEfficiencyHelpers.h"
#include "xAODCore/ShallowCopy.h"
#include "AsgTools/AnaToolHandle.h"


//Asg includes
#include "AsgTools/AsgMessaging.h"
#include "PATInterfaces/SystematicsUtil.h"
#include "EgammaAnalysisInterfaces/IAsgElectronLikelihoodTool.h"

#include <ElectronPhotonSelectorTools/AsgElectronLikelihoodTool.h>
#include "Messaging.h"

int main( int argc, char* argv[] ) {

  // The application's name:
  const char* APP_NAME = argv[ 0 ];

  // Check if we received a file name:
  if( argc < 3 ) {
    Error( APP_NAME, "No file name received!" );
    Error( APP_NAME, "  Usage: %s [xAOD file name] [Correction File name]", APP_NAME );
    return 1;
  }


  double SF_chargeID=0;
  int SF_nevents=0;

  double n_chargeID=0;
  double n_chargeMisID=0;

  // Initialise the application:
  CHECK( xAOD::Init( APP_NAME ) );

  // Open the input file:
  const TString fileName = argv[ 1 ];
  const TString corrFileName = argv[ 2 ];

  Info( APP_NAME, "Opening file: %s", fileName.Data() );
  std::unique_ptr< TFile > ifile( TFile::Open( fileName, "READ" ) );
  CHECK( ifile.get() );

  // Create a TEvent object:
  //xAOD::TEvent event( xAOD::TEvent::kBranchAccess );
  xAOD::TEvent event( xAOD::TEvent::kClassAccess );
  CHECK( event.readFrom( ifile.get() ) );
  Info( APP_NAME, "Number of events in the file: %i",
	static_cast< int >( event.getEntries() ) );

  std::cout << "=="<<std::endl;


  // Decide how many events to run over:
  Long64_t entries = event.getEntries();
  if( argc > 2 ) {
    const Long64_t e = atoll( argv[ 2 ] );
    if( e < entries ) {
      entries = e;
    }
  }

  //Likelihood
  MSG::Level mylevel=MSG::INFO;
  CP::ElectronChargeEfficiencyCorrectionTool myEgCorrections ("myEgCorrections");
  CHECK( myEgCorrections.setProperty("OutputLevel", mylevel) );

  std::string inputFile = corrFileName.Data();//"data/ChMisIDSF_TightLL_FixedCutTight.root" ;
  CHECK( myEgCorrections.setProperty("CorrectionFileName",inputFile) );
  CHECK( myEgCorrections.setProperty("ForceDataType",1) );

  CHECK( myEgCorrections.setProperty("DefaultRandomRunNumber", (unsigned int)311481 ) );
  CHECK( myEgCorrections.setProperty("UseRandomRunNumber",false) );
  myEgCorrections.initialize();

  asg::AnaToolHandle<IAsgElectronLikelihoodTool> electronMediumLHSelector ("AsgElectronLikelihoodTool/electronMediumLHSelector");
  CHECK(electronMediumLHSelector.setProperty("WorkingPoint", "MediumLHElectron"));
  CHECK(electronMediumLHSelector.setProperty("OutputLevel", mylevel));
  CHECK(electronMediumLHSelector.initialize());

  // Loop over the events:
  entries = 1000;

  // Get a list of systematics
  CP::SystematicSet recSysts = myEgCorrections.recommendedSystematics();
  // Convert into a simple list
  // std::vector<CP::SystematicSet> sysList = CP::make_systematics_vector(recSysts);

  // Loop over the events:
  entries = 600;

  Long64_t entry = 0;
  for( ; entry < entries; ++entry ) {

    // Tell the object which entry to look at:
    event.getEntry( entry );

    if (entry<10)     std::cout << "=================NEXT EVENT==========================" << std::endl;
    Info (APP_NAME,"Electron" );


    const xAOD::EventInfo* event_info = 0;
    CHECK( event.retrieve( event_info, "EventInfo" ) );

    const xAOD::ElectronContainer* electrons = 0;
    CHECK( event.retrieve(electrons, "Electrons") );

    // Create shallow copy for this systematic
    std::pair< xAOD::ElectronContainer*, xAOD::ShallowAuxContainer* > electrons_shallowCopy =
      xAOD::shallowCopyContainer( *electrons );

    //Iterate over the shallow copy
    xAOD::ElectronContainer* elsCorr = electrons_shallowCopy.first;
    xAOD::ElectronContainer::iterator el_it      = elsCorr->begin();
    xAOD::ElectronContainer::iterator el_it_last      = elsCorr->end();

    unsigned int i = 0;
    double SF = 0;
    for (; el_it != el_it_last; ++el_it, ++i) {

      xAOD::Electron* el = *el_it;
      if (el->pt() < 20000) continue;//skip electrons outside of recommendations

      bool LHacc = electronMediumLHSelector->accept(el);
      std::cout << "acc:  "<< LHacc << std::endl;
      if(!electronMediumLHSelector->accept(el)) continue; 
      if (fabs(el->caloCluster()->etaBE(2)) > 2.5) continue;//skip electrons outside of recommendations

      SF_nevents++;
      
      Info (APP_NAME,"Electron #%d", i);
      
      if(myEgCorrections.getEfficiencyScaleFactor(*el,SF) != CP::CorrectionCode::Ok){
	Error( APP_NAME, "Problem in getEfficiencyScaleFactor");
	return EXIT_FAILURE;
      }

      Info( APP_NAME, "===>>> SF %f ",SF);

      if(myEgCorrections.applyEfficiencyScaleFactor(*el) != CP::CorrectionCode::Ok){
	Error( APP_NAME, "Problem in applyEfficiencyScaleFactor");
	return EXIT_FAILURE;
      }

      SF_chargeID=SF_chargeID+SF;

      for(const auto& sys : recSysts){  //recSysts){
	double systematic = 0;

	// Configure the tool for this systematic
	CHECK( myEgCorrections.applySystematicVariation({sys}) );
	//
	if(myEgCorrections.getEfficiencyScaleFactor(*el,systematic) == CP::CorrectionCode::Ok){
	  // std::cout <<  (sys).name() <<"  sys names    " << (myEgCorrections.affectingSystematics().name()) << std::endl;    
	  Info( APP_NAME,  "%s: %f Result %f Systematic value %f ", sys.name().c_str(),SF,systematic,systematic-SF );
	}
      }
	    
      CHECK( myEgCorrections.applySystematicVariation(CP::SystematicSet()) );
      int truthcharge = false;
      ElectronEfficiencyHelpers::getEleTruthCharge(*el,truthcharge);

      if ( el->charge() * truthcharge < 0   )  {
	Info( APP_NAME, "===>>> MISID %f ",SF);
	n_chargeMisID++;
      }
      else
	n_chargeID++;
    }
	
  }
  Info( APP_NAME, "===>>>  done processing event #%lld ",entry);
  Info( APP_NAME, "===>>>  processed #%d electrons",SF_nevents);
  Info( APP_NAME, "===>>>  compared to #%f (from Charge MisId SF)",SF_chargeID);
  Info( APP_NAME, "===>>>  compared to #%f and #%f ",n_chargeID,n_chargeMisID);

  CHECK( myEgCorrections.finalize() );

  // Return gracefully:
  return 0;
}
