/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//        Copyright Iowa State University 2016.
//                  Author: Nils Krumnack
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (nils.erik.krumnack@cern.ch) for bug
// reports, feature suggestions, praise and complaints.


//
// includes
//

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma GCC diagnostic ignored "-Wunused-local-typedefs"
#include <boost/algorithm/string.hpp>
#pragma GCC diagnostic pop

#include <AsgTools/AnaToolHandle.h>
#include <AsgTesting/UnitTest.h>
#include <AssociationUtils/AltMuJetOverlapTool.h>
#include <AssociationUtils/DeltaROverlapTool.h>
#include <AssociationUtils/EleJetOverlapTool.h>
#include <AssociationUtils/EleMuSharedTrkOverlapTool.h>
#include <AssociationUtils/MuJetOverlapTool.h>
#include <AssociationUtils/OverlapRemovalTool.h>
#include <AssociationUtils/TauLooseEleOverlapTool.h>
#include <AssociationUtils/TauLooseMuOverlapTool.h>
#include <ElectronEfficiencyCorrection/AsgElectronEfficiencyCorrectionTool.h>
#include <ElectronPhotonFourMomentumCorrection/EgammaCalibrationAndSmearingTool.h>
#include <ElectronPhotonSelectorTools/AsgElectronIsEMSelector.h>
#include <ElectronPhotonSelectorTools/AsgElectronLikelihoodTool.h>
#include <ElectronPhotonSelectorTools/AsgPhotonIsEMSelector.h>
#include <ElectronPhotonShowerShapeFudgeTool/ElectronPhotonShowerShapeFudgeTool.h>
#include <IsolationCorrections/IsolationCorrectionTool.h>
#include <IsolationSelection/IsolationSelectionTool.h>
#include <JetCalibTools/JetCalibrationTool.h>
#include <JetJvtEfficiency/JetJvtEfficiency.h>
#include <JetMomentTools/JetVertexTaggerTool.h>
#include <JetResolution/JERSmearingTool.h>
#include <JetResolution/JERTool.h>
#include <JetSelectorTools/JetCleaningTool.h>
#include <JetUncertainties/JetUncertaintiesTool.h>
#include <METUtilities/METMaker.h>
#include <METUtilities/METSystematicsTool.h>
#include <MuonEfficiencyCorrections/MuonEfficiencyScaleFactors.h>
#include <MuonEfficiencyCorrections/MuonTriggerScaleFactors.h>
#include <MuonMomentumCorrections/MuonCalibrationAndSmearingTool.h>
#include <MuonSelectorTools/MuonSelectionTool.h>
#include <PhotonEfficiencyCorrection/AsgPhotonEfficiencyCorrectionTool.h>
#include <PileupReweighting/PileupReweightingTool.h>
#include <TauAnalysisTools/TauEfficiencyCorrectionsTool.h>
#include <TauAnalysisTools/TauSelectionTool.h>
#include <TauAnalysisTools/TauSmearingTool.h>
#include <TauAnalysisTools/TauTruthMatchingTool.h>
#include <TauAnalysisTools/TauTruthTrackMatchingTool.h>
#include <TrigConfxAOD/xAODConfigTool.h>
#include <gtest/gtest.h>
#include <xAODBTaggingEfficiency/BTaggingEfficiencyTool.h>
#include <xAODBTaggingEfficiency/BTaggingSelectionTool.h>

#ifdef ROOTCORE
#include <xAODRootAccess/Init.h>
#endif

#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

//
// method implementations
//

using namespace asg::msgUserCode;

TEST (ToolMakeTest, make_AsgElectronLikelihoodTool)
{
  asg::AnaToolHandle<IAsgElectronLikelihoodTool> handle ("tool");
  ASSERT_SUCCESS (handle.make ("AsgElectronLikelihoodTool"));
}

TEST (ToolMakeTest, makeNew_AsgElectronLikelihoodTool)
{
  asg::AnaToolHandle<IAsgElectronLikelihoodTool> handle ("tool");
  ASSERT_SUCCESS (ASG_MAKE_ANA_TOOL (handle, AsgElectronLikelihoodTool));
}

TEST (ToolMakeTest, DISABLED_initialize_AsgElectronLikelihoodTool)
{
  asg::AnaToolHandle<IAsgElectronLikelihoodTool> handle ("tool");
  ASSERT_SUCCESS (ASG_MAKE_ANA_TOOL (handle, AsgElectronLikelihoodTool));
  ASSERT_SUCCESS (handle.initialize ());
}

TEST (ToolMakeTest, make_EgammaCalibrationAndSmearingTool)
{
  asg::AnaToolHandle<CP::IEgammaCalibrationAndSmearingTool> handle ("tool");
  ASSERT_SUCCESS (handle.make ("CP::EgammaCalibrationAndSmearingTool"));
}

TEST (ToolMakeTest, makeNew_EgammaCalibrationAndSmearingTool)
{
  asg::AnaToolHandle<CP::IEgammaCalibrationAndSmearingTool> handle ("tool");
  ASSERT_SUCCESS (ASG_MAKE_ANA_TOOL (handle, CP::EgammaCalibrationAndSmearingTool));
}

