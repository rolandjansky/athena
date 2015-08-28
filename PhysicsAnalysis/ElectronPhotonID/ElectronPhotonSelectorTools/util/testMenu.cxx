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
#include "xAODEgamma/PhotonContainer.h" 
#include "xAODEgamma/Egamma.h"

#include "ElectronPhotonSelectorTools/AsgElectronMultiLeptonSelector.h"
#include "ElectronPhotonSelectorTools/AsgElectronIsEMSelector.h"
#include "ElectronPhotonSelectorTools/AsgForwardElectronIsEMSelector.h"
#include "ElectronPhotonSelectorTools/AsgPhotonIsEMSelector.h"
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
   std::unique_ptr< TFile > ifile( TFile::Open( fileName, "READ" ) );
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


   //The path the conf files
   std::string confDir = "ElectronPhotonSelectorTools/offline/mc15_20150518/";

   //std examples


   //MultiLepton
   AsgElectronMultiLeptonSelector myMultiLepton ("myMultiLepton");
   CHECK(    myMultiLepton.initialize() );

   //Likelihood
   AsgElectronLikelihoodTool myLikelihood ("myLikelihood");
   myLikelihood.setProperty("ConfigFile",confDir+"ElectronLikelihoodTightOfflineConfig2015.conf");
   CHECK(   myLikelihood.initialize() );

   //IsEM
   AsgElectronIsEMSelector myLoose ("myLoose"); 
   myLoose.setProperty("ConfigFile",confDir+"ElectronIsEMLooseSelectorCutDefs.conf");
   myLoose.setProperty("isEMMask",static_cast<unsigned int> (egammaPID::ElectronLoosePP) );
   CHECK(    myLoose.initialize() );
   myLoose.msg().setLevel(MSG::DEBUG); //set MSG LEVEL


   //IsEMForward
   AsgForwardElectronIsEMSelector forwardLoose ("forwardLoose"); 
   forwardLoose.setProperty("ConfigFile","ElectronPhotonSelectorTools/ForwardElectronIsEMLooseSelectorCutDefs.conf");
   CHECK( forwardLoose.initialize() );
   forwardLoose.msg().setLevel(MSG::DEBUG); //set MSG LEVEL


   //IsEM no cut 
   AsgElectronIsEMSelector nocut ("nocut"); 
   nocut.setProperty("isEMMask",static_cast<unsigned int> (0) );
   nocut.initialize();

   //IsEM Photon no cut 
   AsgPhotonIsEMSelector phnocut ("phnocut"); 
   phnocut.setProperty("isEMMask",static_cast<unsigned int> (0) );
   phnocut.initialize();


   //IsEM Photon Loose
   AsgPhotonIsEMSelector myphLoose ("myphLoose"); 
   myphLoose.setProperty("ConfigFile",confDir+"PhotonIsEMLooseSelectorCutDefs.conf");
   myphLoose.setProperty("isEMMask",static_cast<unsigned int> (egammaPID::PhotonLoose) );
   CHECK( myphLoose.initialize() );
  
   myphLoose.msg().setLevel(MSG::DEBUG); //set MSG LEVEL

   //Tight photon exampe using unique ptr
   // create the selector
   std::unique_ptr<AsgPhotonIsEMSelector> m_photonTightIsEMSelector ( new AsgPhotonIsEMSelector ( "PhotonTightIsEMSelector") );
   // decide which kind of selection (loose/medium/tight) you want to use
   m_photonTightIsEMSelector.get()->setProperty("isEMMask",egammaPID::PhotonTight);
   // set the file that contains the cuts on the shower shapes (stored in http://atlas.web.cern.ch/Atlas/GROUPS/DATABASE/GroupData/)
   m_photonTightIsEMSelector.get()->setProperty("ConfigFile",confDir+"PhotonIsEMTightSelectorCutDefs.conf");
  
   CHECK( m_photonTightIsEMSelector.get()->initialize());

   ///=========================================================================
   //IsEM here I am an expert, added as an example of what we can do already, but one should
   //probably not to 
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
   //=================================================================================

   // Loop over the events:
   for( Long64_t entry = 0; entry < entries; ++entry ) {
     
     // Tell the object which entry to look at:
     event.getEntry( entry );
     
     std::cout << "=================NEXT EVENT==========================" << std::endl;
     

     //Electrons
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
       Info (APP_NAME,"Expert accept() returns %d ", static_cast<bool> (expert.accept(*el)) );      

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
     
     //Photons
     const xAOD::PhotonContainer* photons = 0 ;  
     CHECK( event.retrieve(photons, "Photons") );
     
     if ( !event.retrieve( photons, "Photons" ).isSuccess() ){ // retrieve arguments: container type, container key
       Error("execute()", "Failed to retrieve Ph container. Exiting." );
     }

     xAOD::PhotonContainer::const_iterator ph_it      = photons->begin(); 
     xAOD::PhotonContainer::const_iterator ph_it_last = photons->end(); 
     unsigned int j = 0; 
     std::cout << "TEST " << std::endl; 
     
     for (; ph_it != ph_it_last; ++ph_it, ++j) {

       const xAOD::Photon* ph = (*ph_it); 
       std::cout << "Photon " << ph << " Num " << j<< std::endl; 
       std::cout << "xAOD pt = " << (*ph_it)->pt() << std::endl; 
       Info (APP_NAME,"Photon #%d", j); 
       Info (APP_NAME,"Photon Loose cut  accept() returns %d ", static_cast<bool> (myphLoose.accept(*ph)) );      
       Info (APP_NAME,"Photon No cut  accept() returns %d ", static_cast<bool> (phnocut.accept(*ph)) );      
       Info (APP_NAME,"Photon Tight cut  accept() returns %d ", static_cast<bool> (m_photonTightIsEMSelector.get()->accept(*ph)) );      
       Info (APP_NAME,"Photon Tight cut  IsemValue() returns %u ", m_photonTightIsEMSelector.get()->IsemValue() );      

     }


    // //forwardElectron
     const xAOD::ElectronContainer* forwardelectrons = 0 ;  
     CHECK( event.retrieve( forwardelectrons, "ForwardElectrons" ) );
     if ( !event.retrieve( forwardelectrons, "ForwardElectrons" ).isSuccess() ){ // retrieve arguments: container type, container key
       Error("execute()", "Failed to retrieve forward-el container. Exiting." );
     }

     xAOD::ElectronContainer::const_iterator elforward_it      = forwardelectrons->begin(); 
     xAOD::ElectronContainer::const_iterator elforward_it_last = forwardelectrons->end(); 
     unsigned int jforward = 0; 
     std::cout << "TEST " << std::endl; 
     
     for (;elforward_it != elforward_it_last; ++elforward_it, ++jforward) {
  
       Info (APP_NAME,"FORWARD electron Loose cut  accept() returns %d ", static_cast<bool> (forwardLoose.accept(*elforward_it)) );      

     }



     Info( APP_NAME,
	   "===>>>  done processing event #%lld ",entry);
   }

   myMultiLepton.finalize();
   myLikelihood.finalize();
   myLoose.finalize();
   forwardLoose.finalize();
   nocut.finalize();
   phnocut.finalize();
   myphLoose.finalize();
   m_photonTightIsEMSelector.get()->finalize();
   expert.finalize();
   // Return gracefully:
   return 0;
}

//  LocalWords:  const
