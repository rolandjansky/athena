/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
#include "xAODCore/ShallowCopy.h"

//Asg includes
#include "AsgTools/AsgMessaging.h"
#include "PATInterfaces/SystematicsUtil.h"

#include <ElectronPhotonSelectorTools/AsgElectronLikelihoodTool.h>

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
   if( argc < 3 ) {
     Error( APP_NAME, "No file name received!" );
     Error( APP_NAME, "  Usage: %s [xAOD file name] [Correction File name]", APP_NAME );
      return 1;
   }

   double SF_chargeID=0;
   //   double SF_chargeMisID=0;
   int SF_nevents=0;

   double n_chargeID=0;
   double n_chargeMisID=0;

   // Initialise the application:
   CHECK( xAOD::Init( APP_NAME ) );

   // Set message level
  MSG::Level mylevel=MSG::DEBUG;
  //this.msg().setLevel(mylevel);

  // Open the input file:
  const TString fileName = argv[ 1 ];
  const TString corrFileName = argv[ 2 ];
 
 Info( APP_NAME, "Opening file: %s", fileName.Data() );
  std::auto_ptr< TFile > ifile( TFile::Open( fileName, "READ" ) );
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

  CP::ElectronChargeEfficiencyCorrectionTool myEgCorrections ("myEgCorrections");
  CHECK( myEgCorrections.setProperty("OutputLevel", mylevel) );

  std::string inputFile = corrFileName.Data();//"data/ChMisIDSF_TightLL_FixedCutTight.root" ;
  CHECK( myEgCorrections.setProperty("CorrectionFileName",inputFile) );
  CHECK( myEgCorrections.setProperty("ForceDataType",1) );

  CHECK( myEgCorrections.setProperty("DefaultRandomRunNumber", (unsigned int)311481 ) );
  CHECK( myEgCorrections.setProperty("UseRandomRunNumber",false) );
  myEgCorrections.initialize();

  AsgElectronLikelihoodTool * m_LHToolTight = new AsgElectronLikelihoodTool("m_LHToolTight");
  CHECK (m_LHToolTight->setProperty("primaryVertexContainer","PrimaryVertices") );
  m_LHToolTight->setProperty("ConfigFile","ElectronPhotonSelectorTools/offline/mc15_20160512/ElectronLikelihoodLooseOfflineConfig2016_CutBL_Smooth.conf").ignore();
  m_LHToolTight->initialize();


  // Get a list of systematics
  CP::SystematicSet recSysts = myEgCorrections.recommendedSystematics();
  // Convert into a simple list
 // std::vector<CP::SystematicSet> sysList = CP::make_systematics_vector(recSysts);
  std::cout << "=="<<std::endl;

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

    // Loop over systematics
    //   for(const auto& sys : sysList){

    //   if (entry<10)      Info(APP_NAME, "Processing syst: %s", sys.name().c_str());

    // Configure the tool for this systematic
    //        CHECK( myEgCorrections.applySystematicVariation(sys) );
    //   Info(APP_NAME, "Applied syst: %s",
    //       myEgCorrections.affectingSystematics().name().c_str());

    // Create shallow copy for this systematic
    std::pair< xAOD::ElectronContainer*, xAOD::ShallowAuxContainer* > electrons_shallowCopy =
    xAOD::shallowCopyContainer( *electrons );

    //Iterate over the shallow copy
    xAOD::ElectronContainer* elsCorr = electrons_shallowCopy.first;
    xAOD::ElectronContainer::iterator el_it      = elsCorr->begin();
    xAOD::ElectronContainer::iterator el_it_last      = elsCorr->end();

    //double SF_chargeID=0;
    //double SF_chargeMisID=0;
    //       int SF_nevents=0;

    unsigned int i = 0;
    double SF = 0;
    for (; el_it != el_it_last; ++el_it, ++i) {

      xAOD::Electron* el = *el_it;
      if (el->pt() < 25000) continue;//skip electrons outside of recommendations

bool LHacc = m_LHToolTight->accept(el);
std::cout << "acc:  "<< LHacc << std::endl;
      if(!m_LHToolTight->accept(el)) continue; 
      if (fabs(el->caloCluster()->etaBE(2)) > 2.4) continue;//skip electrons outside of recommendations
//      if(!m_LHToolTight->accept(el)) continue;

      SF_nevents++;

      // std::cout << "Electron " << i << std::endl;
      std::cout << "xAOD/raw pt = " << el->pt() << ", eta: "
                <<abs(el->eta()) <<" , "  << el->caloCluster()->etaBE(2) << std::endl;

      Info (APP_NAME,"Electron #%d", i);

      if(myEgCorrections.getEfficiencyScaleFactor(*el,SF) != CP::CorrectionCode::Ok){
        Error( APP_NAME, "Problem in getEfficiencyScaleFactor");
        return EXIT_FAILURE;
      }

      if(myEgCorrections.applyEfficiencyScaleFactor(*el) != CP::CorrectionCode::Ok){
        Error( APP_NAME, "Problem in applyEfficiencyScaleFactor");
        return EXIT_FAILURE;
      }

//      Info( APP_NAME, "===>>> Resulting SF (from get function) %f, (from apply function) %f",
 //           SF, el->auxdata< float >("SF"));

      SF_chargeID=SF_chargeID+SF;



     for(const auto& sys : recSysts){  //recSysts){
       double systematic = 0;

    // Configure the tool for this systematic
    CHECK( myEgCorrections.applySystematicVariation({sys}) );
    //
    if(myEgCorrections.getEfficiencyScaleFactor(*el,systematic) == CP::CorrectionCode::Ok){
std::cout <<  (sys).name() <<"  sys names    " << (myEgCorrections.affectingSystematics().name()) << std::endl;    
Info( APP_NAME,  "%f Result %f Systematic value %f ", SF,systematic,systematic-SF );
   //                     unc.push_back(systematic);
    //
}
}

      int truthcharge = (-1)*el->auxdata<int>("firstEgMotherPdgId");
      if (el->auxdata<int>("truthType")) { std::cout << el->charge() << "  " << el->auxdata<int>("firstEgMotherPdgId") << std::endl; }
      if ( el->charge() * truthcharge < 0   )  {
        Info( APP_NAME, "===>>> MISID %f ",SF);
        n_chargeMisID++;
      }
      else n_chargeID++;
    }

    // }

  }
  Info( APP_NAME, "===>>>  done processing event #%lld ",entry);
  Info( APP_NAME, "===>>>  processed #%d electrons",SF_nevents);
  Info( APP_NAME, "===>>>  compared to #%f (from Charge MisId SF)",SF_chargeID);
  Info( APP_NAME, "===>>>  compared to #%f and #%f ",n_chargeID,n_chargeMisID);



  CHECK( myEgCorrections.finalize() );

  // Return gracefully:
  return 0;
}