TEST (ToolMakeTest, DISABLED_initialize_EgammaCalibrationAndSmearingTool)
{
  asg::AnaToolHandle<CP::IEgammaCalibrationAndSmearingTool> handle ("tool");
  ASSERT_SUCCESS (ASG_MAKE_ANA_TOOL (handle, CP::EgammaCalibrationAndSmearingTool));
  ASSERT_SUCCESS (handle.initialize ());
}

TEST (ToolMakeTest, DISABLED_make_IsolationSelectionTool)
{
  asg::AnaToolHandle<CP::IIsolationSelectionTool> handle ("tool");
  ASSERT_SUCCESS (handle.make ("CP::IsolationSelectionTool"));
}

TEST (ToolMakeTest, makeNew_IsolationSelectionTool)
{
  asg::AnaToolHandle<CP::IIsolationSelectionTool> handle ("tool");
  ASSERT_SUCCESS (ASG_MAKE_ANA_TOOL (handle, CP::IsolationSelectionTool));
}

TEST (ToolMakeTest, initialize_IsolationSelectionTool)
{
  asg::AnaToolHandle<CP::IIsolationSelectionTool> handle ("tool");
  ASSERT_SUCCESS (ASG_MAKE_ANA_TOOL (handle, CP::IsolationSelectionTool));
  ASSERT_SUCCESS (handle.initialize ());
}

TEST (ToolMakeTest, DISABLED_make_AsgElectronEfficiencyCorrectionTool)
{
  asg::AnaToolHandle<IAsgElectronEfficiencyCorrectionTool> handle ("tool");
  ASSERT_SUCCESS (handle.make ("AsgElectronEfficiencyCorrectionTool"));
}

TEST (ToolMakeTest, DISABLED_makeNew_AsgElectronEfficiencyCorrectionTool)
{
  asg::AnaToolHandle<IAsgElectronEfficiencyCorrectionTool> handle ("tool");
  ASSERT_SUCCESS (ASG_MAKE_ANA_TOOL (handle, AsgElectronEfficiencyCorrectionTool));
}

TEST (ToolMakeTest, DISABLED_initialize_AsgElectronEfficiencyCorrectionTool)
{
  asg::AnaToolHandle<IAsgElectronEfficiencyCorrectionTool> handle ("tool");
  ASSERT_SUCCESS (ASG_MAKE_ANA_TOOL (handle, AsgElectronEfficiencyCorrectionTool));
  ASSERT_SUCCESS (handle.initialize ());
}

TEST (ToolMakeTest, make_MuonSelectionTool)
{
  asg::AnaToolHandle<CP::IMuonSelectionTool> handle ("tool");
  ASSERT_SUCCESS (handle.make ("CP::MuonSelectionTool"));
}

TEST (ToolMakeTest, makeNew_MuonSelectionTool)
{
  asg::AnaToolHandle<CP::IMuonSelectionTool> handle ("tool");
  ASSERT_SUCCESS (ASG_MAKE_ANA_TOOL (handle, CP::MuonSelectionTool));
}

TEST (ToolMakeTest, initialize_MuonSelectionTool)
{
  asg::AnaToolHandle<CP::IMuonSelectionTool> handle ("tool");
  ASSERT_SUCCESS (ASG_MAKE_ANA_TOOL (handle, CP::MuonSelectionTool));
  ASSERT_SUCCESS (handle.initialize ());
}

TEST (ToolMakeTest, DISABLED_make_JetCalibrationTool)
{
  asg::AnaToolHandle<IJetCalibrationTool> handle ("tool");
  ASSERT_SUCCESS (handle.make ("JetCalibrationTool"));
}

TEST (ToolMakeTest, makeNew_JetCalibrationTool)
{
  asg::AnaToolHandle<IJetCalibrationTool> handle ("tool");
  ASSERT_SUCCESS (ASG_MAKE_ANA_TOOL (handle, JetCalibrationTool));
}

TEST (ToolMakeTest, DISABLED_initialize_JetCalibrationTool)
{
  asg::AnaToolHandle<IJetCalibrationTool> handle ("tool");
  ASSERT_SUCCESS (ASG_MAKE_ANA_TOOL (handle, JetCalibrationTool));
  ASSERT_SUCCESS (handle.initialize ());
}

TEST (ToolMakeTest, DISABLED_make_JetUncertaintiesTool)
{
  asg::AnaToolHandle<IJetUncertaintiesTool> handle ("tool");
  ASSERT_SUCCESS (handle.make ("JetUncertaintiesTool"));
}

TEST (ToolMakeTest, makeNew_JetUncertaintiesTool)
{
  asg::AnaToolHandle<IJetUncertaintiesTool> handle ("tool");
  ASSERT_SUCCESS (ASG_MAKE_ANA_TOOL (handle, JetUncertaintiesTool));
}

