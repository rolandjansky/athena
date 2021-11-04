/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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

#include "InDetSecVertexValidation/InDetSecVertexTruthMatchTool.h"
#include "InDetSecVertexValidation/InDetSecVertexTruthMatchUtils.h"

#include "AsgMessaging/MessageCheck.h"

int main( int argc, char* argv[] ) {

  using namespace asg::msgUserCode;

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
  InDetSecVertexTruthMatchTool matchTool("VertexTruthMatchTool");

  //config
  matchTool.msg().setLevel( MSG::DEBUG );
  ATH_CHECK( matchTool.setProperty( "trackMatchProb", 0.7 ) );
  ATH_CHECK( matchTool.setProperty( "vertexMatchWeight", 0.5 ) );
  ATH_CHECK( matchTool.setProperty( "pdgIds", "36" ) );
  ATH_CHECK( matchTool.initialize() );

  TFile fout("output.root","RECREATE");
  fout.cd();
  
  //some inclusive test plots
  TH1F * h_cat = new TH1F("vertexMatchTypes","Number of vertices of each type",4,-0.5,3.5);
  h_cat->GetXaxis()->SetBinLabel(1, "Matched");
  h_cat->GetXaxis()->SetBinLabel(2, "Merged");
  h_cat->GetXaxis()->SetBinLabel(3, "Split");
  h_cat->GetXaxis()->SetBinLabel(4, "Fake");


  //Loop
  for( Long64_t entry = 0; entry < entries; ++entry ) {

    event.getEntry( entry );

    const xAOD::EventInfo * ei = nullptr;
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
    const xAOD::VertexContainer * vxContainer = nullptr;
    result = event.retrieve( vxContainer, "VrtSecInclusive_SecondaryVertices" );
    if(!result) {
      Error( APP_NAME, "Failed to retrieve VrtSecInclusive_SecondaryVertices on entry %i", static_cast<int>(entry));
      return 1;
    }
    const xAOD::TruthVertexContainer * tvxContainer = nullptr;
    result = event.retrieve( tvxContainer, "TruthVertices" );
    if(!result) {
      Error( APP_NAME, "Failed to retrieve TruthVertices on entry %i", static_cast<int>(entry));
      return 1;
    }

    int status = matchTool.matchVertices( *vxContainer, *tvxContainer );
    if(!status) {
      Error( APP_NAME, "Bad status from matching tool on entry %i", static_cast<int>(entry));
      continue;
    }

    //accessor for the matching type
    xAOD::Vertex::Decorator<InDetSecVertexTruthMatchUtils::VertexMatchType> matchTypeDecor("VertexMatchType");

    //fill the category histogram for all vertices
    for( auto vxit : *vxContainer ) {
      h_cat->Fill( matchTypeDecor( *vxit ) );
    }
    
  }

  h_cat->Write();
  fout.Close();

  delete h_cat;

  return 0;
}
