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
#include <TProfile.h>

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
#include "xAODCore/ShallowCopy.h"

#include "AsgTools/ToolHandle.h"

// Local include(s):
#include "TauAnalysisTools/TauEfficiencyCorrectionsTool.h"
#include "TauAnalysisTools/TauSelectionTool.h"
#include "TauAnalysisTools/TauSmearingTool.h"
#include "TauAnalysisTools/TauTruthMatchingTool.h"
#include "TauAnalysisTools/TauTruthTrackMatchingTool.h"
#include "TauAnalysisTools/TauOverlappingElectronLLHDecorator.h"

#include "PileupReweighting/PileupReweightingTool.h"

using namespace TauAnalysisTools;

std::map<std::string, TH2F*> mTH2F;
double ev_weight = 1;

#define CHECK( ARG )                    \
  do {                                                  \
    const bool result = ARG;                \
    if( ! result ) {                    \
      ::Error( "TauAnalysisToolsValidation", "Failed to execute: \"%s\"",  \
           #ARG );                  \
      return 1;                     \
    }                           \
  } while( false )

#define RETRIEVE( TYPE, CONTAINER , NAME )              \
  do {                                  \
  if (xEvent.contains<TYPE>(NAME))                  \
    CHECK( xEvent.retrieve( CONTAINER, NAME ) );            \
  else                                  \
    Warning("TauAnalysisToolsValidation","%s container is not available", NAME);       \
  } while(false)                            \
 


void fillHistograms(std::string sBaseName, const xAOD::TauJet& xTau, double val);


int main( int argc, char* argv[] )
{
  CP::SystematicCode::enableFailure();

  xAOD::TReturnCode::enableFailure();

  std::string sNominalNames[2]     = {"TauScaleFactorReconstructionHadTau", "TauScaleFactorJetIDHadTau"};
  std::string sSystematicsNames[4] = {"TAUS_TRUEHADTAU_EFF_RECO_TOTAL__1up", "TAUS_TRUEHADTAU_EFF_RECO_HIGHPT__1up", 
                                      "TAUS_TRUEHADTAU_EFF_JETID_TOTAL__1up", "TAUS_TRUEHADTAU_EFF_JETID_HIGHPT__1up"};

  double bins[7] = {20,30,40,60,100,300,1000};
  for (auto s: sNominalNames)
  {
    mTH2F[s+"__1p__pt"]     = new TH2F((s+"__1p__pt").c_str(),    (s+"__1p__pt").c_str(),   20,  0, 500, 160, 0.8, 1.2);
    mTH2F[s+"__3p__pt"]     = new TH2F((s+"__3p__pt").c_str(),    (s+"__3p__pt").c_str(),   20,  0, 500, 160, 0.8, 1.2);
    mTH2F[s+"__1p__log_pt"] = new TH2F((s+"__1p__log_pt").c_str(),(s+"__1p__log_pt").c_str(),6,  bins,   160, 0.8, 1.2);
    mTH2F[s+"__3p__log_pt"] = new TH2F((s+"__3p__log_pt").c_str(),(s+"__3p__log_pt").c_str(),6,  bins,   160, 0.8, 1.2);
    mTH2F[s+"__1p__eta"]    = new TH2F((s+"__1p__eta").c_str(),   (s+"__1p__eta").c_str(),  24, -3,   3, 160, 0.8, 1.2);
    mTH2F[s+"__3p__eta"]    = new TH2F((s+"__3p__eta").c_str(),   (s+"__3p__eta").c_str(),  24, -3,   3, 160, 0.8, 1.2); 
  }
   
  for (auto s: sSystematicsNames)
  {
    mTH2F[s+"__1p__pt"]     = new TH2F((s+"__1p__pt").c_str(),    (s+"__1p__pt").c_str(),   20,  0, 500, 160, -0.2, 0.2);
    mTH2F[s+"__3p__pt"]     = new TH2F((s+"__3p__pt").c_str(),    (s+"__3p__pt").c_str(),   20,  0, 500, 160, -0.2, 0.2);
    mTH2F[s+"__1p__log_pt"] = new TH2F((s+"__1p__log_pt").c_str(),(s+"__1p__log_pt").c_str(),6,  bins,   160, -0.2, 0.2);
    mTH2F[s+"__3p__log_pt"] = new TH2F((s+"__3p__log_pt").c_str(),(s+"__3p__log_pt").c_str(),6,  bins,   160, -0.2, 0.2);
    mTH2F[s+"__1p__eta"]    = new TH2F((s+"__1p__eta").c_str(),   (s+"__1p__eta").c_str(),  24, -3,   3, 160, -0.2, 0.2);
    mTH2F[s+"__3p__eta"]    = new TH2F((s+"__3p__eta").c_str(),   (s+"__3p__eta").c_str(),  24, -3,   3, 160, -0.2, 0.2);
  }
 
  // Check if we received a file name:
  if( argc < 2 )
  {
    Error( "TauAnalysisToolsValidation", "No file name received!" );
    Error( "TauAnalysisToolsValidation", "  Usage: %s [xAOD file name]", "TauAnalysisToolsValidation" );
    return 1;
  }

  // Initialise the application:
  CHECK( xAOD::Init( "TauAnalysisToolsValidation" ) );

  // Open the input file:
  const TString sInputFileName = argv[ 1 ];
  Info( "TauAnalysisToolsValidation", "Opening input file: %s", sInputFileName.Data() );
  std::unique_ptr< TFile > fInputFile( TFile::Open( sInputFileName, "READ" ) );
  CHECK( fInputFile.get() );

  // Create the output file:
  TString sOutputFileName = "output.root";
  if (argc>3)
    sOutputFileName = TString(argv[3]);

  Info( "TauAnalysisToolsValidation", "Opening output file: %s", sOutputFileName.Data() );
  std::unique_ptr< TFile > fOutputFile( TFile::Open( sOutputFileName, "RECREATE" ) );
  CHECK( fOutputFile.get() );

  std::unique_ptr< TFile > fOutputAOD( TFile::Open( "AOD.root", "RECREATE" ) );
  CHECK( fOutputAOD.get() );

  // Create a TEvent object:
  // xAOD::TEvent xEvent( xAOD::TEvent::kClassAccess );
  xAOD::TEvent xEvent( xAOD::TEvent::kAthenaAccess );
  CHECK( xEvent.readFrom( fInputFile.get() ) );

  // Connect TEvent with output file :
  CHECK( xEvent.writeTo( fOutputAOD.get() ) );

  Info( "TauAnalysisToolsValidation", "Number of events in the file: %i",
        static_cast< int >( xEvent.getEntries() ) );

  // Decide how many events to run over:
  Long64_t iEntries = xEvent.getEntries();
  if( argc > 2 )
  {
    const Long64_t iMaxEntries = atoll( argv[ 2 ] );
    if( iMaxEntries>0 && iMaxEntries < iEntries )
    {
      iEntries = iMaxEntries;
    }
  }

  // defining needed Container
  const xAOD::EventInfo* xEventInfo = 0;
  const xAOD::TauJetContainer* xTauJetContainer = 0;

  CP::PileupReweightingTool* m_tPRWTool = new CP::PileupReweightingTool("PileupReweightingTool");
  std::vector<std::string> vLumiCalcFiles = {"/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/dev/PileupReweighting/ilumicalc_histograms_HLT_e24_lhvloose_nod0_L1EM20VH_297730-304494_OflLumi-13TeV-005.root"};
  CHECK(m_tPRWTool->setProperty("LumiCalcFiles", vLumiCalcFiles));
  // CHECK(m_tPRWTool->setProperty("DefaultChannel", "" ));
  CHECK(m_tPRWTool->initialize());
  ToolHandle<CP::IPileupReweightingTool> m_tPRWToolHandle = m_tPRWTool;

  // ===========================================================================
  // TauSelectionTool
  // ===========================================================================
  TauAnalysisTools::TauSelectionTool* TauSelTool = new TauAnalysisTools::TauSelectionTool( "TauSelectionTool" );
  TauSelTool->msg().setLevel( MSG::DEBUG );
  TauSelTool->setOutFile( fOutputFile.get() );
  CHECK(TauSelTool->setProperty("CreateControlPlots", true ));
  // CHECK(TauSelTool->setProperty("JetIDWP", int(JETIDBDTLOOSE) ));
  // CHECK(TauSelTool->setProperty("JetIDWP", int(JETIDBDTMEDIUM) ));
  CHECK(TauSelTool->setProperty("JetIDWP", int(JETIDBDTTIGHT) ));
  CHECK(TauSelTool->setProperty("PtMin", 20. ));
  CHECK(TauSelTool->setProperty("ConfigPath", "" ));
  CHECK(TauSelTool->setProperty("SelectionCuts", int(CutPt|CutJetIDWP) ));
  CHECK(TauSelTool->initialize());

  ToolHandle<TauAnalysisTools::ITauSelectionTool> TauSelToolHandle = TauSelTool;

  // ===========================================================================
  // TauSmearingTool
  // ===========================================================================
  TauAnalysisTools::TauSmearingTool TauSmeTool( "TauSmearingTool" );
  TauSmeTool.msg().setLevel( MSG::INFO );
  CHECK(TauSmeTool.setProperty("ApplyMVATES", true ));
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
  TauEffCorrTool.msg().setLevel( MSG::INFO );
  CHECK(TauEffCorrTool.setProperty("TauSelectionTool", TauSelToolHandle));
  CHECK(TauEffCorrTool.initialize());

  // restructure all recommended systematic variations for efficiency tools
  std::map<std::string, CP::SystematicSet> mSysSet;
  std::vector<CP::SystematicSet> vEfficiencyCorrectionsSystematicSet;
  vEfficiencyCorrectionsSystematicSet.push_back(CP::SystematicSet());
  mSysSet["NOMINAL"] = CP::SystematicSet();
  for (auto SystematicsVariation : TauEffCorrTool.recommendedSystematics())
  {
    mSysSet[SystematicsVariation.name()] = CP::SystematicSet();
    mSysSet[SystematicsVariation.name()].insert(SystematicsVariation);
    vEfficiencyCorrectionsSystematicSet.push_back(CP::SystematicSet());
    vEfficiencyCorrectionsSystematicSet.back().insert(SystematicsVariation);
  }


  // ===========================================================================
  // TauEfficiencyCorrectionsTriggerTool
  // ===========================================================================
  // TauAnalysisTools::TauEfficiencyCorrectionsTool TauEffTrigTool( "TauEfficiencyCorrectionsTriggerTool" );
  // TauEffTrigTool.msg().setLevel( MSG::INFO );
  // CHECK(TauEffTrigTool.setProperty("EfficiencyCorrectionTypes", std::vector<int>({SFTriggerHadTau}) ));
  // CHECK(TauEffTrigTool.setProperty("TriggerName", "HLT_tau25_medium1_tracktwo" ));
  // CHECK(TauEffTrigTool.setProperty("IDLevel", (int)JETIDBDTTIGHT ));
  // CHECK(TauEffTrigTool.setProperty("PileupReweightingTool", m_tPRWToolHandle ));
  // CHECK(TauEffTrigTool.setProperty("TriggerSFMeasurement", "combined"));
  // CHECK(TauEffTrigTool.initialize());

  // restructure all recommended systematic variations for efficiency tools
  // std::vector<CP::SystematicSet> vEfficiencyCorrectionsTriggerSystematicSet;
  // vEfficiencyCorrectionsTriggerSystematicSet.push_back(CP::SystematicSet());
  // for (auto SystematicsVariation : TauEffTrigTool.recommendedSystematics())
  // {
  //   vEfficiencyCorrectionsTriggerSystematicSet.push_back(CP::SystematicSet());
  //   vEfficiencyCorrectionsTriggerSystematicSet.back().insert(SystematicsVariation);
  // }

  // ===========================================================================
  // TauTruthMatchingTool
  // ===========================================================================
  TauAnalysisTools::TauTruthMatchingTool T2MT( "TauTruthMatchingTool");
  T2MT.msg().setLevel( MSG::INFO );
  CHECK(T2MT.setProperty("WriteTruthTaus", true));
  CHECK(T2MT.initialize());


  // Loop over the events:
  for( Long64_t iEntry = 0; iEntry < iEntries; ++iEntry )
  {
    // Tell the object which entry to look at:
    xEvent.getEntry( iEntry );

    // Print some event information for fun:
    RETRIEVE(xAOD::EventInfo, xEventInfo, "EventInfo");
    if (xEventInfo)
      Info( "TauAnalysisToolsValidation",
            "===>>>  start processing event #%i, "
            "run #%i %i events processed so far  <<<===",
            static_cast< int >( xEventInfo->eventNumber() ),
            static_cast< int >( xEventInfo->runNumber() ),
            static_cast< int >( iEntry ) );

    RETRIEVE(xAOD::TauJetContainer, xTauJetContainer, "TauJets");
    std::pair< xAOD::TauJetContainer*, xAOD::ShallowAuxContainer* >xTauShallowContainer = xAOD::shallowCopyContainer(*xTauJetContainer);

    // get event weight
    ev_weight = 1;
    const std::vector<float> weights = xEventInfo->mcEventWeights();
    if(weights.size()>0) ev_weight = weights[0];

    // Print tau properties, using the tools:
    for ( auto xTau : *xTauShallowContainer.first )
    {
      // perform truth matching
      auto xTruthTau = T2MT.getTruth(*xTau);

      for (auto sSystematicSet: vSmearingSystematicSet)
      {
        CHECK( TauSmeTool.applySystematicVariation(sSystematicSet)) ;
        CHECK( TauSmeTool.applyCorrection(*xTau) );
      }

      // Select "good" taus:
      if( ! TauSelTool->accept( *xTau ) ) continue;

      // skip tau candidates that are not true hadronic decays
      if (!(bool)xTau->auxdata<char>("IsTruthMatched") or !(bool)xTruthTau->auxdata<char>("IsHadronicTau")) continue;

      // apply efficiency corrections and fill validation plots
      double sfNominalReco = 0;
      double sfNominalID = 0;
      double sfVar = 0;

      CHECK( TauEffCorrTool.applySystematicVariation(mSysSet["NOMINAL"]));
      CHECK( TauEffCorrTool.applyEfficiencyScaleFactor(*xTau) );
      sfNominalReco = xTau->auxdata<double>("TauScaleFactorReconstructionHadTau");
      sfNominalID = xTau->auxdata<double>("TauScaleFactorJetIDHadTau");
      fillHistograms("TauScaleFactorReconstructionHadTau", *xTau, sfNominalReco);
      fillHistograms("TauScaleFactorJetIDHadTau", *xTau, sfNominalID);

      CHECK( TauEffCorrTool.applySystematicVariation(mSysSet["TAUS_TRUEHADTAU_EFF_RECO_TOTAL__1up"]));
      CHECK( TauEffCorrTool.applyEfficiencyScaleFactor(*xTau) );
      sfVar = xTau->auxdata< double >("TauScaleFactorReconstructionHadTau");
      fillHistograms("TAUS_TRUEHADTAU_EFF_RECO_TOTAL__1up", *xTau, sfVar-sfNominalReco);

      CHECK( TauEffCorrTool.applySystematicVariation(mSysSet["TAUS_TRUEHADTAU_EFF_RECO_HIGHPT__1up"]));
      CHECK( TauEffCorrTool.applyEfficiencyScaleFactor(*xTau) );
      sfVar = xTau->auxdata< double >("TauScaleFactorReconstructionHadTau");
      fillHistograms("TAUS_TRUEHADTAU_EFF_RECO_HIGHPT__1up", *xTau, sfVar-sfNominalReco);

      CHECK( TauEffCorrTool.applySystematicVariation(mSysSet["TAUS_TRUEHADTAU_EFF_JETID_TOTAL__1up"]));
      CHECK( TauEffCorrTool.applyEfficiencyScaleFactor(*xTau) );
      sfVar = xTau->auxdata< double >("TauScaleFactorJetIDHadTau");
      fillHistograms("TAUS_TRUEHADTAU_EFF_JETID_TOTAL__1up", *xTau, sfVar-sfNominalID);

      CHECK( TauEffCorrTool.applySystematicVariation(mSysSet["TAUS_TRUEHADTAU_EFF_JETID_HIGHPT__1up"]));
      CHECK( TauEffCorrTool.applyEfficiencyScaleFactor(*xTau) );
      sfVar = xTau->auxdata< double >("TauScaleFactorJetIDHadTau");
      fillHistograms("TAUS_TRUEHADTAU_EFF_JETID_HIGHPT__1up", *xTau, sfVar-sfNominalID);


      // for (auto sSystematicSet: vEfficiencyCorrectionsTriggerSystematicSet)
      // {
      //   CHECK( TauEffTrigTool.applySystematicVariation(sSystematicSet));
      //   CHECK( TauEffTrigTool.applyEfficiencyScaleFactor(*xTau) );
      //   Info( "TauAnalysisToolsValidation",
      //         "SystType %s: Trigger: %g",
      //         sSystematicSet.name().c_str(),
      //         xTau->auxdata< double >( "TauScaleFactorTriggerHadTau" ));
      // }

    }
    if (xTauJetContainer->empty())
      CHECK(T2MT.retrieveTruthTaus());
    CHECK(xEvent.fill());
  }

  // Tau selection control histograms
  TauSelTool->writeControlHistograms();
  delete TauSelTool;
  fOutputFile.get()->mkdir(("EfficiencyCorrectionsTool_control"));
  fOutputFile.get()->cd(("EfficiencyCorrectionsTool_control"));

  // Tau efficiency control histograms
  for (auto& h: mTH2F) h.second->Write();
  for (auto& h: mTH2F) h.second->ProfileX((h.first+"_pfx").c_str(), 1, -1, "s")->Write();
  fOutputFile.get()->Close();
  Info( "TauAnalysisToolsValidation", "Finished writing to file: %s", sOutputFileName.Data() );


  // AOD output
  CHECK( xEvent.finishWritingTo(fOutputAOD.get()));
  fOutputAOD.get()->Close();
  Info( "TauAnalysisToolsValidation", "Finished writing to file: AOD.root" );

  return 0;
}


void fillHistograms( std::string sBaseName, const xAOD::TauJet& xTau, double val )
{
  if (xTau.nTracks() == 1)
  {
    mTH2F[sBaseName+"__1p__pt"]->Fill(xTau.pt()/1000, val, ev_weight);
    mTH2F[sBaseName+"__1p__log_pt"]->Fill(xTau.pt()/1000, val, ev_weight);
    mTH2F[sBaseName+"__1p__eta"]->Fill(xTau.eta(), val, ev_weight);
  }
  else if (xTau.nTracks() == 3)
  {
    mTH2F[sBaseName+"__3p__pt"]->Fill(xTau.pt()/1000, val, ev_weight);
    mTH2F[sBaseName+"__3p__log_pt"]->Fill(xTau.pt()/1000, val, ev_weight);
    mTH2F[sBaseName+"__3p__eta"]->Fill(xTau.eta(), val, ev_weight);
  }
}
