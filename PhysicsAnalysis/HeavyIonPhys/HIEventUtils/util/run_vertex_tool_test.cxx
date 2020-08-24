/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifdef XAOD_STANDALONE
#include <memory>
#include <string>
#include "xAODRootAccess/Init.h"
#include "xAODRootAccess/TEvent.h"
#include "AsgTools/Check.h"
#include "CxxUtils/make_unique.h"
#include "HIEventUtils/HIVertexSelectionTool.h"
#include "xAODTracking/VertexContainer.h"
#include "InDetTrackSelectionTool/InDetTrackSelectionTool.h"
#include "TError.h"
#include "TFile.h"

using std::string;
using std::unique_ptr;
using CxxUtils::make_unique;

int main(int argc, char* argv[])
{
  string appName = argv[0];
  if( !xAOD::Init(appName.data()).isSuccess() ) {
    Error( appName.data(), "Could not call xAOD::Init()" );
    return 1;
  }

  string fileName;
  // Check if we received a file name:
  if( argc < 2 ) {
    fileName = getenv("ROOTCORE_TEST_FILE");
    if (fileName.empty()) {
      Error( appName.data(), "No file name received!" );
      Error( appName.data(), "  Usage: %s [xAOD file name]", appName.data() );
      return 1;
    }
  } else {
    fileName = argv[ 1 ];
  }

  unique_ptr<TFile> fin( TFile::Open( fileName.data(), "READ" ) );

  auto event = make_unique<xAOD::TEvent>( fin.get(), xAOD::TEvent::kClassAccess );
  Long64_t entries = event->getEntries();
  Info( appName.data(), "Number of events in file: %llu", entries );

  auto vertexSelTool = make_unique< HI::HIVertexSelectionTool >("vertexSelectionTool");
  ASG_CHECK_SA( appName, vertexSelTool->setProperty( "MinNTrk", 10 ) );
  ASG_CHECK_SA( appName, vertexSelTool->setProperty( "MinRmsPt", 10000. ) );

  auto vtxTrkTool = make_unique<InDet::InDetTrackSelectionTool>("vtxTrkTool", "HILoose");
  ToolHandle< InDet::IInDetTrackSelectionTool > vtxTrkToolHandle = vtxTrkTool.get();
  ASG_CHECK_SA( appName, vtxTrkTool->initialize() );
  ASG_CHECK_SA( appName, vertexSelTool->setProperty( "TrackSelectionTool", vtxTrkToolHandle ) );

  ASG_CHECK_SA( appName, vertexSelTool->initialize() );


  Long64_t nVtxPass = 0;
  Long64_t nVtxFail = 0;
  for (Long64_t i_evt=0; i_evt<entries; ++i_evt) {
    event->getEntry( i_evt );

    const xAOD::VertexContainer* vertices = nullptr;
    if ( !event->retrieve( vertices, "PrimaryVertices" ).isSuccess() ) {
      Error( appName.data(), "Could not retrieve PrimaryVertices" );
      return 1;
    }

    for (const auto* vtx : *vertices) {
      if (vertexSelTool->accept( *vtx )) nVtxPass++;
      else nVtxFail++;
    }
    
  }
  Info( appName.data(), "%llu passed.", nVtxPass );
  Info( appName.data(), "%llu failed.", nVtxFail );
  
  ASG_CHECK_SA( appName, vertexSelTool->finalize() );
  ASG_CHECK_SA( appName, vtxTrkTool->finalize() );
  return 0;
}
#endif // XAOD_STANDALONE
