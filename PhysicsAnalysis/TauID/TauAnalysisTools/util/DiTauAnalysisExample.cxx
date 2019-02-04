/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
#include "xAODTau/DiTauJetContainer.h"
#include "xAODTau/DiTauJetAuxContainer.h"

#include "AsgTools/ToolHandle.h"

// Local include(s):
#include "TauAnalysisTools/DiTauEfficiencyCorrectionsTool.h"
#include "TauAnalysisTools/DiTauSmearingTool.h"
#include "TauAnalysisTools/DiTauTruthMatchingTool.h"

#include "tauRecTools/DiTauIDVarCalculator.h"
#include "tauRecTools/DiTauDiscriminantTool.h"


// Smart Slimming include(s):
#include "xAODCore/tools/IOStats.h"
#include "xAODCore/tools/ReadStats.h"

using namespace TauAnalysisTools;

#define CHECK( ARG )                    \
  do {                                                  \
    const bool result = ARG;                \
    if( ! result ) {                    \
      ::Error( "DiTauAnalysisExample", "Failed to execute: \"%s\"",  \
           #ARG );                  \
      return 1;                     \
    }                           \
  } while( false )

#define RETRIEVE( TYPE, CONTAINER , NAME )              \
  do {                                  \
  if (xEvent.contains<TYPE>(NAME))                  \
    CHECK( xEvent.retrieve( CONTAINER, NAME ) );            \
  else                                  \
    Warning("DiTauAnalysisExample","%s container is not available", NAME);       \
  } while(false)                            \
 
int main( int argc, char* argv[] )
{
  CP::SystematicCode::enableFailure();

  xAOD::TReturnCode::enableFailure();

  // Check if we received a file name:
  if( argc < 2 )
  {
    Error( "DiTauAnalysisExample", "No file name received!" );
    Error( "DiTauAnalysisExample", "  Usage: %s [xAOD file name]", "DiTauAnalysisExample" );
    return 1;
  }

  // Initialise the application:
  CHECK( xAOD::Init( "DiTauAnalysisExample" ) );

  // Open the input file:
  const TString sInputFileName = argv[ 1 ];
  Info( "DiTauAnalysisExample", "Opening input file: %s", sInputFileName.Data() );
  std::unique_ptr< TFile > fInputFile( TFile::Open( sInputFileName, "READ" ) );
  CHECK( fInputFile.get() );

  // Create the output file:
  TString sOutputFileName = "output.root";
  if (argc>3)
    sOutputFileName = TString(argv[3]);

  Info( "DiTauAnalysisExample", "Opening output file: %s", sOutputFileName.Data() );
  std::unique_ptr< TFile > fOutputFile( TFile::Open( sOutputFileName, "RECREATE" ) );
  CHECK( fOutputFile.get() );

  // Create a TEvent object:
  // xAOD::TEvent xEvent( xAOD::TEvent::kClassAccess );
  xAOD::TEvent xEvent( xAOD::TEvent::kAthenaAccess );
  CHECK( xEvent.readFrom( fInputFile.get() ) );

  // Connect TEvent with output file :
  CHECK( xEvent.writeTo( fOutputFile.get() ) );

  Info( "DiTauAnalysisExample", "Number of events in the file: %i",
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
  const xAOD::DiTauJetContainer* xDiTauJetContainer = 0;

  // ===========================================================================
  // DiTauEfficiencyCorrectionsTool
  // ===========================================================================
  TauAnalysisTools::DiTauEfficiencyCorrectionsTool DiTauEffCorrTool( "DiTauEfficiencyCorrectionsTool" );
  // DiTauEffCorrTool.msg().setLevel( MSG::DEBUG );
  CHECK(DiTauEffCorrTool.setProperty("IDLevel", int(TauAnalysisTools::JETIDBDTLOOSE)));
  CHECK(DiTauEffCorrTool.initialize());

  // restructure all recommended systematic variations for efficiency tools
  std::vector<CP::SystematicSet> vEfficiencyCorrectionsSystematicSet;
  vEfficiencyCorrectionsSystematicSet.push_back(CP::SystematicSet());
  for (auto SystematicsVariation : DiTauEffCorrTool.recommendedSystematics())
  {
    vEfficiencyCorrectionsSystematicSet.push_back(CP::SystematicSet());
    vEfficiencyCorrectionsSystematicSet.back().insert(SystematicsVariation);
  }

  // ===========================================================================
  // DiTauSmearingTool
  // ===========================================================================
  TauAnalysisTools::DiTauSmearingTool DiTauSmeTool( "DiTauSmearingTool" );
  DiTauSmeTool.msg().setLevel( MSG::DEBUG );
  CHECK(DiTauSmeTool.initialize());

  // restructure all recommended systematic variations for smearing tool
  std::vector<CP::SystematicSet> vSmearingSystematicSet;
  for (auto SystematicsVariation : DiTauSmeTool.recommendedSystematics())
  {
    vSmearingSystematicSet.push_back(CP::SystematicSet());
    vSmearingSystematicSet.back().insert(SystematicsVariation);
  }

  // ===========================================================================
  // DiTauTruthMatchingTool
  // ===========================================================================
  TauAnalysisTools::DiTauTruthMatchingTool DiTauTruthMatchingTool( "TauTruthMatchingTool");
  // DiTauTruthMatchingTool.msg().setLevel( MSG::DEBUG );
  CHECK(DiTauTruthMatchingTool.setProperty("WriteTruthTaus", false));
  CHECK(DiTauTruthMatchingTool.initialize());

  // ===========================================================================
  // DiTauIDVarCalculator
  // ===========================================================================
  tauRecTools::DiTauIDVarCalculator DiTauIDVarCalculator("IDVarCalculator");
  // DiTauIDVarCalculator.msg().setLevel( MSG::DEBUG );
  CHECK(DiTauIDVarCalculator.setProperty("doCalcCluserVariables", false));
  CHECK(DiTauIDVarCalculator.initialize());

  // ===========================================================================
  // DiTauDiscriminantTool
  // ===========================================================================
  tauRecTools::DiTauDiscriminantTool DiTauDiscrTool("DiTauDiscrTool");
  // DiTauDiscrTool.msg().setLevel( MSG::DEBUG );
  CHECK(DiTauDiscrTool.initialize());

  // Loop over the events:
  for( Long64_t iEntry = 0; iEntry < iEntries; ++iEntry )
  {

    // Tell the object which entry to look at:
    xEvent.getEntry( iEntry );

    // Print some event information for fun:
    RETRIEVE(xAOD::EventInfo, xEventInfo, "EventInfo");
    if (xEventInfo)
      Info( "DiTauAnalysisExample",
            "===>>>  start processing event #%i, "
            "run #%i %i events processed so far  <<<===",
            static_cast< int >( xEventInfo->eventNumber() ),
            static_cast< int >( xEventInfo->runNumber() ),
            static_cast< int >( iEntry ) );

    bool bIsMC = xEventInfo->eventType(xAOD::EventInfo::IS_SIMULATION);

    RETRIEVE(xAOD::DiTauJetContainer, xDiTauJetContainer, "DiTauJets");
    std::pair< xAOD::DiTauJetContainer*, xAOD::ShallowAuxContainer* >xDiTauShallowContainer = xAOD::shallowCopyContainer(*xDiTauJetContainer);

    // // copy truth particles to get truthparticle link for truth taus to work
    if (xEvent.contains<xAOD::TruthParticleContainer>("TruthParticles"))
      CHECK( xEvent.copy("TruthParticles") );

    // copy taus
    CHECK( xEvent.copy("DiTauJets") );
    CHECK( xEvent.copy("EventInfo") );

    ///////////////////////////////////////////////////////////////////
    for ( auto xDiTau : *xDiTauShallowContainer.first )
    {
      Info("DiTauAnalysisExample", "  ditau pt = %.2f GeV", (xDiTau->pt() /1000));
      CHECK( DiTauIDVarCalculator.calculateIDVariables(*xDiTau) );
      double bdtScore = DiTauDiscrTool.getJetBDTScore(*xDiTau);
      DiTauTruthMatchingTool.getTruth(*xDiTau);

      int truthMatch = 0;
      if (bIsMC && xDiTau->auxdata<char>("IsTruthHadronic"))
        truthMatch = 1;
      Info( "DiTauAnalysisExample",
            "truthMatch? %i, bdt: %g",
            truthMatch,
            bdtScore);
      Info( "DiTauAnalysisExample",
            "leading subjet pt: %g eta: %g phi: %g",
            xDiTau->subjetPt(0)/1000,
            xDiTau->subjetEta(0),
            xDiTau->subjetPhi(0));
      Info( "DiTauAnalysisExample",
            "sublead subjet pt: %g eta: %g phi: %g",
            xDiTau->subjetPt(1)/1000,
            xDiTau->subjetEta(1),
            xDiTau->subjetPhi(1));

      if (truthMatch)
        Info( "DiTauAnalysisExample",
              "truth   leading truetau pt: %g sublead truetau pt: %g deltaR: %g",
              xDiTau->auxdata<double>("TruthVisLeadPt")/1000,
              xDiTau->auxdata<double>("TruthVisSubleadPt")/1000,
              xDiTau->auxdata<double>("TruthVisDeltaR"));

      for (auto sSystematicSet: vSmearingSystematicSet)
      {
        CHECK( DiTauSmeTool.applySystematicVariation(sSystematicSet)) ;
        CHECK( DiTauSmeTool.applyCorrection(*xDiTau) );
        Info( "TauAnalysisToolsExample",
              "Smeared ditau pt: %g for type %s ",
              xDiTau->pt()*0.001,
              sSystematicSet.name().c_str());
      }

      for (auto sSystematicSet: vEfficiencyCorrectionsSystematicSet)
      {
        CHECK( DiTauEffCorrTool.applySystematicVariation(sSystematicSet));
        CHECK( DiTauEffCorrTool.applyEfficiencyScaleFactor(*xDiTau) );
        Info( "DiTauAnalysisExample",
              "SystType %s: JetIDSF: %g",
              sSystematicSet.name().c_str(),
              xDiTau->auxdata< double >( "DiTauScaleFactorJetIDHadTau" ));
      }
    }

    if (xDiTauJetContainer->empty())
    {
      CHECK( DiTauTruthMatchingTool.retrieveTruthTaus() );
    }

    CHECK( xEvent.record(xDiTauShallowContainer.first, "_DiTauJets") );
    CHECK( xEvent.record(xDiTauShallowContainer.second, "_DiTauJetsAux.") );

    xEvent.fill();
  }

  CHECK( xEvent.finishWritingTo(fOutputFile.get()));
  fOutputFile.get()->Close();
  Info( "DiTauAnalysisExample", "Finished writing to file: %s", sOutputFileName.Data() );

  // smart slimming
  xAOD::IOStats::instance().stats().printSmartSlimmingBranchList();

  return 0;
}
