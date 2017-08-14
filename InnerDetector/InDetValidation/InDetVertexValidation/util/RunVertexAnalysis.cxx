/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ROOT
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
#include "xAODTruth/TruthEventContainer.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthVertexContainer.h"

#include "InDetTruthVertexValidation/InDetVertexTruthMatchTool.h"
#include "InDetVertexValidation/VertexAnalysis.h"

//#include "PileupReweighting/PileupReweightingTool.h"

int main( int argc, char* argv[] ) {

  const char * APP_NAME = argv[0];
  
  // Check for filename
  if( argc < 2 ) {
    Error( APP_NAME, "No file name argument" );
    Error( APP_NAME, "Usage: %s xAOD_file_name [outName vertexColl nEntries]",APP_NAME );
    return 1;
  }

  // Check for output file name
  const char * outputName = "output.root";
  if( argc > 2 ) {
    outputName = argv[2];
  }
  
  // Check for vertex container name
  const char * vertexCollName = "PrimaryVertices";
  if( argc > 3 ) {
    vertexCollName = argv[3];
  }

  // Init app
  bool result = xAOD::Init( APP_NAME );
  if(!result) {
    Error( APP_NAME, "Failed to init");
    return 1;
  }

  // Open file
  const char * fileName = argv[1];
  Info(APP_NAME, "Opening file: %s", fileName );
  TFile * file = new TFile(fileName,"READ");

  xAOD::TEvent event( xAOD::TEvent::kClassAccess );
  result = event.readFrom( file );
  if(!result) {
    Error( APP_NAME, "Failed to have TEvent read file");
    return 1;
  }
  
  // Create transient object store
  xAOD::TStore store;
  
  Long64_t entries = event.getEntries();
  if( argc > 4 ) {
    const Long64_t e = atoll( argv[4] );
    if( e < entries )
      entries = e;
  }

  // Tool to test
  InDetVertexTruthMatchTool matchTool("VertexTruthMatchTool");
  // Config
  matchTool.msg().setLevel( MSG::DEBUG );
  matchTool.setProperty( "trackMatchProb", 0.7 );
  matchTool.setProperty( "vertexMatchWeight", 0.7 );
  matchTool.initialize();

  TFile fout(outputName,"RECREATE");
  fout.cd();
  
  // Create the analyzer object
  VertexAnalysis analyzer;

  float m_MaxLayoutEta = 4.0;
  std::vector<std::pair<TH1*,std::string> > vhists = analyzer.bookHistograms(m_MaxLayoutEta);

  // Loop
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
    const xAOD::VertexContainer *vxContainer = 0;
    const xAOD::TrackParticleContainer *trackContainer = 0;
    const xAOD::TruthEventContainer *truthEventContainer = 0;
    const xAOD::TruthPileupEventContainer *truthPUEventContainer = 0;
    const xAOD::TruthParticleContainer *truthParticleContainer = 0;
    const xAOD::TruthVertexContainer *truthVertexContainer = 0;
    
    result = event.retrieve(vxContainer, vertexCollName);
    if(!result) {
      Error( APP_NAME, "Failed to retrieve PrimaryVertices on entry %i", static_cast<int>(entry));
      return 1;
    }
    
    //---
    const char * trackCollName = "InDetTrackParticles";
    // Work with either the DC14 or MC15 names
    const char * truthEventCollName = event.contains<xAOD::TruthEventContainer>("TruthEvents") ? "TruthEvents" : "TruthEvent";
    const char * truthPUEventCollName = event.contains<xAOD::TruthEventContainer>("TruthPileupEvents") ? "TruthPileupEvents" : "TruthPileupEvents";
    const char * truthParticleCollName = event.contains<xAOD::TruthParticleContainer>("TruthParticles") ? "TruthParticles" : "TruthParticle";
    const char * truthVertexCollName = event.contains<xAOD::TruthVertexContainer>("TruthVertices") ? "TruthVertices" : "TruthVertices";
    
    result = event.retrieve(trackContainer, trackCollName);
    if(!result) {
      Error( APP_NAME, "Failed to retrieve TrackParticles on entry %i", static_cast<int>(entry));
      return 1;
    }
    
    // Retrieve the truth events:
    bool Truthevents_available = false;
    bool TruthPUevents_available = false;

    if(ei->eventType(xAOD::EventInfo::IS_SIMULATION) == true) {

      result = event.retrieve(truthEventContainer, truthEventCollName);
      if(!result) {
        Error( APP_NAME, "Failed to retrieve TruthEvents on entry %i", static_cast<int>(entry));
        return 1;
      }
      Truthevents_available = true;

      result = event.retrieve(truthPUEventContainer, truthPUEventCollName);
      if(!result) {
        Error( APP_NAME, "Failed to retrieve TruthPUEvents on entry %i", static_cast<int>(entry));
        return 1;
      }
      TruthPUevents_available = true;

      result = event.retrieve(truthParticleContainer, truthParticleCollName);
      if(!result) {
        Error( APP_NAME, "Failed to retrieve TruthParticle on entry %i", static_cast<int>(entry));
        return 1;
      }

      result = event.retrieve(truthVertexContainer, truthVertexCollName);
      if(!result) {
        Error( APP_NAME, "Failed to retrieve TruthVertex on entry %i", static_cast<int>(entry));
        return 1;
      }
    }
    //---

    // Run matching
    int status = matchTool.matchVertices( *vxContainer );
    if(!status) {
      Error( APP_NAME, "Bad status from matching tool on entry %i", static_cast<int>(entry));
      continue;
    }
    
    // Pileup Reweighting Tool
    /*m_pileupReweightingTool = new PileupReweightingTool("PileupReweightingTool");
    CHECK(m_pileupReweightingTool->setProperty("UsePeriodConfig", "MC15"));
    std::vector<std::string> lumicalcFiles;
    cout << "$ROOTCOREBIN" << endl;
    lumicalcFiles.push_back("$ROOTCOREBIN/ilumicalc_histograms_None_297730-303304.root");
    CHECK(m_pileupReweightingTool->setProperty("LumiCalcFiles", lumicalcFiles));
    
    if (!m_pileupReweightingTool->initialize().isSuccess()) {
    ATH_MSG_WARNING("Failed to properly initialize the Pile Up Reweighting. Exiting.");
    return StatusCode::FAILURE;
    }
    
    m_runNumber = eventInfo->runNumber();
    m_eventNumber = eventInfo->eventNumber();
    m_lumiBlock = eventInfo->lumiBlock();
    
    auto myTool = dynamic_cast<PileupReweightingTool*>(m_pileupReweightingTool);*/

    double m_muWeight = 1.0;

    xAOD::TrackVertexAssociationMap trkvxassoUniqueMap_loose;
    xAOD::TrackVertexAssociationMap trkvxassoUniqueMap_tight;
    
    // Run analysis
    double m_muCorrected = -1.0;
    
    if(m_muCorrected == -1.0){
      m_muCorrected = ei->actualInteractionsPerCrossing();
    }

    status = analyzer.execute(*vxContainer, *truthVertexContainer, *ei, m_muCorrected, m_muWeight, *trackContainer, *truthEventContainer, *truthPUEventContainer, trkvxassoUniqueMap_loose, trkvxassoUniqueMap_tight, Truthevents_available, TruthPUevents_available);

    if(status) {
      Error( APP_NAME, "Bad status from analyzer on entry %i", static_cast<int>(entry));
      continue;
    }
  }

  analyzer.procHistograms();
  
  for(auto & hit : vhists )
    hit.first->Write();
  fout.Close();
  
  return 0;
  
}
