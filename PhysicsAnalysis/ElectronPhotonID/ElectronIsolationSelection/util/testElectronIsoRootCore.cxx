/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: testElectronIsoRootCore.cxx 635994 2014-12-15 14:37:46Z morrisj $

// Mindlessly copied from CPAnalysisExamples
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

#include "xAODEgamma/ElectronContainer.h"
#include "xAODCore/ShallowCopy.h"

// To be honest, I don't like the way Info() works
#include <iostream>
using std::cout;
using std::endl;
#include <cmath>

#include "ElectronIsolationSelection/ElectronIsolationSelectionTool.h"


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
  xAOD::TEvent event( xAOD::TEvent::kClassAccess );
  CHECK( event.readFrom( ifile.get() ) );
  Info( APP_NAME, "Number of events in the file: %i",
        static_cast< int >( event.getEntries() ) );

  // Create a transient object store. Needed for the tools.
  xAOD::TStore store;

  // Decide how many events to run over:
  Long64_t entries = event.getEntries();
  if( argc > 2 ) {
    const Long64_t e = atoll( argv[ 2 ] );
    if( e < entries ) {
        entries = e;
    }
  }
  
  // This is a testing file, lets fail whenever we can
  StatusCode::enableFailure();

  CP::ElectronIsolationSelectionTool isoTool( "ElectronIsolationSelectionTool" );
  CHECK( isoTool.initialize() );

  // Should I change the CHECK macro to something else? If so what?
  
  // Fixed 95% efficiency
  CHECK( isoTool.configureEfficiencyBasedIsolation( xAOD::Iso::etcone20 , "0.95" ) );
  CHECK( isoTool.configureEfficiencyBasedIsolation( xAOD::Iso::ptcone30 , "0.95" ) );   
  
  // Configure separate isolation conditions to study multijet backgrounds
  CHECK( isoTool.configureCutBasedIsolationBackground( xAOD::Iso::etcone20 , 6000.0 ) );
  CHECK( isoTool.configureCutBasedIsolationBackground( xAOD::Iso::ptcone30 , 6000.0 ) );   
  
  std::string m_sgKey("ElectronCollection"),m_sgKeyEventInfo("EventInfo");
  // Loop over the events:
  for( Long64_t entry = 0; entry < entries; ++entry ) {

    // Tell the object which entry to look at:
    event.getEntry( entry );
      
    const xAOD::ElectronContainer* electrons(nullptr);
    CHECK( event.retrieve( electrons , m_sgKey ) );
    
    const xAOD::EventInfo* evtInfo(nullptr);
    CHECK( event.retrieve( evtInfo , m_sgKeyEventInfo ) );
    
    Info(APP_NAME,"-----------");
    
    // Copy for corrections
    std::pair< xAOD::ElectronContainer*, xAOD::ShallowAuxContainer* > electrons_shallowCopy = xAOD::shallowCopyContainer( *electrons );
    //Iterate over the shallow copy
    xAOD::ElectronContainer* elsCorr = electrons_shallowCopy.first;
    xAOD::ElectronContainer::iterator el_it      = elsCorr->begin();
    xAOD::ElectronContainer::iterator el_it_last = elsCorr->end();   
      
    for(; el_it != el_it_last; ++el_it){
      xAOD::Electron* el = *el_it;
      // some basic selections
      if( el->pt() > 20000.0 ){
        float feta = fabs( el->caloCluster()->eta() );
        if( feta < 2.47 ){
          if( !(feta > 1.37 && feta < 1.52) ){   
            
              
            Info(APP_NAME," Testing Electron with pT = %f , cluster eta = %f ", el->pt() , el->caloCluster()->eta() );
            
            Info(APP_NAME,"  Testing analysis isolation conditions" );
            // lets test the isolation
            if( isoTool.accept( *el ) ){
              Info(APP_NAME,"   --> passes isolation" );
            }
            else{
              Info(APP_NAME,"   --> fails isolation" );
            }
              
            std::shared_ptr<std::map<TString,float>> cutValues = isoTool.getCutValues( *el );
            for( std::map<TString,float>::const_iterator i=cutValues->begin();i!=cutValues->end();++i ){
              // There has to be a getter way to get Info to accept TString.....
              Info(APP_NAME,"    Cut for %s is <= %f GeV ", std::string((*i).first).c_str() , (*i).second );
            }
            
            const Root::TAccept& acceptDecision = isoTool.getTAccept();
            for(unsigned int a=0;a<acceptDecision.getNCuts();++a){
              Info(APP_NAME,"    Cut decision for %s is %i " , std::string(acceptDecision.getCutName(a)).c_str() , acceptDecision.getCutResult(a) );
            }
              
              
            Info(APP_NAME,"  Testing background isolation conditions" );
            // lets test the isolation
            if( isoTool.acceptBackground( *el ) ){
              Info(APP_NAME,"   --> passes background isolation" );
            }
            else{
              Info(APP_NAME,"   --> fails background isolation" );
            }
              
            std::shared_ptr<std::map<TString,float>> cutValuesBackground = isoTool.getCutValuesBackground( *el );
            for( std::map<TString,float>::const_iterator i=cutValuesBackground->begin();i!=cutValuesBackground->end();++i ){
              Info(APP_NAME,"    Background cut for %s is <= %f GeV ", std::string((*i).first).c_str() , (*i).second );
            } 
            
            const Root::TAccept& acceptDecisionBackground = isoTool.getTAcceptBackground();
            for(unsigned int a=0;a<acceptDecisionBackground.getNCuts();++a){
              Info(APP_NAME,"    Background cut decision for %s is %i " , std::string(acceptDecisionBackground.getCutName(a)).c_str() , acceptDecisionBackground.getCutResult(a) );
            }          
          }
        }
      }
    }
  }
  return 0; 
}
    
    
