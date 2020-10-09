/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ROOTCORE
#ifdef XAOD_ANALYSIS

#ifndef TAUANALYSISTOOLS_UT_TAUANALYSISTOOLS_TEST_H
#define TAUANALYSISTOOLS_UT_TAUANALYSISTOOLS_TEST_H 1


#include "AthAnalysisBaseComps/AthAnalysisHelper.h" //tool creation and configuration
#include "POOLRootAccess/TEvent.h" //event looping
#include "GaudiKernel/ToolHandle.h" //for better working with tools

#include "AsgMessaging/MessageCheck.h" //messaging
using namespace asg::msgUserCode;  //messaging

//ROOT includes
#include "TString.h"
#include "TSystem.h"

// Local include(s):
#include "TauAnalysisTools/TauEfficiencyCorrectionsTool.h"
#include "TauAnalysisTools/TauSelectionTool.h"
#include "TauAnalysisTools/TauSmearingTool.h"
#include "TauAnalysisTools/TauTruthMatchingTool.h"
#include "TauAnalysisTools/TauTruthTrackMatchingTool.h"
#include "TauAnalysisTools/TauOverlappingElectronLLHDecorator.h"

// EDM include(s):
#include "xAODTau/TauJetContainer.h"

int main( int argc, char* argv[] )
{

  IAppMgrUI* app = POOL::Init(); //important to do this first!

  // Open the input file:
  TString fileName = "$ASG_TEST_FILE_MC";
  if( argc < 2 )
  {
    ANA_MSG_WARNING( "No file name received, using $ASG_TEST_FILE_MC" );
  }
  else
  {
    fileName = argv[1]; //use the user provided file
  }
  ANA_MSG_INFO("Opening file: " << gSystem->ExpandPathName(fileName.Data()) );

  //Here's an example of how you would create a tool of type ToolType, and set a property on it
  //The preferred way to create and configure the tool is with a ToolHandle:
  //ToolHandle<IToolInterface> myTool("ToolType/myTool");
  //AthAnalysisHelper::setProperty( myTool, "MyProperty", value );
  //myTool.retrieve(); //this will cause the tool to be created and initialized

  // ===========================================================================
  // TauSelectionTool
  // ===========================================================================
  ToolHandle<TauAnalysisTools::ITauSelectionTool> TauSelTool("TauAnalysisTools::TauSelectionTool/TauSelectionTool");
  TauSelTool.retrieve(); //this will cause the tool to be created and initialized

  // ===========================================================================
  // TauSmearingTool
  // ===========================================================================
  ToolHandle<TauAnalysisTools::ITauSmearingTool> TauSmeTool("TauAnalysisTools::TauSmearingTool/TauSmearingTool");
  TauSmeTool.retrieve();

  // ===========================================================================
  // TauEfficiencyCorrectionsTool
  // ===========================================================================
  ToolHandle<TauAnalysisTools::ITauEfficiencyCorrectionsTool> TauEffCorrTool( "TauAnalysisTools::TauEfficiencyCorrectionsTool/TauEfficiencyCorrectionsTool" );
  AthAnalysisHelper::setProperty(TauEffCorrTool, "TauSelectionTool", TauSelTool);
  TauEffCorrTool.retrieve();

  // ===========================================================================
  // TauTruthMatchingTool
  // ===========================================================================
  ToolHandle<TauAnalysisTools::ITauTruthMatchingTool> T2MT( "TauAnalysisTools::TauTruthMatchingTool/TauTruthMatchingTool");
  AthAnalysisHelper::setProperty(T2MT, "WriteTruthTaus", true);
  T2MT.retrieve();

  // ===========================================================================
  // TauTruthTrackMatchingTool
  // ===========================================================================
  ToolHandle<TauAnalysisTools::ITauTruthTrackMatchingTool> T3MT( "TauAnalysisTools::TauTruthTrackMatchingTool/TauTruthTrackMatchingTool");
  T3MT.retrieve();

  // defining needed Container
  const xAOD::TauJetContainer* xTauJetContainer = 0;

  //loop over input file with POOL
  POOL::TEvent evt;
  evt.readFrom( fileName );

  // for(int i=0;i < evt.getEntries(); i++) {
  for(int i=0; i < 100; i++)
  {
    if( evt.getEntry(i) < 0)
    {
      ANA_MSG_ERROR("Failed to read event " << i);
      continue;
    }

    evt.retrieve( xTauJetContainer, "TauJets" );

    for ( auto xTau : *xTauJetContainer )
    {
      // perform truth matching
      auto xTruthTau = T2MT->getTruth(*xTau);

      // Select "good" taus:
      if( ! TauSelTool->accept( *xTau ) ) continue;

      // print some info about the selected tau:
      ANA_MSG_INFO( "Selected tau: pt = " << xTau->pt()
                    << " MeV, eta = " << xTau->eta()
                    << ", phi = " << xTau->phi()
                    << ", prong = " << int(xTau->nTracks())
                    << ", charge = " << int(xTau->charge()));

      if ((bool)xTau->auxdata<char>("IsTruthMatched"))
      {
        if (xTruthTau->isTau())
        {
          if ((bool)xTruthTau->auxdata<char>("IsHadronicTau"))
            ANA_MSG_INFO( "Tau was matched to a truth hadronic tau, which has " << int(xTruthTau->auxdata<size_t>("numCharged"))
                          << " prongs and a charge of " << int(xTruthTau->charge()));
          else
            ANA_MSG_INFO( "Tau was matched to a truth leptonic tau, which has a charge of " << int(xTruthTau->charge()));
        }
        else if (xTruthTau->isElectron())
          ANA_MSG_INFO( "Tau was matched to a truth electron");
        else if (xTruthTau->isMuon())
          ANA_MSG_INFO( "Tau was matched to a truth muon");
      }
      else
        ANA_MSG_INFO( "Tau was not matched to truth" );

      typedef ElementLink< xAOD::TruthParticleContainer > Link_t;
      if (!xTau->isAvailable< Link_t >("truthParticleLink"))
      {
        ANA_MSG_WARNING("link truthParticleLink is not available");
        continue;
      }
      auto xTruthJetLink = xTau->auxdata< ElementLink< xAOD::JetContainer > >("truthJetLink");
      if (xTruthJetLink.isValid())
      {
        const xAOD::Jet* xTruthJet = *xTruthJetLink;
        ANA_MSG_INFO( "Tau was matched to a truth jet, which has pt = " << xTruthJet->p4().Pt()
                      << ", eta = " << xTruthJet->p4().Eta()
                      << ", phi = " << xTruthJet->p4().Phi()
                      << ", m = " << xTruthJet->p4().M() );
      }
      else
        ANA_MSG_INFO( "Tau was not matched to truth jet" );

      // test the TauEfficiencyCorrectionsTool
      ANA_CHECK(TauEffCorrTool->applyEfficiencyScaleFactor(*xTau));

      // test the TauSmearingTool
      xAOD::TauJet* xTauCopy = 0;
      ANA_CHECK(TauSmeTool->correctedCopy(*xTau, xTauCopy));
    }
  }
  ServiceHandle<IProperty> toolSvc("ToolSvc","");
  toolSvc->setProperty("OutputLevel","1");
  asg::msgToolHandle::setMsgLevel(MSG::Level::DEBUG);

  app->finalize(); //trigger finalization of all services and tools created by the Gaudi Application
  return 0;
}

#endif //> !TAUANALYSISTOOLS_UT_TAUANALYSISTOOLS_TEST_H

#else
int main()
{
  return 0;
}

#endif // XAOD_ANALYSIS

#endif // not ROOTCORE