TEST (ToolMakeTest, DISABLED_initialize_JetUncertaintiesTool)
{
  asg::AnaToolHandle<IJetUncertaintiesTool> handle ("tool");
  ASSERT_SUCCESS (ASG_MAKE_ANA_TOOL (handle, JetUncertaintiesTool));
  ASSERT_SUCCESS (handle.initialize ());
}

TEST (ToolMakeTest, make_JERTool)
{
  asg::AnaToolHandle<IJERTool> handle ("tool");
  ASSERT_SUCCESS (handle.make ("JERTool"));
}

TEST (ToolMakeTest, makeNew_JERTool)
{
  asg::AnaToolHandle<IJERTool> handle ("tool");
  ASSERT_SUCCESS (ASG_MAKE_ANA_TOOL (handle, JERTool));
}

TEST (ToolMakeTest, initialize_JERTool)
{
  asg::AnaToolHandle<IJERTool> handle ("tool");
  ASSERT_SUCCESS (ASG_MAKE_ANA_TOOL (handle, JERTool));
  ASSERT_SUCCESS (handle.initialize ());
}

TEST (ToolMakeTest, DISABLED_make_JERSmearingTool)
{
  asg::AnaToolHandle<IJERSmearingTool> handle ("tool");
  ASSERT_SUCCESS (handle.make ("JERSmearingTool"));
}

TEST (ToolMakeTest, makeNew_JERSmearingTool)
{
  asg::AnaToolHandle<IJERSmearingTool> handle ("tool");
  ASSERT_SUCCESS (ASG_MAKE_ANA_TOOL (handle, JERSmearingTool));
}

TEST (ToolMakeTest, DISABLED_initialize_JERSmearingTool)
{
  asg::AnaToolHandle<IJERSmearingTool> handle ("tool");
  ASSERT_SUCCESS (ASG_MAKE_ANA_TOOL (handle, JERSmearingTool));
  ASSERT_SUCCESS (handle.initialize ());
}

TEST (ToolMakeTest, DISABLED_make_JetVertexTaggerTool)
{
  asg::AnaToolHandle<IJetUpdateJvt> handle ("tool");
  ASSERT_SUCCESS (handle.make ("JetVertexTaggerTool"));
}

TEST (ToolMakeTest, makeNew_JetVertexTaggerTool)
{
  asg::AnaToolHandle<IJetUpdateJvt> handle ("tool");
  ASSERT_SUCCESS (ASG_MAKE_ANA_TOOL (handle, JetVertexTaggerTool));
}

TEST (ToolMakeTest, DISABLED_initialize_JetVertexTaggerTool)
{
  asg::AnaToolHandle<IJetUpdateJvt> handle ("tool");
  ASSERT_SUCCESS (ASG_MAKE_ANA_TOOL (handle, JetVertexTaggerTool));
  ASSERT_SUCCESS (handle.initialize ());
}

TEST (ToolMakeTest, make_BTaggingSelectionTool)
{
  asg::AnaToolHandle<IBTaggingSelectionTool> handle ("tool");
  ASSERT_SUCCESS (handle.make ("BTaggingSelectionTool"));
}

TEST (ToolMakeTest, makeNew_BTaggingSelectionTool)
{
  asg::AnaToolHandle<IBTaggingSelectionTool> handle ("tool");
  ASSERT_SUCCESS (ASG_MAKE_ANA_TOOL (handle, BTaggingSelectionTool));
}

TEST (ToolMakeTest, DISABLED_initialize_BTaggingSelectionTool)
{
  asg::AnaToolHandle<IBTaggingSelectionTool> handle ("tool");
  ASSERT_SUCCESS (ASG_MAKE_ANA_TOOL (handle, BTaggingSelectionTool));
  ASSERT_SUCCESS (handle.initialize ());
}

TEST (ToolMakeTest, make_JetCleaningTool)
{
  asg::AnaToolHandle<IJetSelector> handle ("tool");
  ASSERT_SUCCESS (handle.make ("JetCleaningTool"));
}

TEST (ToolMakeTest, makeNew_JetCleaningTool)
{
  asg::AnaToolHandle<IJetSelector> handle ("tool");
  ASSERT_SUCCESS (ASG_MAKE_ANA_TOOL (handle, JetCleaningTool));
}

TEST (ToolMakeTest, initialize_JetCleaningTool)
{
  asg::AnaToolHandle<IJetSelector> handle ("tool");
  ASSERT_SUCCESS (ASG_MAKE_ANA_TOOL (handle, JetCleaningTool));
  ASSERT_SUCCESS (handle.initialize ());
}

TEST (ToolMakeTest, DISABLED_make_BTaggingEfficiencyTool)
{
  asg::AnaToolHandle<IBTaggingEfficiencyTool> handle ("tool");
  ASSERT_SUCCESS (handle.make ("BTaggingEfficiencyTool"));
}

TEST (ToolMakeTest, DISABLED_makeNew_BTaggingEfficiencyTool)
{
  asg::AnaToolHandle<IBTaggingEfficiencyTool> handle ("tool");
  ASSERT_SUCCESS (ASG_MAKE_ANA_TOOL (handle, BTaggingEfficiencyTool));
}

