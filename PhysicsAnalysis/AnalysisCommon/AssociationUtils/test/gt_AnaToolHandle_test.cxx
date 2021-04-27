/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/** Unit tests for AssociationUtils tools in StandaloneToolHandle
 */

#include "AsgTesting/UnitTest.h"
#include "AsgMessaging/MessageCheck.h"
#include "gtest/gtest.h"
#ifdef ROOTCORE
#include "xAODRootAccess/Init.h"
#endif

#include "AsgTools/StandaloneToolHandle.h"

#include "AssociationUtils/IOverlapTool.h"
#include "AssociationUtils/IOverlapRemovalTool.h"

namespace
{
  using namespace asg::msgUserCode;

  TEST(StandaloneToolHandleTest, OverlapTools)
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

    // Try to create and initialize each tool with the StandaloneToolHandle
    for(const auto& toolStr : toolStrings) {
      asg::StandaloneToolHandle<ORUtils::IOverlapTool> handle(toolStr);
      EXPECT_SUCCESS( handle.initialize() );
    }
  }

  TEST(StandaloneToolHandleTest, MasterTools)
  {
    auto toolStrings = {
      "ORUtils::OverlapRemovalTool/ORTool1",
    };
    // Try to create and initialize each tool with the StandaloneToolHandle
    for(const auto& toolStr : toolStrings) {
      asg::StandaloneToolHandle<ORUtils::IOverlapRemovalTool> handle(toolStr);
      EXPECT_SUCCESS( handle.initialize() );
    }
  }

  // Try to assign an overlap tool to a master tool with StandaloneToolHandle
  TEST(StandaloneToolHandleTest, ToolAssignment)
  {
    // Specify the tool types and names
    const auto masterToolName = "ORUtils::OverlapRemovalTool/ORTool2";
    const auto overlapToolType = "ORUtils::DeltaROverlapTool";
    const auto key = "EleJetORT";
    // Create the StandaloneToolHandles
    asg::StandaloneToolHandle<ORUtils::IOverlapRemovalTool> masterHandle(masterToolName);
    // Hook them up and initialize
    ASSERT_SUCCESS( masterHandle.createPrivateTool(key, overlapToolType) );
    ASSERT_SUCCESS( masterHandle.initialize() );
    ASSERT_TRUE( masterHandle.get() != nullptr );
  }

  // Try to assign an overlap tool to a master tool with StandaloneToolHandle.
  // Same as above except that we initialize the overlap handle differently.
  // This test is designed to mimic what happens in the ToolBox initialization.
  TEST(StandaloneToolHandleTest, ToolAssignment2)
  {
    // Specify the tool types and names
    const auto masterToolName = "ORUtils::OverlapRemovalTool/ORTool3";
    const auto overlapToolType = "ORUtils::DeltaROverlapTool";
    const auto key = "EleJetORT";
    // Create the StandaloneToolHandles
    asg::StandaloneToolHandle<ORUtils::IOverlapRemovalTool> masterHandle(masterToolName);
    // Hook up the sub-tool
    ASSERT_SUCCESS( masterHandle.createPrivateTool(key, overlapToolType) );
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
