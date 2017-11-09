/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
  xTau1->auxdecor<float>("etaTauEtaCalib") = xTau1->eta();
  xTau1->auxdecor<float>("ptTauEtaCalib") = xTau1->pt();
  xTau1->addTauTrackLink(xTauTrackLink);

  xTau2->setP4(50e3, -0.5, 0., 1000);
  xTau2->auxdecor<float>("etaTauEtaCalib") = xTau2->eta();
  xTau2->auxdecor<float>("ptTauEtaCalib") = xTau2->pt();
  xTau2->addTauTrackLink(xTauTrackLink);
  xTau2->addTauTrackLink(xTauTrackLink);
  xTau2->addTauTrackLink(xTauTrackLink);
  
  xTau3->setP4(250e3, 1.0, 0., 1000);
  xTau3->auxdecor<float>("etaTauEtaCalib") = xTau3->eta();
  xTau3->auxdecor<float>("ptTauEtaCalib") = xTau3->pt();
  xTau3->addTauTrackLink(xTauTrackLink);

  // ===========================================================================
  // Expected scale factors and uncertainties
  // ===========================================================================
  std::vector<std::map<std::string, double>> vExpectedValues;
  vExpectedValues.push_back({
    {"TESScaleFactorHadTau",                    1.006     },
    {"TAUS_TRUEHADTAU_SME_TES_DETECTOR__1down", 0.        },
    {"TAUS_TRUEHADTAU_SME_TES_DETECTOR__1up",   0.        },
    {"TAUS_TRUEHADTAU_SME_TES_INSITU__1down",  -0.03689   },
    {"TAUS_TRUEHADTAU_SME_TES_INSITU__1up",     0.03689   },
    {"TAUS_TRUEHADTAU_SME_TES_MODEL__1down",   -0.0102478 },
    {"TAUS_TRUEHADTAU_SME_TES_MODEL__1up",      0.0102478 },
    {"TauScaleFactorReconstructionHadTau",      1.        },
    {"TAUS_TRUEHADTAU_EFF_RECO_TOTAL__1up",     0.00885311},
    {"TAUS_TRUEHADTAU_EFF_RECO_HIGHPT__1up",    0.        },
    {"TauScaleFactorJetIDHadTau",               1.074     },
    {"TAUS_TRUEHADTAU_EFF_JETID_TOTAL__1up",    0.05962   },
    {"TAUS_TRUEHADTAU_EFF_JETID_HIGHPT__1up",   0.        }
  });

  vExpectedValues.push_back({
    {"TESScaleFactorHadTau",                    0.977517  },
    {"TAUS_TRUEHADTAU_SME_TES_DETECTOR__1down", 0.        },
    {"TAUS_TRUEHADTAU_SME_TES_DETECTOR__1up",   0.        },
    {"TAUS_TRUEHADTAU_SME_TES_INSITU__1down",  -0.02343   },
    {"TAUS_TRUEHADTAU_SME_TES_INSITU__1up",     0.02343   },
    {"TAUS_TRUEHADTAU_SME_TES_MODEL__1down",   -0.00464107},
    {"TAUS_TRUEHADTAU_SME_TES_MODEL__1up",      0.00464107},
    {"TauScaleFactorReconstructionHadTau",      1.        },
    {"TAUS_TRUEHADTAU_EFF_RECO_TOTAL__1up",     0.0155508 },
    {"TAUS_TRUEHADTAU_EFF_RECO_HIGHPT__1up",    0.        },
    {"TauScaleFactorJetIDHadTau",               1.108     },
    {"TAUS_TRUEHADTAU_EFF_JETID_TOTAL__1up",    0.06964   },
    {"TAUS_TRUEHADTAU_EFF_JETID_HIGHPT__1up",   0.        }
  });
  
  vExpectedValues.push_back({
    {"TESScaleFactorHadTau",                     1.        },
    {"TAUS_TRUEHADTAU_SME_TES_DETECTOR__1down", -0.0304613 },
    {"TAUS_TRUEHADTAU_SME_TES_DETECTOR__1up",    0.0304613 },
    {"TAUS_TRUEHADTAU_SME_TES_INSITU__1down",    0.        },
    {"TAUS_TRUEHADTAU_SME_TES_INSITU__1up",      0.        },
    {"TAUS_TRUEHADTAU_SME_TES_MODEL__1down",    -0.00316562},
    {"TAUS_TRUEHADTAU_SME_TES_MODEL__1up",       0.00316562},
    {"TauScaleFactorReconstructionHadTau",       1.        },
    {"TAUS_TRUEHADTAU_EFF_RECO_TOTAL__1up",      0.00591177},
    {"TAUS_TRUEHADTAU_EFF_RECO_HIGHPT__1up",     0.        },
    {"TauScaleFactorJetIDHadTau",                1.074     },
    {"TAUS_TRUEHADTAU_EFF_JETID_TOTAL__1up",     0.05962   },
    {"TAUS_TRUEHADTAU_EFF_JETID_HIGHPT__1up",    0.0282150 }
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
    CHECK( TauEffCorrTool.applySystematicVariation(mSysSet["TAUS_TRUEHADTAU_EFF_JETID_TOTAL__1up"]) );
    CHECK( TauEffCorrTool.applyEfficiencyScaleFactor(*xTau) );
    sfVar = xTau->auxdata< double >("TauScaleFactorJetIDHadTau");
    CHECKSAME( (sfVar-sfNominalID)/sfNominalID, mVals["TAUS_TRUEHADTAU_EFF_JETID_TOTAL__1up"] );

    CHECK( TauEffCorrTool.applySystematicVariation(mSysSet["TAUS_TRUEHADTAU_EFF_JETID_HIGHPT__1up"]) );
    CHECK( TauEffCorrTool.applyEfficiencyScaleFactor(*xTau) );
    sfVar = xTau->auxdata< double >("TauScaleFactorJetIDHadTau");
    CHECKSAME( (sfVar-sfNominalID)/sfNominalID, mVals["TAUS_TRUEHADTAU_EFF_JETID_HIGHPT__1up"] );
  }

  Info("TauCorrectionsTest", "Test finished.");
  return 0;
}
