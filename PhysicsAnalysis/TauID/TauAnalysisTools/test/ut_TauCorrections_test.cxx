/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <iostream>

// Infrastructure include(s):
#ifdef ROOTCORE
#   include "xAODRootAccess/Init.h"
#   include "xAODRootAccess/TEvent.h"
#   include "xAODRootAccess/TStore.h"
#endif // ROOTCORE

// EDM include(s):
#include "xAODEventInfo/EventInfo.h"
#include "xAODTau/TauJetContainer.h"
#include "xAODTau/TauJetAuxContainer.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthParticleAuxContainer.h"
#include "xAODTau/TauTrackContainer.h"
#include "xAODTau/TauTrackAuxContainer.h"

// Local include(s):
#include "TauAnalysisTools/TauEfficiencyCorrectionsTool.h"
#include "TauAnalysisTools/TauSmearingTool.h"


using namespace TauAnalysisTools;

#define CHECK( ARG )                              \
  do {                                            \
    const bool result = ARG;                      \
    if( ! result ) {                              \
      ::Error( "TauCorrectionsTest", "Failed to execute: \"%s\"",  \
           #ARG );                                \
      return 1;                                   \
    }                                             \
  } while( false )

#define RETRIEVE( TYPE, CONTAINER , NAME )        \
  do {                                            \
  if (xEvent.contains<TYPE>(NAME))                \
    CHECK( xEvent.retrieve( CONTAINER, NAME ) );  \
  else                                            \
    Warning("TauCorrectionsTest","%s container is not available", NAME); \
  } while(false)                                  \
 
#define CHECKSAME(VAL1, VAL2)                     \
  do {                                            \
    if (fabs(VAL1-VAL2) > 1e-7) {                 \
      ::Error( "TauCorrectionsTest", "Found different values than expected.");                     \
      ::Error( "TauCorrectionsTest", "Expected: %f. Got from TauAnalysisTools: %f ", VAL2, VAL1);  \
      return 1;                                   \
    }  \
  } while( false )


int main()
{
  CP::SystematicCode::enableFailure();
  xAOD::TReturnCode::enableFailure();

  // Initialise the application
  CHECK( xAOD::Init( "TauCorrectionsTest" ) );

  // Create TEvent and TStore objects
  xAOD::TEvent xEvent;
  xAOD::TStore xStore;

  // ===========================================================================
  // TauSmearingTool
  // ===========================================================================
  TauAnalysisTools::TauSmearingTool TauSmeTool( "TauSmearingTool" );
  CHECK( TauSmeTool.setProperty("ApplyMVATES", false ) );
  CHECK( TauSmeTool.setProperty("SkipTruthMatchCheck", true) );
  CHECK( TauSmeTool.initialize() );

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
  CHECK( TauEffCorrTool.setProperty("IDLevel", (int)JETIDBDTTIGHT) );
  CHECK( TauEffCorrTool.setProperty("SkipTruthMatchCheck", true) );
  CHECK( TauEffCorrTool.initialize() );

  // restructure all recommended systematic variations for efficiency tools
  std::map<std::string, CP::SystematicSet> mSysSet;
  for (auto SystematicsVariation : TauEffCorrTool.recommendedSystematics())
  {
    mSysSet[SystematicsVariation.name()] = CP::SystematicSet();
    mSysSet[SystematicsVariation.name()].insert(SystematicsVariation);
  }

  // ===========================================================================
  // Build tau container
  // ===========================================================================
  xAOD::TauJetContainer* xTauJetContainer = new xAOD::TauJetContainer();
  xAOD::TauJetAuxContainer* xTauJetAuxContainer = new xAOD::TauJetAuxContainer();
  xTauJetContainer->setStore( xTauJetAuxContainer );

  xAOD::TruthParticleContainer* xTauTruthContainer = new xAOD::TruthParticleContainer();
  xAOD::TruthParticleAuxContainer* xTauTruthAuxContainer = new xAOD::TruthParticleAuxContainer();
  xTauTruthContainer->setStore( xTauTruthAuxContainer );
  CHECK(xStore.record(xTauTruthContainer, "TauTruth"));
  CHECK(xStore.record(xTauTruthAuxContainer, "TauTruthAux."));

  ElementLink< xAOD::TruthParticleContainer > xTauTruthLink1;
  ElementLink< xAOD::TruthParticleContainer > xTauTruthLink2;
  ElementLink< xAOD::TruthParticleContainer > xTauTruthLink3;

  xAOD::TruthParticle* xTauTruth1 = new xAOD::TruthParticle();
  xAOD::TruthParticle* xTauTruth2 = new xAOD::TruthParticle();
  xAOD::TruthParticle* xTauTruth3 = new xAOD::TruthParticle();
  xTauTruthContainer->push_back(xTauTruth1);
  xTauTruthContainer->push_back(xTauTruth2);
  xTauTruthContainer->push_back(xTauTruth3);
  xTauTruthLink1.toContainedElement(*xTauTruthContainer, xTauTruth1);
  xTauTruthLink2.toContainedElement(*xTauTruthContainer, xTauTruth2);
  xTauTruthLink3.toContainedElement(*xTauTruthContainer, xTauTruth3);
  xTauTruth1->auxdecor<char>("IsHadronicTau") = (char)true;
  xTauTruth1->setPx(25e2);
  xTauTruth1->setPy(25e2);
  xTauTruth1->setPz(25e2);
  xTauTruth1->setM(1777);
  xTauTruth2->auxdecor<char>("IsHadronicTau") = (char)true;
  xTauTruth2->setPx(50e2);
  xTauTruth2->setPy(50e2);
  xTauTruth2->setPz(50e2);
  xTauTruth2->setM(1777);
  xTauTruth3->auxdecor<char>("IsHadronicTau") = (char)true;
  xTauTruth3->setPx(250e2);
  xTauTruth3->setPy(250e2);
  xTauTruth3->setPz(250e2);
  xTauTruth3->setM(1777);

  xAOD::TauTrackContainer* xTauTrackContainer = new xAOD::TauTrackContainer();
  xAOD::TauTrackAuxContainer* xTauTrackAuxContainer = new xAOD::TauTrackAuxContainer();
  xTauTrackContainer->setStore( xTauTrackAuxContainer );
  CHECK(xStore.record(xTauTrackContainer, "TauTracks"));
  CHECK(xStore.record(xTauTrackAuxContainer, "TauTracksAux."));

  ElementLink< xAOD::TauTrackContainer > xTauTrackLink;

  xAOD::TauTrack* xTauTrack = new xAOD::TauTrack();
  xTauTrackContainer->push_back(xTauTrack);
  xTauTrackLink.toContainedElement(*xTauTrackContainer, xTauTrack);
  xTauTrack->setP4(100, 0., 0., 0.);
  xTauTrack->setFlag(xAOD::TauJetParameters::TauTrackFlag::classifiedCharged, true); 

  xAOD::TauJet* xTau1 = new xAOD::TauJet();
  xAOD::TauJet* xTau2 = new xAOD::TauJet();
  xAOD::TauJet* xTau3 = new xAOD::TauJet();
  xTauJetContainer->push_back(xTau1);
  xTauJetContainer->push_back(xTau2);
  xTauJetContainer->push_back(xTau3);

  xTau1->setP4(25e3, -2.0, 0., 1000);
  xTau1->auxdecor<float>("etaFinalCalib") = xTau1->eta();
  xTau1->auxdecor<float>("ptFinalCalib") = xTau1->pt();
  xTau1->addTauTrackLink(xTauTrackLink);
  xTau1->auxdecor<char>("IsTruthMatched") = (char)true;
  xTau1->auxdecor<ElementLink<xAOD::TruthParticleContainer>>("truthParticleLink") = xTauTruthLink1;

  xTau2->setP4(50e3, -0.5, 0., 1000);
  xTau2->auxdecor<float>("etaFinalCalib") = xTau2->eta();
  xTau2->auxdecor<float>("ptFinalCalib") = xTau2->pt();
  xTau2->addTauTrackLink(xTauTrackLink);
  xTau2->addTauTrackLink(xTauTrackLink);
  xTau2->addTauTrackLink(xTauTrackLink);
  xTau2->auxdecor<char>("IsTruthMatched") = (char)true;
  xTau2->auxdecor<ElementLink<xAOD::TruthParticleContainer>>("truthParticleLink") = xTauTruthLink2;
  
  xTau3->setP4(250e3, 1.0, 0., 1000);
  xTau3->auxdecor<float>("etaFinalCalib") = xTau3->eta();
  xTau3->auxdecor<float>("ptFinalCalib") = xTau3->pt();
  xTau3->addTauTrackLink(xTauTrackLink);
  xTau3->auxdecor<char>("IsTruthMatched") = (char)true;
  xTau3->auxdecor<ElementLink<xAOD::TruthParticleContainer>>("truthParticleLink") = xTauTruthLink3;

  // ===========================================================================
  // Expected scale factors and uncertainties
  // ===========================================================================
  std::vector<std::map<std::string, double>> vExpectedValues;
  vExpectedValues.push_back({
    {"TESScaleFactorHadTau",                          0.9934    },
    {"TAUS_TRUEHADTAU_SME_TES_PHYSICSLIST__1down",   -0.00649495},
    {"TAUS_TRUEHADTAU_SME_TES_PHYSICSLIST__1up",      0.00649495},
    {"TAUS_TRUEHADTAU_SME_TES_INSITUFIT__1down",     -0.0087578 },
    {"TAUS_TRUEHADTAU_SME_TES_INSITUFIT__1up",        0.0087578 },
    {"TAUS_TRUEHADTAU_SME_TES_INSITUEXP__1down",     -0.00543588},
    {"TAUS_TRUEHADTAU_SME_TES_INSITUEXP__1up",        0.00543588},
    {"TAUS_TRUEHADTAU_SME_TES_MODEL_CLOSURE__1down", -0.00836961},
    {"TAUS_TRUEHADTAU_SME_TES_MODEL_CLOSURE__1up",    0.00836961},
    {"TauScaleFactorReconstructionHadTau",            1.        },
    {"TAUS_TRUEHADTAU_EFF_RECO_TOTAL__1up",           0.0118809 },
    {"TAUS_TRUEHADTAU_EFF_RECO_HIGHPT__1up",          0.        },
    {"TauScaleFactorJetIDHadTau",                     1.025     },
    {"TAUS_TRUEHADTAU_EFF_JETID_SYST__1up",           0.00292683},
    {"TAUS_TRUEHADTAU_EFF_JETID_HIGHPT__1up",         0.        }
  });

  vExpectedValues.push_back({
    {"TESScaleFactorHadTau",                          1.0018    },
    {"TAUS_TRUEHADTAU_SME_TES_PHYSICSLIST__1down",   -0.00671885},
    {"TAUS_TRUEHADTAU_SME_TES_PHYSICSLIST__1up",      0.00671885},
    {"TAUS_TRUEHADTAU_SME_TES_INSITUFIT__1down",     -0.0139748 },
    {"TAUS_TRUEHADTAU_SME_TES_INSITUFIT__1up",        0.0139748 },
    {"TAUS_TRUEHADTAU_SME_TES_INSITUEXP__1down",     -0.0058894 },
    {"TAUS_TRUEHADTAU_SME_TES_INSITUEXP__1up",        0.0058894 },
    {"TAUS_TRUEHADTAU_SME_TES_MODEL_CLOSURE__1down", -0.00156075},
    {"TAUS_TRUEHADTAU_SME_TES_MODEL_CLOSURE__1up",    0.00156075},
    {"TauScaleFactorReconstructionHadTau",            1.        },
    {"TAUS_TRUEHADTAU_EFF_RECO_TOTAL__1up",           0.0137146 },
    {"TAUS_TRUEHADTAU_EFF_RECO_HIGHPT__1up",          0.        },
    {"TauScaleFactorJetIDHadTau",                     1.062     },
    {"TAUS_TRUEHADTAU_EFF_JETID_SYST__1up",           0.00470810},
    {"TAUS_TRUEHADTAU_EFF_JETID_HIGHPT__1up",         0.        }
  });
  
  vExpectedValues.push_back({
    {"TESScaleFactorHadTau",                          1.        },
    {"TAUS_TRUEHADTAU_SME_TES_PHYSICSLIST__1down",   -0.00849452},
    {"TAUS_TRUEHADTAU_SME_TES_PHYSICSLIST__1up",      0.00849452},
    {"TAUS_TRUEHADTAU_SME_TES_INSITUFIT__1down",      0.        },
    {"TAUS_TRUEHADTAU_SME_TES_INSITUFIT__1up",        0.        },
    {"TAUS_TRUEHADTAU_SME_TES_INSITUEXP__1down",      0.        },
    {"TAUS_TRUEHADTAU_SME_TES_INSITUEXP__1up",        0.        },
    {"TAUS_TRUEHADTAU_SME_TES_DETECTOR__1down",      -0.0304613 },
    {"TAUS_TRUEHADTAU_SME_TES_DETECTOR__1up",         0.0304613 },
    {"TAUS_TRUEHADTAU_SME_TES_MODEL_CLOSURE__1down", -0.004188  },
    {"TAUS_TRUEHADTAU_SME_TES_MODEL_CLOSURE__1up",    0.004188  },
    {"TauScaleFactorReconstructionHadTau",            1.        },
    {"TAUS_TRUEHADTAU_EFF_RECO_TOTAL__1up",           0.00740873},
    {"TAUS_TRUEHADTAU_EFF_RECO_HIGHPT__1up",          0.        },
    {"TauScaleFactorJetIDHadTau",                     1.052     },
    {"TAUS_TRUEHADTAU_EFF_JETID_SYST__1up",           0.00285171},
    {"TAUS_TRUEHADTAU_EFF_JETID_HIGHPT__1up",         0.0140745 }
  });


  // ===========================================================================
  // Run over tau container
  // ===========================================================================
  int i = 0;
  for ( auto xTau : *xTauJetContainer )
  {
    // expected values for i'th tau 
    std::map<std::string, double> mVals = vExpectedValues.at(i);
    i++;

    // check TES scale factors
    double originalPt = xTau->pt();
    CHECK( TauSmeTool.applySystematicVariation(CP::SystematicSet()) );
    CHECK( TauSmeTool.applyCorrection(*xTau) );

    double sfNominalTES = xTau->pt() / originalPt;
    CHECKSAME( sfNominalTES, mVals["TESScaleFactorHadTau"] );
    // reset tau pt 
    xTau->setP4( originalPt, xTau->eta(), xTau->phi(), xTau->m());

    // check TES uncertainties
    double sfVarTES;
    for (auto sSystematicSet: vSmearingSystematicSet)
    {
      CHECK( TauSmeTool.applySystematicVariation(sSystematicSet) );
      CHECK( TauSmeTool.applyCorrection(*xTau) );
      sfVarTES = xTau->pt()/originalPt;
      CHECKSAME( (sfVarTES-sfNominalTES)/sfNominalTES, mVals[sSystematicSet.name()] );
      // reset tau pt 
      xTau->setP4( originalPt, xTau->eta(), xTau->phi(), xTau->m());
    }

    // check reconstruction and ID scale factors
    CHECK( TauEffCorrTool.applySystematicVariation(CP::SystematicSet()) );
    CHECK( TauEffCorrTool.applyEfficiencyScaleFactor(*xTau) );

    double sfNominalReco = xTau->auxdata<double>("TauScaleFactorReconstructionHadTau");
    CHECKSAME( sfNominalReco, mVals["TauScaleFactorReconstructionHadTau"] );
    double sfNominalID = xTau->auxdata<double>("TauScaleFactorJetIDHadTau");
    CHECKSAME( sfNominalID, mVals["TauScaleFactorJetIDHadTau"] );
    
    // check reconstruction uncertainties
    double sfVar;
    CHECK( TauEffCorrTool.applySystematicVariation(mSysSet["TAUS_TRUEHADTAU_EFF_RECO_TOTAL__1up"]) );
    CHECK( TauEffCorrTool.applyEfficiencyScaleFactor(*xTau) );
    sfVar = xTau->auxdata< double >("TauScaleFactorReconstructionHadTau");
    CHECKSAME( (sfVar-sfNominalReco)/sfNominalReco, mVals["TAUS_TRUEHADTAU_EFF_RECO_TOTAL__1up"] );

    CHECK( TauEffCorrTool.applySystematicVariation(mSysSet["TAUS_TRUEHADTAU_EFF_RECO_HIGHPT__1up"]) );
    CHECK( TauEffCorrTool.applyEfficiencyScaleFactor(*xTau) );
    sfVar = xTau->auxdata< double >("TauScaleFactorReconstructionHadTau");
    CHECKSAME( (sfVar-sfNominalReco)/sfNominalReco, mVals["TAUS_TRUEHADTAU_EFF_RECO_HIGHPT__1up"] );

    // check ID uncertainties
    CHECK( TauEffCorrTool.applySystematicVariation(mSysSet["TAUS_TRUEHADTAU_EFF_JETID_SYST__1up"]) );
    CHECK( TauEffCorrTool.applyEfficiencyScaleFactor(*xTau) );
    sfVar = xTau->auxdata< double >("TauScaleFactorJetIDHadTau");
    CHECKSAME( (sfVar-sfNominalID)/sfNominalID, mVals["TAUS_TRUEHADTAU_EFF_JETID_SYST__1up"] );

    CHECK( TauEffCorrTool.applySystematicVariation(mSysSet["TAUS_TRUEHADTAU_EFF_JETID_HIGHPT__1up"]) );
    CHECK( TauEffCorrTool.applyEfficiencyScaleFactor(*xTau) );
    sfVar = xTau->auxdata< double >("TauScaleFactorJetIDHadTau");
    CHECKSAME( (sfVar-sfNominalID)/sfNominalID, mVals["TAUS_TRUEHADTAU_EFF_JETID_HIGHPT__1up"] );
  }

  Info("TauCorrectionsTest", "Test finished.");
  return 0;
}