TEST (ToolMakeTest, DISABLED_initialize_BTaggingEfficiencyTool)
{
  asg::AnaToolHandle<IBTaggingEfficiencyTool> handle ("tool");
  ASSERT_SUCCESS (ASG_MAKE_ANA_TOOL (handle, BTaggingEfficiencyTool));
  ASSERT_SUCCESS (handle.initialize ());
}

TEST (ToolMakeTest, DISABLED_make_JetJvtEfficiency)
{
  asg::AnaToolHandle<CP::IJetJvtEfficiency> handle ("tool");
  ASSERT_SUCCESS (handle.make ("CP::JetJvtEfficiency"));
}

TEST (ToolMakeTest, makeNew_JetJvtEfficiency)
{
  asg::AnaToolHandle<CP::IJetJvtEfficiency> handle ("tool");
  ASSERT_SUCCESS (ASG_MAKE_ANA_TOOL (handle, CP::JetJvtEfficiency));
}

TEST (ToolMakeTest, initialize_JetJvtEfficiency)
{
  asg::AnaToolHandle<CP::IJetJvtEfficiency> handle ("tool");
  ASSERT_SUCCESS (ASG_MAKE_ANA_TOOL (handle, CP::JetJvtEfficiency));
  ASSERT_SUCCESS (handle.initialize ());
}

TEST (ToolMakeTest, make_AltMuJetOverlapTool)
{
  asg::AnaToolHandle<ORUtils::IOverlapTool> handle ("tool");
  ASSERT_SUCCESS (handle.make ("ORUtils::AltMuJetOverlapTool"));
}

TEST (ToolMakeTest, makeNew_AltMuJetOverlapTool)
{
  asg::AnaToolHandle<ORUtils::IOverlapTool> handle ("tool");
  ASSERT_SUCCESS (ASG_MAKE_ANA_TOOL (handle, ORUtils::AltMuJetOverlapTool));
}

TEST (ToolMakeTest, initialize_AltMuJetOverlapTool)
{
  asg::AnaToolHandle<ORUtils::IOverlapTool> handle ("tool");
  ASSERT_SUCCESS (ASG_MAKE_ANA_TOOL (handle, ORUtils::AltMuJetOverlapTool));
  ASSERT_SUCCESS (handle.initialize ());
}

TEST (ToolMakeTest, make_DeltaROverlapTool)
{
  asg::AnaToolHandle<ORUtils::IOverlapTool> handle ("tool");
  ASSERT_SUCCESS (handle.make ("ORUtils::DeltaROverlapTool"));
}

TEST (ToolMakeTest, makeNew_DeltaROverlapTool)
{
  asg::AnaToolHandle<ORUtils::IOverlapTool> handle ("tool");
  ASSERT_SUCCESS (ASG_MAKE_ANA_TOOL (handle, ORUtils::DeltaROverlapTool));
}

TEST (ToolMakeTest, initialize_DeltaROverlapTool)
{
  asg::AnaToolHandle<ORUtils::IOverlapTool> handle ("tool");
  ASSERT_SUCCESS (ASG_MAKE_ANA_TOOL (handle, ORUtils::DeltaROverlapTool));
  ASSERT_SUCCESS (handle.initialize ());
}

TEST (ToolMakeTest, make_EleJetOverlapTool)
{
  asg::AnaToolHandle<ORUtils::IOverlapTool> handle ("tool");
  ASSERT_SUCCESS (handle.make ("ORUtils::EleJetOverlapTool"));
}

TEST (ToolMakeTest, makeNew_EleJetOverlapTool)
{
  asg::AnaToolHandle<ORUtils::IOverlapTool> handle ("tool");
  ASSERT_SUCCESS (ASG_MAKE_ANA_TOOL (handle, ORUtils::EleJetOverlapTool));
}

TEST (ToolMakeTest, initialize_EleJetOverlapTool)
{
  asg::AnaToolHandle<ORUtils::IOverlapTool> handle ("tool");
  ASSERT_SUCCESS (ASG_MAKE_ANA_TOOL (handle, ORUtils::EleJetOverlapTool));
  ASSERT_SUCCESS (handle.initialize ());
}

TEST (ToolMakeTest, make_EleMuSharedTrkOverlapTool)
{
  asg::AnaToolHandle<ORUtils::IOverlapTool> handle ("tool");
  ASSERT_SUCCESS (handle.make ("ORUtils::EleMuSharedTrkOverlapTool"));
}

TEST (ToolMakeTest, makeNew_EleMuSharedTrkOverlapTool)
{
  asg::AnaToolHandle<ORUtils::IOverlapTool> handle ("tool");
  ASSERT_SUCCESS (ASG_MAKE_ANA_TOOL (handle, ORUtils::EleMuSharedTrkOverlapTool));
}

TEST (ToolMakeTest, initialize_EleMuSharedTrkOverlapTool)
{
  asg::AnaToolHandle<ORUtils::IOverlapTool> handle ("tool");
  ASSERT_SUCCESS (ASG_MAKE_ANA_TOOL (handle, ORUtils::EleMuSharedTrkOverlapTool));
  ASSERT_SUCCESS (handle.initialize ());
}

