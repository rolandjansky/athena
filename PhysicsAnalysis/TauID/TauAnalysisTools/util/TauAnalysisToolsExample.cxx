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
#include "xAODTau/TauJetAuxContainer.h"
#include "xAODTruth/TruthParticle.h"

// Local include(s):
#include "TauAnalysisTools/TauEfficiencyCorrectionsTool.h"
#include "TauAnalysisTools/TauSelectionTool.h"
#include "TauAnalysisTools/TauSmearingTool.h"
#include "TauAnalysisTools/TauTruthMatchingTool.h"
#include "TauAnalysisTools/TauTruthTrackMatchingTool.h"
#include "TauAnalysisTools/TauOverlappingElectronLLHDecorator.h"

// Smart Slimming include(s):
#include "xAODCore/tools/IOStats.h"
#include "xAODCore/tools/ReadStats.h"

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

#define RETRIEVE( TYPE, CONTAINER , NAME )				\
  do {									\
  if (xEvent.contains<TYPE>(NAME))					\
    CHECK( xEvent.retrieve( CONTAINER, NAME ) );			\
  else									\
    Warning("TauAnalysisToolsExample","%s container is not available", NAME);		\
  } while(false)							\
 
int main( int argc, char* argv[] )
{
  CP::SystematicCode::enableFailure();

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
  TString sOutputFileName = "output.root";
  if (argc>3)
    sOutputFileName = TString(argv[3]);

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

  // ===========================================================================
  // TauSelectionTool
  // ===========================================================================
  TauAnalysisTools::TauSelectionTool* TauSelTool = new TauAnalysisTools::TauSelectionTool( "TauSelectionTool" );
  TauSelTool->msg().setLevel( MSG::INFO );
  // preparation for control hisograms
  TauSelTool->setOutFile( fOutputFile.get() );
  CHECK(TauSelTool->setProperty("CreateControlPlots", true ));
  CHECK(TauSelTool->initialize());

  // ===========================================================================
  // TauSmearingTool
  // ===========================================================================
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

  // ===========================================================================
  // TauEfficiencyCorrectionsTool
  // ===========================================================================
  TauAnalysisTools::TauEfficiencyCorrectionsTool TauEffCorrTool( "TauEfficiencyCorrectionsTool" );
  TauEffCorrTool.msg().setLevel( MSG::DEBUG );
  CHECK(TauEffCorrTool.initialize());

  // restructure all recommended systematic variations for efficiency tools
  std::vector<CP::SystematicSet> vEfficiencyCorrectionsSystematicSet;
  vEfficiencyCorrectionsSystematicSet.push_back(CP::SystematicSet());
  for (auto SystematicsVariation : TauEffCorrTool.recommendedSystematics())
  {
    std::cout<< "SystematicsVariation " << SystematicsVariation.basename()<<"\n";
    vEfficiencyCorrectionsSystematicSet.push_back(CP::SystematicSet());
    vEfficiencyCorrectionsSystematicSet.back().insert(SystematicsVariation);
  }
  CP::SystematicSet sHighRecoPtDown;
  sHighRecoPtDown.insert(CP::SystematicVariation("TAUS_TRUEHADTAU_EFF_RECO_TOTAL",-1));
  sHighRecoPtDown.insert(CP::SystematicVariation("TAUS_TRUEHADTAU_EFF_RECO_HIGHPT",-1));
  vEfficiencyCorrectionsSystematicSet.push_back(sHighRecoPtDown);
  CP::SystematicSet sHighRecoPtUp;
  sHighRecoPtUp.insert(CP::SystematicVariation("TAUS_TRUEHADTAU_EFF_RECO_TOTAL",1));
  sHighRecoPtUp.insert(CP::SystematicVariation("TAUS_TRUEHADTAU_EFF_RECO_HIGHPT",1));
  vEfficiencyCorrectionsSystematicSet.push_back(sHighRecoPtUp);

  CP::SystematicSet sHighJetIDPtDown;
  sHighJetIDPtDown.insert(CP::SystematicVariation("TAUS_TRUEHADTAU_EFF_JETID_TOTAL",-1));
  sHighJetIDPtDown.insert(CP::SystematicVariation("TAUS_TRUEHADTAU_EFF_JETID_HIGHPT",-1));
  vEfficiencyCorrectionsSystematicSet.push_back(sHighJetIDPtDown);
  CP::SystematicSet sHighJetIDPtUp;
  sHighJetIDPtUp.insert(CP::SystematicVariation("TAUS_TRUEHADTAU_EFF_JETID_TOTAL",1));
  sHighJetIDPtUp.insert(CP::SystematicVariation("TAUS_TRUEHADTAU_EFF_JETID_HIGHPT",1));
  vEfficiencyCorrectionsSystematicSet.push_back(sHighJetIDPtUp);

  // ===========================================================================
  // TauEfficiencyCorrectionsTriggerTool
  // ===========================================================================
  TauAnalysisTools::TauEfficiencyCorrectionsTool TauEffTrigTool( "TauEfficiencyCorrectionsTriggerTool" );
  TauEffTrigTool.msg().setLevel( MSG::DEBUG );
  CHECK(TauEffTrigTool.setProperty("EfficiencyCorrectionTypes", std::vector<int>({SFTriggerHadTau}) ));
  CHECK(TauEffTrigTool.setProperty("TriggerName", "HLT_tau125_medium1_tracktwo" ));
  CHECK(TauEffTrigTool.setProperty("IDLevel", (int)JETIDBDTTIGHT ));
  CHECK(TauEffTrigTool.initialize());

  // restructure all recommended systematic variations for efficiency tools
  std::vector<CP::SystematicSet> vEfficiencyCorrectionsTriggerSystematicSet;
  vEfficiencyCorrectionsTriggerSystematicSet.push_back(CP::SystematicSet());
  for (auto SystematicsVariation : TauEffTrigTool.recommendedSystematics())
  {
    std::cout<< "SystematicsVariation " << SystematicsVariation.basename()<<"\n";
    vEfficiencyCorrectionsTriggerSystematicSet.push_back(CP::SystematicSet());
    vEfficiencyCorrectionsTriggerSystematicSet.back().insert(SystematicsVariation);
  }

  CP::SystematicSet sTOTALDOWN;
  sTOTALDOWN.insert(CP::SystematicVariation("TAUS_TRUEHADTAU_EFF_TRIGGER_TOTAL",-1));
  vEfficiencyCorrectionsTriggerSystematicSet.push_back(sTOTALDOWN);
  CP::SystematicSet sTOTALUP;
  sTOTALUP.insert(CP::SystematicVariation("TAUS_TRUEHADTAU_EFF_TRIGGER_TOTAL",1));
  vEfficiencyCorrectionsTriggerSystematicSet.push_back(sTOTALUP);

  CP::SystematicSet sTOTALDOWN_COMB;
  sTOTALDOWN_COMB.insert(CP::SystematicVariation("TAUS_TRUEHADTAU_EFF_TRIGGER_STATDATA",-1));
  sTOTALDOWN_COMB.insert(CP::SystematicVariation("TAUS_TRUEHADTAU_EFF_TRIGGER_STATMC",1));
  sTOTALDOWN_COMB.insert(CP::SystematicVariation("TAUS_TRUEHADTAU_EFF_TRIGGER_SYST",-1));
  vEfficiencyCorrectionsTriggerSystematicSet.push_back(sTOTALDOWN_COMB);
  CP::SystematicSet sTOTALUP_COMB;
  sTOTALUP_COMB.insert(CP::SystematicVariation("TAUS_TRUEHADTAU_EFF_TRIGGER_STATDATA",1));
  sTOTALUP_COMB.insert(CP::SystematicVariation("TAUS_TRUEHADTAU_EFF_TRIGGER_STATMC",-1));
  sTOTALUP_COMB.insert(CP::SystematicVariation("TAUS_TRUEHADTAU_EFF_TRIGGER_SYST",1));
  vEfficiencyCorrectionsTriggerSystematicSet.push_back(sTOTALUP_COMB);

  // ===========================================================================
  // TauTruthMatchingTool
  // ===========================================================================
  TauAnalysisTools::TauTruthMatchingTool T2MT( "TauTruthMatchingTool");
  T2MT.msg().setLevel( MSG::INFO );
  CHECK(T2MT.setProperty("WriteTruthTaus", true));
  CHECK(T2MT.initialize());

  // ===========================================================================
  // TauTruthTrackMatchingTool
  // ===========================================================================
  TauAnalysisTools::TauTruthTrackMatchingTool T3MT( "TauTruthTrackMatchingTool");
  T3MT.msg().setLevel( MSG::INFO );
  CHECK(T3MT.initialize());

  // ===========================================================================
  // TauOverlappingElectronLLHDecorator
  // ===========================================================================
  TauAnalysisTools::TauOverlappingElectronLLHDecorator TOELLHDecorator( "TauOverlappingElectronLLHDecorator");
  TOELLHDecorator.msg().setLevel( MSG::INFO );
  CHECK(TOELLHDecorator.initialize());

  // Loop over the events:
  for( Long64_t iEntry = 0; iEntry < iEntries; ++iEntry )
  {

    // Tell the object which entry to look at:
    xEvent.getEntry( iEntry );

    // Print some event information for fun:
    RETRIEVE(xAOD::EventInfo, xEventInfo, "EventInfo");
    if (xEventInfo)
      Info( "TauAnalysisToolsExample",
            "===>>>  start processing event #%i, "
            "run #%i %i events processed so far  <<<===",
            static_cast< int >( xEventInfo->eventNumber() ),
            static_cast< int >( xEventInfo->runNumber() ),
            static_cast< int >( iEntry ) );

    RETRIEVE(xAOD::TauJetContainer, xTauJetContainer, "TauJets");
    RETRIEVE(xAOD::TrackParticleContainer, xTrackParticleContainer , "InDetTrackParticles" );
    RETRIEVE(xAOD::TruthParticleContainer, xTruthParticleContainer , "TruthParticles" );

    // copy truth particles to get truthparticle link for truth taus to work
    CHECK( xEvent.copy("TruthParticles") );

    // copy taus
    CHECK( xEvent.copy("TauJets") );

    // copy tracks
    CHECK( xEvent.copy("InDetTrackParticles") );

    // Print tau properties, using the tools:
    for ( auto xTau : *xTauJetContainer )
    {
      // decorate tau with electron llh score
      CHECK(TOELLHDecorator.decorate(*xTau));

      // perform truth matching
      auto xTruthTau = T2MT.getTruth(*xTau);

      // Select "good" taus:
      if( ! TauSelTool->accept( *xTau ) ) continue;

      // print some info about the selected tau:
      Info( "TauAnalysisToolsExample", "Selected tau: pt = %g, eta = %g, phi = %g, prong = %i, charge = %i",
            xTau->pt(), xTau->eta(), xTau->phi(), int(xTau->nTracks()), int(xTau->charge()));

      if ((bool)xTau->auxdata<char>("IsTruthMatched"))
      {
        if (xTruthTau->isTau())
        {
          if ((bool)xTruthTau->auxdata<char>("IsHadronicTau"))
            Info( "TauAnalysisToolsExample",
                  "Tau was matched to a truth hadronic tau, which has %i prongs and a charge of %i",
                  int(xTruthTau->auxdata<size_t>("numCharged")),
                  int(xTruthTau->charge()));
          else
            Info( "TauAnalysisToolsExample",
                  "Tau was matched to a truth leptonic tau, which has a charge of %i",
                  int(xTruthTau->charge()));
        }
        else if (xTruthTau->isElectron())
          Info( "TauAnalysisToolsExample",
                "Tau was matched to a truth electron");
        else if (xTruthTau->isMuon())
          Info( "TauAnalysisToolsExample",
                "Tau was matched to a truth muon");
      }
      else
        Info( "TauAnalysisToolsExample", "Tau was not matched to truth" );

      auto xTruthJetLink = xTau->auxdata< ElementLink< xAOD::JetContainer > >("truthJetLink");
      if (xTruthJetLink.isValid())
      {
        const xAOD::Jet* xTruthJet = *xTruthJetLink;
        Info( "TauAnalysisToolsExample",
              "Tau was matched to a truth jet, which has pt=%g, eta=%g, phi=%g, m=%g",
              xTruthJet->p4().Pt(),
              xTruthJet->p4().Eta(),
              xTruthJet->p4().Phi(),
              xTruthJet->p4().M());
      }
      else
        Info( "TauAnalysisToolsExample", "Tau was not matched to truth jet" );

      for (auto sSystematicSet: vEfficiencyCorrectionsSystematicSet)
      {
        CHECK( TauEffCorrTool.applySystematicVariation(sSystematicSet));
        CHECK( TauEffCorrTool.applyEfficiencyScaleFactor(*xTau) );
        Info( "TauAnalysisToolsExample",
              "SystType %s: RecoSF: %g JetIDSF: %g EleOLRSFHadTau: %g EleOLRSFElectron: %g",
              sSystematicSet.name().c_str(),
              xTau->auxdata< double >( "TauScaleFactorReconstructionHadTau" ),
              xTau->auxdata< double >( "TauScaleFactorJetIDHadTau" ),
              xTau->auxdata< double >( "TauScaleFactorEleOLRHadTau" ),
              xTau->auxdata< double >( "TauScaleFactorEleOLRElectron" ));
      }

      for (auto sSystematicSet: vEfficiencyCorrectionsTriggerSystematicSet)
      {
        CHECK( TauEffTrigTool.applySystematicVariation(sSystematicSet));
        CHECK( TauEffTrigTool.applyEfficiencyScaleFactor(*xTau) );
        Info( "TauAnalysisToolsExample",
              "SystType %s: Trigger: %g",
              sSystematicSet.name().c_str(),
              xTau->auxdata< double >( "TauScaleFactorTriggerHadTau" ));
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
        const TAUTRACKPARTICLE* xTrack = xTau->track(iTrack);

        CHECK(T3MT.classifyTrack(*xTrack));
        if (xTrack->auxdata<char>("IsHadronicTrack"))
          Info( "TauAnalysisToolsExample", "Track is matched to a hadronic tau decay, with decay depth %i",xTrack->auxdata<int>("IsHadronicTrackDecayDepth"));
        else
          Info( "TauAnalysisToolsExample", "Track is not matched to a hadronic tau decay");
        Info ("TauAnalysisToolsExample", "The track decay history is: %s", (xTrack->auxdata<std::string>("DecayHistory")).c_str());
        Info ("TauAnalysisToolsExample", "The spurious type is: %i", xTrack->auxdata<int>("SpuriousType") );
      }
    }
    xEvent.fill();
  }

  TauSelTool->writeControlHistograms();
  delete TauSelTool;
  CHECK( xEvent.finishWritingTo(fOutputFile.get()));
  fOutputFile.get()->Close();
  Info( "TauAnalysisToolsExample", "Finished writing to file: %s", sOutputFileName.Data() );

  // smart slimming
  xAOD::IOStats::instance().stats().printSmartSlimmingBranchList();

  return 0;
}
