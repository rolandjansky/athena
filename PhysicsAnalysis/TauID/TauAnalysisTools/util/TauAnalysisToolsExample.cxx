/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <memory>
#include <cstdlib>
#include <iostream>

// ROOT include(s):
#include <TFile.h>
#include <TError.h>
#include <TString.h>

// Infrastructure include(s):
#ifdef ROOTCORE
#   include "xAODRootAccess/Init.h"
#   include "xAODRootAccess/TEvent.h"
#endif // ROOTCORE
#include "xAODCore/ShallowCopy.h"

// EDM include(s):
#include "xAODEventInfo/EventInfo.h"
#include "xAODTau/TauJetContainer.h"
#include "xAODTruth/TruthParticle.h"

// Local include(s):
#include "TauAnalysisTools/TauEfficiencyCorrectionsTool.h"
#include "TauAnalysisTools/TauSelectionTool.h"
#include "TauAnalysisTools/TauSmearingTool.h"
#include "TauAnalysisTools/TauTruthMatchingTool.h"
#include "TauAnalysisTools/TauTruthTrackMatchingTool.h"

using namespace TauAnalysisTools;

#define CHECK( ARG )					\
  do {                                                  \
    const bool result = ARG;				\
    if( ! result ) {					\
      ::Error( "TauAnalysisToolsExample", "Failed to execute: \"%s\"",	\
	       #ARG );					\
      return 1;						\
    }							\
  } while( false )

int main( int argc, char* argv[] )
{
  // Check if we received a file name:
  if( argc < 2 )
  {
    Error( "TauAnalysisToolsExample", "No file name received!" );
    Error( "TauAnalysisToolsExample", "  Usage: %s [xAOD file name]", "TauAnalysisToolsExample" );
    return 1;
  }

  // Initialise the application:
  CHECK( xAOD::Init( "TauAnalysisToolsExample" ) );

  // Open the input file:
  const TString sInputFileName = argv[ 1 ];
  Info( "TauAnalysisToolsExample", "Opening input file: %s", sInputFileName.Data() );
  std::unique_ptr< TFile > fInputFile( TFile::Open( sInputFileName, "READ" ) );
  CHECK( fInputFile.get() );

  // Create the output file:
  const TString sOutputFileName = "output.root";
  Info( "TauAnalysisToolsExample", "Opening output file: %s", sOutputFileName.Data() );
  std::unique_ptr< TFile > fOutputFile( TFile::Open( sOutputFileName, "RECREATE" ) );
  CHECK( fOutputFile.get() );

  // Create a TEvent object:
  xAOD::TEvent xEvent( xAOD::TEvent::kClassAccess );
  CHECK( xEvent.readFrom( fInputFile.get() ) );

  // Connect TEvent with output file :
  CHECK( xEvent.writeTo( fOutputFile.get() ) );

  Info( "TauAnalysisToolsExample", "Number of events in the file: %i",
        static_cast< int >( xEvent.getEntries() ) );

  // Decide how many events to run over:
  Long64_t iEntries = xEvent.getEntries();
  if( argc > 2 )
  {
    const Long64_t iMaxEntries = atoll( argv[ 2 ] );
    if( iMaxEntries < iEntries )
    {
      iEntries = iMaxEntries;
    }
  }

  // defining needed Container
  const xAOD::EventInfo* xEventInfo = 0;
  const xAOD::TauJetContainer* xTauJetContainer = 0;
  const xAOD::TruthParticleContainer* xTruthParticleContainer = 0;
  const xAOD::TrackParticleContainer* xTrackParticleContainer = 0;
  const xAOD::VertexContainer* xVertexContainer = 0;

  // define selection tool
  TauAnalysisTools::TauSelectionTool* TauSelTool = new TauAnalysisTools::TauSelectionTool( "TauSelectionTool" );
  TauSelTool->msg().setLevel( MSG::INFO );
  // setting recommended cuts
  TauSelTool->setRecommendedProperties();
  // loosening number of cuts
  CHECK(TauSelTool->setProperty("SelectionCuts", int(CutPt | CutAbsEta | CutNTrack)));
  // also accepting 2 track taus
  std::vector<size_t> vTracks = {1,2,3};
  CHECK(TauSelTool->setProperty("NTracks", vTracks ));
  // preparation for control hisograms
  TauSelTool->setOutFile( fOutputFile.get() );
  CHECK(TauSelTool->setProperty("CreateControlPlots", true ));
  CHECK(TauSelTool->initialize());

  // define smearing tools
  TauAnalysisTools::TauSmearingTool TauSmeTool( "TauSmearingTool" );
  TauSmeTool.msg().setLevel( MSG::INFO );
  CHECK(TauSmeTool.initialize());

  // restructure all recommended systematic variations for smearing tool
  std::vector<CP::SystematicSet> vSmearingSystematicSet;
  for (auto SystematicsVariation : TauSmeTool.recommendedSystematics())
  {
    vSmearingSystematicSet.push_back(CP::SystematicSet());
    vSmearingSystematicSet.back().insert(SystematicsVariation);
  }

  // define efficiency tools
  TauAnalysisTools::TauEfficiencyCorrectionsTool TauEffJetTool( "TauEfficiencyJetTool" );
  TauEffJetTool.msg().setLevel( MSG::INFO );
  CHECK(TauEffJetTool.setProperty("EfficiencyCorrectionType", (int)SFJetID));
  CHECK(TauEffJetTool.initialize());
  TauAnalysisTools::TauEfficiencyCorrectionsTool TauEffEleTool( "TauEfficiencyEleTool" );
  TauEffEleTool.msg().setLevel( MSG::INFO );
  CHECK(TauEffEleTool.setProperty("EfficiencyCorrectionType", (int)SFEleID));
  CHECK(TauEffEleTool.initialize());
  TauAnalysisTools::TauEfficiencyCorrectionsTool TauEffContJetTool( "TauEfficiencyContJetTool" );
  TauEffContJetTool.msg().setLevel( MSG::INFO );
  CHECK(TauEffContJetTool.setProperty("EfficiencyCorrectionType", (int)SFContJetID));
  CHECK(TauEffContJetTool.initialize());

  // restructure all recommended systematic variations for efficiency tools
  std::vector<CP::SystematicSet> vEfficiencyCorrectionsSystematicSetJet;
  vEfficiencyCorrectionsSystematicSetJet.push_back(CP::SystematicSet());
  for (auto SystematicsVariation : TauEffJetTool.recommendedSystematics())
  {
    vEfficiencyCorrectionsSystematicSetJet.push_back(CP::SystematicSet());
    vEfficiencyCorrectionsSystematicSetJet.back().insert(SystematicsVariation);
  }
  std::vector<CP::SystematicSet> vEfficiencyCorrectionsSystematicSetEle;
  vEfficiencyCorrectionsSystematicSetEle.push_back(CP::SystematicSet());
  for (auto SystematicsVariation : TauEffEleTool.recommendedSystematics())
  {
    vEfficiencyCorrectionsSystematicSetEle.push_back(CP::SystematicSet());
    vEfficiencyCorrectionsSystematicSetEle.back().insert(SystematicsVariation);
  }
  std::vector<CP::SystematicSet> vEfficiencyCorrectionsSystematicSetContJet;
  vEfficiencyCorrectionsSystematicSetContJet.push_back(CP::SystematicSet());
  for (auto SystematicsVariation : TauEffContJetTool.recommendedSystematics())
  {
    vEfficiencyCorrectionsSystematicSetContJet.push_back(CP::SystematicSet());
    vEfficiencyCorrectionsSystematicSetContJet.back().insert(SystematicsVariation);
  }

  TauAnalysisTools::TauTruthMatchingTool T2MT( "TauTruthMatchingTool");
  T2MT.msg().setLevel( MSG::INFO );
  CHECK(T2MT.initialize());

  TauAnalysisTools::TauTruthTrackMatchingTool T3MT( "TauTruthTrackMatchingTool");
  T3MT.msg().setLevel( MSG::INFO );
  CHECK(T3MT.initialize());

  // Loop over the events:
  for( Long64_t iEntry = 0; iEntry < iEntries; ++iEntry )
  {

    // Tell the object which entry to look at:
    xEvent.getEntry( iEntry );

    // Print some event information for fun:
    CHECK( xEvent.retrieve( xEventInfo, "EventInfo" ) );
    Info( "TauAnalysisToolsExample",
          "===>>>  start processing event #%i, "
          "run #%i %i events processed so far  <<<===",
          static_cast< int >( xEventInfo->eventNumber() ),
          static_cast< int >( xEventInfo->runNumber() ),
          static_cast< int >( iEntry ) );

    // Get container from the event:
    CHECK( xEvent.retrieve( xTauJetContainer, "TauRecContainer" ) );
    CHECK( xEvent.retrieve( xTrackParticleContainer , "InDetTrackParticles" ) );
    CHECK( xEvent.retrieve( xVertexContainer , "PrimaryVertices" ) );
    CHECK( xEvent.retrieve( xTruthParticleContainer , "TruthParticle" ) );

    // copy truth particles to get truthparticle link for truth taus to work
    CHECK( xEvent.copy("TruthParticle") );

    Info( "TauAnalysisToolsExample", "Number of taus: %i",
          static_cast< int >( xTauJetContainer->size() ) );

    // create TruthTau Container in output file
    CHECK(T2MT.setTruthParticleContainer(xTruthParticleContainer));
    CHECK(T2MT.createTruthTauContainer());
    xAOD::TruthParticleContainer* xTruthTauContainer = T2MT.getTruthTauContainer();
    xAOD::TruthParticleAuxContainer* xTruthTauAuxContainer = T2MT.getTruthTauAuxContainer();

    CHECK( xEvent.record(xTruthTauContainer, "TruthTau") );
    CHECK( xEvent.record(xTruthTauAuxContainer, "TruthTauAux.") );

    // // Print their properties, using the tools:
    for ( auto xTau : *xTauJetContainer )
    {

      // Select "good" taus:
      if( ! TauSelTool->accept( *xTau ) ) continue;

      // print some info about the selected tau:
      Info( "TauAnalysisToolsExample", "Selected tau: pt = %g, eta = %g, phi = %g, prong = %i, charge = %i",
            xTau->pt(), xTau->eta(), xTau->phi(), int(xTau->nTracks()), int(xTau->charge()));

      T2MT.applyTruthMatch(*xTau);
      if (xTau->auxdata<bool>("IsTruthMatched"))
        Info( "TauAnalysisToolsExample",
              "Tau was matched to a truth tau, which has %i prongs and a charge of %i",
              int(xTau->auxdata<size_t>("TruthProng")),
              xTau->auxdata<int>("TruthCharge"));
      else
        Info( "TauAnalysisToolsExample", "Tau was not matched to truth" );

      for (auto sSystematicSet: vEfficiencyCorrectionsSystematicSetJet)
      {
        CHECK( TauEffJetTool.applySystematicVariation(sSystematicSet));
        CHECK( TauEffJetTool.applyEfficiencyScaleFactor(*xTau) );
        Info( "TauAnalysisToolsExample",
              "JetIDEff: %g for type %s ",
              xTau->auxdata< double >( "TauScaleFactorJetID" ),
              sSystematicSet.name().c_str());
      }
      for (auto sSystematicSet: vEfficiencyCorrectionsSystematicSetEle)
      {
        CHECK( TauEffEleTool.applySystematicVariation(sSystematicSet));
        CHECK( TauEffEleTool.applyEfficiencyScaleFactor(*xTau) );
        Info( "TauAnalysisToolsExample",
              "EleIDEff: %g for type %s ",
              xTau->auxdata< double >( "TauScaleFactorEleID" ),
              sSystematicSet.name().c_str());
      }
      for (auto sSystematicSet: vEfficiencyCorrectionsSystematicSetContJet)
      {
        CHECK( TauEffContJetTool.applySystematicVariation(sSystematicSet));
        CHECK( TauEffContJetTool.applyEfficiencyScaleFactor(*xTau) );
        Info( "TauAnalysisToolsExample",
              "ContJetIDEff: %g for type %s ",
              xTau->auxdata< double >( "TauScaleFactorContJetID" ),
              sSystematicSet.name().c_str());
      }

      for (auto sSystematicSet: vSmearingSystematicSet)
      {
        xAOD::TauJet* xTauCopy = 0;
        CHECK( TauSmeTool.applySystematicVariation(sSystematicSet)) ;
        CHECK( TauSmeTool.correctedCopy(*xTau, xTauCopy) );
        Info( "TauAnalysisToolsExample",
              "Smeared tau pt: %g for type %s ",
              xTauCopy->p4().Pt(),
              sSystematicSet.name().c_str());
      }

      for (size_t iTrack = 0; iTrack < xTau->nTracks(); iTrack++)
      {
        const xAOD::TrackParticle* xTrack = xTau->track(iTrack);
        CHECK(T3MT.classifyTrack(*xTrack));
        if (xTrack->auxdecor<bool>("IsHadronicTrack"))
          Info( "TauAnalysisToolsExample", "Track is matched to a hadronic tau decay, with decay depth %i",xTrack->auxdecor<int>("IsHadronicTrackDecayDepth"));
        else
          Info( "TauAnalysisToolsExample", "Track is not matched to a hadronic tau decay");
        Info ("TauAnalysisToolsExample", "The track decay history is: %s", (xTrack->auxdecor<std::string>("DecayHistory")).c_str());
        Info ("TauAnalysisToolsExample", "The spurious type is: %i", xTrack->auxdecor<int>("SpuriousType") );
      }
    }
    xEvent.fill();
  }

  TauSelTool->writeControlHistograms();
  delete TauSelTool;
  CHECK( xEvent.finishWritingTo(fOutputFile.get()));
  fOutputFile.get()->Close();
  Info( "TauAnalysisToolsExample", "Finished writing to file: %s", sOutputFileName.Data() );
  return 0;
}
