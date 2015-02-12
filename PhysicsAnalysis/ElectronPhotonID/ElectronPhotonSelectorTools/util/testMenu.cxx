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
#include "xAODEgamma/ElectronContainer.h" 
#include "xAODEgamma/Egamma.h"

#include "ElectronPhotonSelectorTools/AsgElectronMultiLeptonSelector.h"
#include "ElectronPhotonSelectorTools/AsgElectronIsEMSelector.h"
#include "ElectronPhotonSelectorTools/AsgElectronLikelihoodTool.h"

#include "PathResolver/PathResolver.h"

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
   xAOD::TEvent event( xAOD::TEvent::kBranchAccess );
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

   // Create a transient object store. Needed for the tools.
   //   xAOD::TStore *m_store;


   std::string confDir = "ElectronPhotonSelectorTools/offline/dc14b_20150121/";

   //MultiLepton
   AsgElectronMultiLeptonSelector myMultiLepton ("myMultiLepton");
   myMultiLepton.initialize();

   //Likelihood
   AsgElectronLikelihoodTool myLikelihood ("myLikelihood");
   myLikelihood.setProperty("ConfigFile",confDir+"ElectronLikelihoodTightOfflineConfig2015.conf");
   myLikelihood.initialize();

   //IsEM
   AsgElectronIsEMSelector myLoose ("myLoose"); 
   myLoose.setProperty("ConfigFile",confDir+"ElectronIsEMLooseSelectorCutDefs.conf");
   myLoose.setProperty("isEMMask",static_cast<unsigned int> (egammaPID::ElectronLoosePP) );
   myLoose.initialize();
   myLoose.msg().setLevel(MSG::DEBUG); //set MSG LEVEL
   //IsEM no cut 
   AsgElectronIsEMSelector nocut ("nocut"); 
   nocut.setProperty("isEMMask",static_cast<unsigned int> (0) );
   nocut.initialize();

   ///=========================================================================
   //IsEM here I am an expert, added as an example of what we can do already...
   AsgElectronIsEMSelector expert ("expert"); 
   expert.setProperty("ConfigFile",confDir+"ElectronIsEMLooseSelectorCutDefs.conf");
   expert.setProperty("isEMMask",static_cast<unsigned int> (egammaPID::ElectronLoosePP) );
   expert.initialize();
   std::cout<<"What I was :  "  << expert.getOperatingPointName( )<<std::endl;
   //So far nothing dramatic I just got a std loose

   //But the setProperty (via declareProperty) talks directly to the pimpl ;-)
   expert.setProperty("isEMMask",static_cast<unsigned int> (egammaPID::ElectronMediumPP));
   std::cout<< "Hey just changed that to something else  " << expert.getOperatingPointName( )<<std::endl;
   //Now I am still using Loose cut values due to the conf but medium mask


   //This could be also cut values
   std::vector<int> expertcutpixel = {2, 2, 2, 2, 2, 2, 2, 2, 2, 2};
   expert.setProperty("CutPi",expertcutpixel);
   std::vector<int> whatpixel = *(expert.getProperty<std::vector<int> >("CutPi"));

   std::cout<<"There are my pixel cuts now, remember I connect to  m_rootTool->CutPi  "; 
   for (unsigned int i =0; i <whatpixel.size(); ++i){
     std::cout<<whatpixel.at(i)<< " , ";
   }
   std::cout<<std::endl;
   
   //BUT IT CANNOT BE A DIFFERENT CONF file, it will work, 
   //but without init will not pick up the new cut values
   //The reason is that the conf was designed to get picked up in init


   // Loop over the events:
   for( Long64_t entry = 0; entry < entries; ++entry ) {
     
     // Tell the object which entry to look at:
     event.getEntry( entry );
     
     std::cout << "=================NEXT EVENT==========================" << std::endl;
     
     const xAOD::ElectronContainer* electrons = 0 ;  
     CHECK( event.retrieve(electrons, "Electrons") );
     
     if ( !event.retrieve( electrons, "Electrons" ).isSuccess() ){ // retrieve arguments: container type, container key
       Error("execute()", "Failed to retrieve El container. Exiting." );
     }

     xAOD::ElectronContainer::const_iterator el_it      = electrons->begin(); 
     xAOD::ElectronContainer::const_iterator el_it_last = electrons->end(); 
     unsigned int i = 0; 
     std::cout << "TEST " << std::endl; 
     
     for (; el_it != el_it_last; ++el_it, ++i) {
       const xAOD::Electron* el = (*el_it); 
       std::cout << "Electron " << el << " Num " << i << std::endl; 
       std::cout << "xAOD pt = " << (*el_it)->pt() << std::endl; 
       Info (APP_NAME,"Electron #%d", i); 

       bool value=false;
       //////////////
       Info (APP_NAME,"Electron No cut  accept() returns %d ", static_cast<bool> (nocut.accept(*el)) );      

       if(el->passSelection(value,"MultiLepton")){
	 Info (APP_NAME,"Electron MultiLepton from Reco returns %d ", static_cast<bool> (value) );
       }
       Info (APP_NAME,"Electron MultiLepton accept() returns %d ", static_cast<bool> (myMultiLepton.accept(*el)) );
       ///////////
       if(el->passSelection(value,"LHLoose")){
	 Info (APP_NAME,"Electron Likelihood Loose from Reco returns %d ", static_cast<bool> (value) );
       }
       Info (APP_NAME,"Electron Likelihood Loose accept() returns %d " , static_cast<bool> (myLikelihood.accept(*el)) ); 
       if(el->passSelection(value,"Loose")){
	 Info (APP_NAME,"Electron Loose from Reco returns %d ", static_cast<bool> (value) );
       }
       Info (APP_NAME,"Electron Loose accept() returns %d ", static_cast<bool> (myLoose.accept(*el)) );
     }
     
     Info( APP_NAME,
	   "===>>>  done processing event #%lld ",entry);
     
   }
   myMultiLepton.finalize();
   myLikelihood.finalize();
   myLoose.finalize();
   nocut.finalize();
   // Return gracefully:
   return 0;
}
