/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/** Unit tests for AssociationUtils tools in AnaToolHandle
 */

#include "AsgTesting/UnitTest.h"
#include "AsgTools/MessageCheck.h"
#include "gtest/gtest.h"
#ifdef ROOTCORE
#include "xAODRootAccess/Init.h"
#endif

#include "AsgTools/AnaToolHandle.h"

#include "AssociationUtils/IOverlapTool.h"
#include "AssociationUtils/IOverlapRemovalTool.h"

namespace
{
  using namespace asg::msgUserCode;

  TEST(AnaToolHandleTest, OverlapTools)
  {
    auto toolStrings = {
      "ORUtils::DeltaROverlapTool/DrORT1",
      "ORUtils::ObjLinkOverlapTool/LinkORT1",
      "ORUtils::EleEleOverlapTool/EleEleORT1",
      "ORUtils::EleJetOverlapTool/EleJetORT1",
      "ORUtils::EleMuSharedTrkOverlapTool/EleMuORT1",
      "ORUtils::MuJetOverlapTool/MuJetORT1",
      "ORUtils::AltMuJetOverlapTool/AltMuJetORT1",
      "ORUtils::TauLooseEleOverlapTool/TauEleORT1",
      "ORUtils::TauLooseMuOverlapTool/TauMuORT1",
      "ORUtils::TauJetOverlapTool/TauJetORT1",
      "ORUtils::TauAntiTauJetOverlapTool/AntiTauJetORT1",
    };

    // Try to create and initialize each tool with the AnaToolHandle
    for(const auto& toolStr : toolStrings) {
      asg::AnaToolHandle<ORUtils::IOverlapTool> handle(toolStr);
      EXPECT_SUCCESS( handle.initialize() );
    }
  }

  TEST(AnaToolHandleTest, MasterTools)
  {
    auto toolStrings = {
      "ORUtils::OverlapRemovalTool/ORTool1",
    };
    // Try to create and initialize each tool with the AnaToolHandle
    for(const auto& toolStr : toolStrings) {
      asg::AnaToolHandle<ORUtils::IOverlapRemovalTool> handle(toolStr);
      EXPECT_SUCCESS( handle.initialize() );
    }
  }

  // Try to assign an overlap tool to a master tool with AnaToolHandle
  TEST(AnaToolHandleTest, ToolAssignment)
  {
    // Specify the tool types and names
    const auto masterToolName = "ORUtils::OverlapRemovalTool/ORTool2";
    const auto overlapToolName = "ORUtils::DeltaROverlapTool/ORTool2.DrORT2";
    const auto key = "EleJetORT";
    // Create the AnaToolHandles
    asg::AnaToolHandle<ORUtils::IOverlapRemovalTool> masterHandle(masterToolName);
    asg::AnaToolHandle<ORUtils::IOverlapTool> overlapHandle(overlapToolName);
    // Hook them up and initialize
    ASSERT_SUCCESS( masterHandle.setProperty(key, overlapHandle) );
    ASSERT_SUCCESS( masterHandle.initialize() );
    ASSERT_TRUE( masterHandle.get() != nullptr );
  }

  // Try to assign an overlap tool to a master tool with AnaToolHandle.
  // Same as above except that we initialize the overlap handle differently.
  // This test is designed to mimic what happens in the ToolBox initialization.
  TEST(AnaToolHandleTest, ToolAssignment2)
  {
    // Specify the tool types and names
    const auto masterToolName = "ORUtils::OverlapRemovalTool/ORTool3";
    const auto overlapToolName = "ORUtils::DeltaROverlapTool/ORTool3.DrORT3";
    const auto key = "EleJetORT";
    // Create the AnaToolHandles
    asg::AnaToolHandle<ORUtils::IOverlapRemovalTool> masterHandle(masterToolName);
    asg::AnaToolHandle<ORUtils::IOverlapTool> overlapHandle("");
    // Set type and name of the handle
    overlapHandle.setTypeAndName(overlapToolName);
    // Hook them up and initialize
    ASSERT_SUCCESS( masterHandle.setProperty(key, overlapHandle) );
    ASSERT_SUCCESS( masterHandle.initialize() );
    ASSERT_TRUE( masterHandle.get() != nullptr );
  }

}

int main(int argc, char** argv)
{
#ifdef ROOTCORE
  StatusCode::enableFailure();
  ANA_CHECK( xAOD::Init() );
#endif
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