TEST (ToolMakeTest, make_MuJetOverlapTool)
{
  asg::AnaToolHandle<ORUtils::IOverlapTool> handle ("tool");
  ASSERT_SUCCESS (handle.make ("ORUtils::MuJetOverlapTool"));
}

TEST (ToolMakeTest, makeNew_MuJetOverlapTool)
{
  asg::AnaToolHandle<ORUtils::IOverlapTool> handle ("tool");
  ASSERT_SUCCESS (ASG_MAKE_ANA_TOOL (handle, ORUtils::MuJetOverlapTool));
}

TEST (ToolMakeTest, initialize_MuJetOverlapTool)
{
  asg::AnaToolHandle<ORUtils::IOverlapTool> handle ("tool");
  ASSERT_SUCCESS (ASG_MAKE_ANA_TOOL (handle, ORUtils::MuJetOverlapTool));
  ASSERT_SUCCESS (handle.initialize ());
}

TEST (ToolMakeTest, make_TauLooseEleOverlapTool)
{
  asg::AnaToolHandle<ORUtils::IOverlapTool> handle ("tool");
  ASSERT_SUCCESS (handle.make ("ORUtils::TauLooseEleOverlapTool"));
}

TEST (ToolMakeTest, makeNew_TauLooseEleOverlapTool)
{
  asg::AnaToolHandle<ORUtils::IOverlapTool> handle ("tool");
  ASSERT_SUCCESS (ASG_MAKE_ANA_TOOL (handle, ORUtils::TauLooseEleOverlapTool));
}

TEST (ToolMakeTest, initialize_TauLooseEleOverlapTool)
{
  asg::AnaToolHandle<ORUtils::IOverlapTool> handle ("tool");
  ASSERT_SUCCESS (ASG_MAKE_ANA_TOOL (handle, ORUtils::TauLooseEleOverlapTool));
  ASSERT_SUCCESS (handle.initialize ());
}

TEST (ToolMakeTest, make_TauLooseMuOverlapTool)
{
  asg::AnaToolHandle<ORUtils::IOverlapTool> handle ("tool");
  ASSERT_SUCCESS (handle.make ("ORUtils::TauLooseMuOverlapTool"));
}

TEST (ToolMakeTest, makeNew_TauLooseMuOverlapTool)
{
  asg::AnaToolHandle<ORUtils::IOverlapTool> handle ("tool");
  ASSERT_SUCCESS (ASG_MAKE_ANA_TOOL (handle, ORUtils::TauLooseMuOverlapTool));
}

TEST (ToolMakeTest, initialize_TauLooseMuOverlapTool)
{
  asg::AnaToolHandle<ORUtils::IOverlapTool> handle ("tool");
  ASSERT_SUCCESS (ASG_MAKE_ANA_TOOL (handle, ORUtils::TauLooseMuOverlapTool));
  ASSERT_SUCCESS (handle.initialize ());
}

TEST (ToolMakeTest, make_OverlapRemovalTool)
{
  asg::AnaToolHandle<ORUtils::IOverlapRemovalTool> handle ("tool");
  ASSERT_SUCCESS (handle.make ("ORUtils::OverlapRemovalTool"));
}

TEST (ToolMakeTest, makeNew_OverlapRemovalTool)
{
  asg::AnaToolHandle<ORUtils::IOverlapRemovalTool> handle ("tool");
  ASSERT_SUCCESS (ASG_MAKE_ANA_TOOL (handle, ORUtils::OverlapRemovalTool));
}

TEST (ToolMakeTest, initialize_OverlapRemovalTool)
{
  asg::AnaToolHandle<ORUtils::IOverlapRemovalTool> handle ("tool");
  ASSERT_SUCCESS (ASG_MAKE_ANA_TOOL (handle, ORUtils::OverlapRemovalTool));
  ASSERT_SUCCESS (handle.initialize ());
}

TEST (ToolMakeTest, DISABLED_make_METMaker)
{
  asg::AnaToolHandle<IMETMaker> handle ("tool");
  ASSERT_SUCCESS (handle.make ("met::METMaker"));
}

TEST (ToolMakeTest, makeNew_METMaker)
{
  asg::AnaToolHandle<IMETMaker> handle ("tool");
  ASSERT_SUCCESS (ASG_MAKE_ANA_TOOL (handle, met::METMaker));
}

TEST (ToolMakeTest, initialize_METMaker)
{
  asg::AnaToolHandle<IMETMaker> handle ("tool");
  ASSERT_SUCCESS (ASG_MAKE_ANA_TOOL (handle, met::METMaker));
  ASSERT_SUCCESS (handle.initialize ());
}

TEST (ToolMakeTest, DISABLED_make_METSystematicsTool)
{
  asg::AnaToolHandle<IMETSystematicsTool> handle ("tool");
  ASSERT_SUCCESS (handle.make ("met::METSystematicsTool"));
}

