/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


//ROOT
#include <TFile.h>
#include <TH1.h>

#ifdef ROOTCORE
#include "xAODRootAccess/Init.h"
#include "xAODRootAccess/TEvent.h"
#include "xAODRootAccess/TStore.h"
#endif

#include "xAODEventInfo/EventInfo.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/TrackParticleContainer.h"

#include "InDetTruthVertexValidation/InDetVertexTruthMatchTool.h"
#include "InDetTruthVertexValidation/InDetVertexTruthMatchUtils.h"

int main( int argc, char* argv[] ) {

  const char * APP_NAME = argv[0];

  //check for filename
  if( argc < 2 ) {
    Error( APP_NAME, "No file name argument" );
    Error( APP_NAME, "  Usage: %s xAOD_file_name [nEntries]",APP_NAME );
    return 1;
  }

  //init app
  bool result = xAOD::Init( APP_NAME );
  if(!result) {
    Error( APP_NAME, "Failed to init");
    return 1;
  }

  //open file
  const char * fileName = argv[1];
  Info(APP_NAME, "Opening file: %s", fileName );
  TFile * file = new TFile(fileName,"READ");

  xAOD::TEvent event( xAOD::TEvent::kClassAccess );
  result = event.readFrom( file );
  if(!result) {
    Error( APP_NAME, "Failed to have TEvent read file");
    return 1;
  }

  //create transient object store
  xAOD::TStore store;

  Long64_t entries = event.getEntries();
  if( argc > 2 ) {
    const Long64_t e = atoll( argv[2] );
    if( e < entries )
      entries = e;
  }

  //Tool to test
  InDetVertexTruthMatchTool matchTool("VertexTruthMatchTool");

  //config
  matchTool.msg().setLevel( MSG::DEBUG );
  matchTool.setProperty( "trackMatchProb", 0.7 );
  matchTool.setProperty( "vertexMatchWeight", 0.7 );
  matchTool.initialize();

  TFile fout("output.root","RECREATE");
  fout.cd();
  
  //some inclusive test plots
  TH1F * h_cat = new TH1F("vertexMatchTypes","Number of vertices of each type",4,-0.5,3.5);
  h_cat->GetXaxis()->SetBinLabel(1, "Matched");
  h_cat->GetXaxis()->SetBinLabel(2, "Merged");
  h_cat->GetXaxis()->SetBinLabel(3, "Split");
  h_cat->GetXaxis()->SetBinLabel(4, "Fake");


  //plot some things for the best reco match to hard-scatter event
  TH1F * h_cat_hs = new TH1F("vertexTypes_hardScatter","Number of events of each type",5,-0.5,4.5);
  h_cat_hs->GetXaxis()->SetBinLabel(1, "Clean");
  h_cat_hs->GetXaxis()->SetBinLabel(2, "Low PU");
  h_cat_hs->GetXaxis()->SetBinLabel(3, "High PU");
  h_cat_hs->GetXaxis()->SetBinLabel(4, "Split");
  h_cat_hs->GetXaxis()->SetBinLabel(5, "No match");  

  //Loop
  for( Long64_t entry = 0; entry < entries; ++entry ) {

    event.getEntry( entry );

    const xAOD::EventInfo * ei = 0;
    result = event.retrieve( ei, "EventInfo" );
    if(!result) {
      Error( APP_NAME, "Failed to retrieve EventInfo on entry %i", static_cast<int>(entry));
      return 1;
    }

    if( entry % 100 == 0 ) {
      Info( APP_NAME, 
            "===Start event %i, run %i; %i events processed so far===",
            static_cast<int>( ei->eventNumber() ),
            static_cast<int>( ei->runNumber() ),
            static_cast<int>( entry ) );
    }

    //get vertices
    const xAOD::VertexContainer * vxContainer = 0;
    result = event.retrieve( vxContainer, "PrimaryVertices" );
    if(!result) {
      Error( APP_NAME, "Failed to retrieve PrimaryVertices on entry %i", static_cast<int>(entry));
      return 1;
    }

    int status = matchTool.matchVertices( *vxContainer );
    if(!status) {
      Error( APP_NAME, "Bad status from matching tool on entry %i", static_cast<int>(entry));
      continue;
    }

    //accessor for the matching type
    xAOD::Vertex::Decorator<InDetVertexTruthMatchUtils::VertexMatchType> matchTypeDecor("VertexMatchType");

    //fill the category histogram for all vertices
    for( auto vxit : *vxContainer ) {
      h_cat->Fill( matchTypeDecor( *vxit ) );
    }
    
    //get the matching type for hard scatter and fill
    h_cat_hs->Fill( InDetVertexTruthMatchUtils::classifyHardScatter( *vxContainer ) );

    //get the best match for the hard scatter
    //const xAOD::Vertex * hsMatch = InDetVertexTruthMatchUtils::bestHardScatterMatch( *vxContainer );
    
    

  }

  h_cat->Write();
  h_cat_hs->Write();
  fout.Close();

  return 0;
}