TEST (ToolMakeTest, makeNew_METSystematicsTool)
{
  asg::AnaToolHandle<IMETSystematicsTool> handle ("tool");
  ASSERT_SUCCESS (ASG_MAKE_ANA_TOOL (handle, met::METSystematicsTool));
}

TEST (ToolMakeTest, initialize_METSystematicsTool)
{
  asg::AnaToolHandle<IMETSystematicsTool> handle ("tool");
  ASSERT_SUCCESS (ASG_MAKE_ANA_TOOL (handle, met::METSystematicsTool));
  ASSERT_SUCCESS (handle.initialize ());
}

TEST (ToolMakeTest, make_MuonCalibrationAndSmearingTool)
{
  asg::AnaToolHandle<CP::IMuonCalibrationAndSmearingTool> handle ("tool");
  ASSERT_SUCCESS (handle.make ("CP::MuonCalibrationAndSmearingTool"));
}

TEST (ToolMakeTest, makeNew_MuonCalibrationAndSmearingTool)
{
  asg::AnaToolHandle<CP::IMuonCalibrationAndSmearingTool> handle ("tool");
  ASSERT_SUCCESS (ASG_MAKE_ANA_TOOL (handle, CP::MuonCalibrationAndSmearingTool));
}

TEST (ToolMakeTest, initialize_MuonCalibrationAndSmearingTool)
{
  asg::AnaToolHandle<CP::IMuonCalibrationAndSmearingTool> handle ("tool");
  ASSERT_SUCCESS (ASG_MAKE_ANA_TOOL (handle, CP::MuonCalibrationAndSmearingTool));
  ASSERT_SUCCESS (handle.initialize ());
}

TEST (ToolMakeTest, DISABLED_make_MuonEfficiencyScaleFactors)
{
  asg::AnaToolHandle<CP::IMuonEfficiencyScaleFactors> handle ("tool");
  ASSERT_SUCCESS (handle.make ("CP::MuonEfficiencyScaleFactors"));
}

TEST (ToolMakeTest, makeNew_MuonEfficiencyScaleFactors)
{
  asg::AnaToolHandle<CP::IMuonEfficiencyScaleFactors> handle ("tool");
  ASSERT_SUCCESS (ASG_MAKE_ANA_TOOL (handle, CP::MuonEfficiencyScaleFactors));
}

TEST (ToolMakeTest, DISABLED_initialize_MuonEfficiencyScaleFactors)
{
  asg::AnaToolHandle<CP::IMuonEfficiencyScaleFactors> handle ("tool");
  ASSERT_SUCCESS (ASG_MAKE_ANA_TOOL (handle, CP::MuonEfficiencyScaleFactors));
  ASSERT_SUCCESS (handle.initialize ());
}

TEST (ToolMakeTest, make_ElectronPhotonShowerShapeFudgeTool)
{
  asg::AnaToolHandle<IElectronPhotonShowerShapeFudgeTool> handle ("tool");
  ASSERT_SUCCESS (handle.make ("ElectronPhotonShowerShapeFudgeTool"));
}

TEST (ToolMakeTest, makeNew_ElectronPhotonShowerShapeFudgeTool)
{
  asg::AnaToolHandle<IElectronPhotonShowerShapeFudgeTool> handle ("tool");
  ASSERT_SUCCESS (ASG_MAKE_ANA_TOOL (handle, ElectronPhotonShowerShapeFudgeTool));
}

TEST (ToolMakeTest, DISABLED_initialize_ElectronPhotonShowerShapeFudgeTool)
{
  asg::AnaToolHandle<IElectronPhotonShowerShapeFudgeTool> handle ("tool");
  ASSERT_SUCCESS (ASG_MAKE_ANA_TOOL (handle, ElectronPhotonShowerShapeFudgeTool));
  ASSERT_SUCCESS (handle.initialize ());
}

TEST (ToolMakeTest, DISABLED_make_IsolationCorrectionTool)
{
  asg::AnaToolHandle<CP::IIsolationCorrectionTool> handle ("tool");
  ASSERT_SUCCESS (handle.make ("CP::IsolationCorrectionTool"));
}

TEST (ToolMakeTest, makeNew_IsolationCorrectionTool)
{
  asg::AnaToolHandle<CP::IIsolationCorrectionTool> handle ("tool");
  ASSERT_SUCCESS (ASG_MAKE_ANA_TOOL (handle, CP::IsolationCorrectionTool));
}

TEST (ToolMakeTest, DISABLED_initialize_IsolationCorrectionTool)
{
  asg::AnaToolHandle<CP::IIsolationCorrectionTool> handle ("tool");
  ASSERT_SUCCESS (ASG_MAKE_ANA_TOOL (handle, CP::IsolationCorrectionTool));
  ASSERT_SUCCESS (handle.initialize ());
}

TEST (ToolMakeTest, make_AsgPhotonIsEMSelector)
{
  asg::AnaToolHandle<IAsgPhotonIsEMSelector> handle ("tool");
  ASSERT_SUCCESS (handle.make ("AsgPhotonIsEMSelector"));
}

TEST (ToolMakeTest, makeNew_AsgPhotonIsEMSelector)
{
  asg::AnaToolHandle<IAsgPhotonIsEMSelector> handle ("tool");
  ASSERT_SUCCESS (ASG_MAKE_ANA_TOOL (handle, AsgPhotonIsEMSelector));
}

TEST (ToolMakeTest, initialize_AsgPhotonIsEMSelector)
{
  asg::AnaToolHandle<IAsgPhotonIsEMSelector> handle ("tool");
  ASSERT_SUCCESS (ASG_MAKE_ANA_TOOL (handle, AsgPhotonIsEMSelector));
  ASSERT_SUCCESS (handle.initialize ());
}

TEST (ToolMakeTest, make_AsgPhotonEfficiencyCorrectionTool)
{
  asg::AnaToolHandle<IAsgPhotonEfficiencyCorrectionTool> handle ("tool");
  ASSERT_SUCCESS (handle.make ("AsgPhotonEfficiencyCorrectionTool"));
}

TEST (ToolMakeTest, makeNew_AsgPhotonEfficiencyCorrectionTool)
{
  asg::AnaToolHandle<IAsgPhotonEfficiencyCorrectionTool> handle ("tool");
  ASSERT_SUCCESS (ASG_MAKE_ANA_TOOL (handle, AsgPhotonEfficiencyCorrectionTool));
}

TEST (ToolMakeTest, DISABLED_initialize_AsgPhotonEfficiencyCorrectionTool)
{
  asg::AnaToolHandle<IAsgPhotonEfficiencyCorrectionTool> handle ("tool");
  ASSERT_SUCCESS (ASG_MAKE_ANA_TOOL (handle, AsgPhotonEfficiencyCorrectionTool));
  ASSERT_SUCCESS (handle.initialize ());
}

TEST (ToolMakeTest, make_PileupReweightingTool)
{
  asg::AnaToolHandle<CP::IPileupReweightingTool> handle ("tool");
  ASSERT_SUCCESS (handle.make ("CP::PileupReweightingTool"));
}

TEST (ToolMakeTest, makeNew_PileupReweightingTool)
{
  asg::AnaToolHandle<CP::IPileupReweightingTool> handle ("tool");
  ASSERT_SUCCESS (ASG_MAKE_ANA_TOOL (handle, CP::PileupReweightingTool));
}

TEST (ToolMakeTest, initialize_PileupReweightingTool)
{
  asg::AnaToolHandle<CP::IPileupReweightingTool> handle ("tool");
  ASSERT_SUCCESS (ASG_MAKE_ANA_TOOL (handle, CP::PileupReweightingTool));
  ASSERT_SUCCESS (handle.initialize ());
}

TEST (ToolMakeTest, make_TauSmearingTool)
{
  asg::AnaToolHandle<TauAnalysisTools::ITauSmearingTool> handle ("tool");
  ASSERT_SUCCESS (handle.make ("TauAnalysisTools::TauSmearingTool"));
}

TEST (ToolMakeTest, makeNew_TauSmearingTool)
{
  asg::AnaToolHandle<TauAnalysisTools::ITauSmearingTool> handle ("tool");
  ASSERT_SUCCESS (ASG_MAKE_ANA_TOOL (handle, TauAnalysisTools::TauSmearingTool));
}

TEST (ToolMakeTest, initialize_TauSmearingTool)
{
  asg::AnaToolHandle<TauAnalysisTools::ITauSmearingTool> handle ("tool");
  ASSERT_SUCCESS (ASG_MAKE_ANA_TOOL (handle, TauAnalysisTools::TauSmearingTool));
  ASSERT_SUCCESS (handle.initialize ());
}

TEST (ToolMakeTest, make_TauSelectionTool)
{
  asg::AnaToolHandle<TauAnalysisTools::ITauSelectionTool> handle ("tool");
  ASSERT_SUCCESS (handle.make ("TauAnalysisTools::TauSelectionTool"));
}

TEST (ToolMakeTest, makeNew_TauSelectionTool)
{
  asg::AnaToolHandle<TauAnalysisTools::ITauSelectionTool> handle ("tool");
  ASSERT_SUCCESS (ASG_MAKE_ANA_TOOL (handle, TauAnalysisTools::TauSelectionTool));
}

TEST (ToolMakeTest, initialize_TauSelectionTool)
{
  asg::AnaToolHandle<TauAnalysisTools::ITauSelectionTool> handle ("tool");
  ASSERT_SUCCESS (ASG_MAKE_ANA_TOOL (handle, TauAnalysisTools::TauSelectionTool));
  ASSERT_SUCCESS (handle.initialize ());
}

TEST (ToolMakeTest, make_TauEfficiencyCorrectionsTool)
{
  asg::AnaToolHandle<TauAnalysisTools::ITauEfficiencyCorrectionsTool> handle ("tool");
  ASSERT_SUCCESS (handle.make ("TauAnalysisTools::TauEfficiencyCorrectionsTool"));
}

TEST (ToolMakeTest, makeNew_TauEfficiencyCorrectionsTool)
{
  asg::AnaToolHandle<TauAnalysisTools::ITauEfficiencyCorrectionsTool> handle ("tool");
  ASSERT_SUCCESS (ASG_MAKE_ANA_TOOL (handle, TauAnalysisTools::TauEfficiencyCorrectionsTool));
}

TEST (ToolMakeTest, initialize_TauEfficiencyCorrectionsTool)
{
  asg::AnaToolHandle<TauAnalysisTools::ITauEfficiencyCorrectionsTool> handle ("tool");
  ASSERT_SUCCESS (ASG_MAKE_ANA_TOOL (handle, TauAnalysisTools::TauEfficiencyCorrectionsTool));
  ASSERT_SUCCESS (handle.initialize ());
}

TEST (ToolMakeTest, DISABLED_make_TauTruthMatchingTool)
{
  asg::AnaToolHandle<TauAnalysisTools::ITauTruthMatchingTool> handle ("tool");
  ASSERT_SUCCESS (handle.make ("TauAnalysisTools::TauTruthMatchingTool"));
}

TEST (ToolMakeTest, makeNew_TauTruthMatchingTool)
{
  asg::AnaToolHandle<TauAnalysisTools::ITauTruthMatchingTool> handle ("tool");
  ASSERT_SUCCESS (ASG_MAKE_ANA_TOOL (handle, TauAnalysisTools::TauTruthMatchingTool));
}

TEST (ToolMakeTest, initialize_TauTruthMatchingTool)
{
  asg::AnaToolHandle<TauAnalysisTools::ITauTruthMatchingTool> handle ("tool");
  ASSERT_SUCCESS (ASG_MAKE_ANA_TOOL (handle, TauAnalysisTools::TauTruthMatchingTool));
  ASSERT_SUCCESS (handle.initialize ());
}

TEST (ToolMakeTest, make_xAODConfigTool)
{
  asg::AnaToolHandle<TrigConf::ITrigConfigTool> handle ("tool");
  ASSERT_SUCCESS (handle.make ("TrigConf::xAODConfigTool"));
}

TEST (ToolMakeTest, makeNew_xAODConfigTool)
{
  asg::AnaToolHandle<TrigConf::ITrigConfigTool> handle ("tool");
  ASSERT_SUCCESS (ASG_MAKE_ANA_TOOL (handle, TrigConf::xAODConfigTool));
}

TEST (ToolMakeTest, initialize_xAODConfigTool)
{
  asg::AnaToolHandle<TrigConf::ITrigConfigTool> handle ("tool");
  ASSERT_SUCCESS (ASG_MAKE_ANA_TOOL (handle, TrigConf::xAODConfigTool));
  ASSERT_SUCCESS (handle.initialize ());
}

// TEST (ToolMakeTest, make_TrigDecisionTool)
// {
//   asg::AnaToolHandle<Trig::TrigDecisionTool> handle ("tool");
//   ASSERT_SUCCESS (handle.make ("Trig::TrigDecisionTool"));
// }

// TEST (ToolMakeTest, makeNew_TrigDecisionTool)
// {
//   asg::AnaToolHandle<Trig::TrigDecisionTool> handle ("tool");
//   ASSERT_SUCCESS (ASG_MAKE_ANA_TOOL (handle, Trig::TrigDecisionTool));
// }

// TEST (ToolMakeTest, DISABLED_initialize_TrigDecisionTool)
// {
//   asg::AnaToolHandle<Trig::TrigDecisionTool> handle ("tool");
//   ASSERT_SUCCESS (ASG_MAKE_ANA_TOOL (handle, Trig::TrigDecisionTool));
//   ASSERT_SUCCESS (handle.initialize ());
// }

TEST (ToolMakeTest, DISABLED_make_MuonTriggerScaleFactors)
{
  asg::AnaToolHandle<CP::IMuonTriggerScaleFactors> handle ("tool");
  ASSERT_SUCCESS (handle.make ("CP::MuonTriggerScaleFactors"));
}

TEST (ToolMakeTest, makeNew_MuonTriggerScaleFactors)
{
  asg::AnaToolHandle<CP::IMuonTriggerScaleFactors> handle ("tool");
  ASSERT_SUCCESS (ASG_MAKE_ANA_TOOL (handle, CP::MuonTriggerScaleFactors));
}

TEST (ToolMakeTest, initialize_MuonTriggerScaleFactors)
{
  asg::AnaToolHandle<CP::IMuonTriggerScaleFactors> handle ("tool");
  ASSERT_SUCCESS (ASG_MAKE_ANA_TOOL (handle, CP::MuonTriggerScaleFactors));
  ASSERT_SUCCESS (handle.initialize ());
}

int main (int argc, char **argv)
{
#ifdef ROOTCORE
  StatusCode::enableFailure();
  ANA_CHECK (xAOD::Init ());
#endif
  ::testing::InitGoogleTest (&argc, argv);
  return RUN_ALL_TESTS();
}
