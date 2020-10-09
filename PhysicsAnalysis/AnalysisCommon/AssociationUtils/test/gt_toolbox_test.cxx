/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/** Unit tests for the ORUtils::ToolBox
 */

// Infrastructure
#include "AsgTools/UnitTest.h"
#include "AsgMessaging/MessageCheck.h"
#include "gtest/gtest.h"
#ifdef ROOTCORE
#include "xAODRootAccess/Init.h"
#endif

// Local includes
#include "AssociationUtils/ToolBox.h"


namespace
{
  using namespace ORUtils;
  using namespace asg::msgUserCode;

  TEST(ToolBoxTest, Empty)
  {
    ToolBox tbox;
    EXPECT_TRUE( tbox.eleEleORT.empty() );
    EXPECT_TRUE( tbox.eleMuORT.empty() );
    EXPECT_TRUE( tbox.eleJetORT.empty() );
    EXPECT_TRUE( tbox.muJetORT.empty() );
    EXPECT_TRUE( tbox.tauEleORT.empty() );
    EXPECT_TRUE( tbox.tauMuORT.empty() );
    EXPECT_TRUE( tbox.tauJetORT.empty() );
    EXPECT_TRUE( tbox.phoEleORT.empty() );
    EXPECT_TRUE( tbox.phoMuORT.empty() );
    EXPECT_TRUE( tbox.phoJetORT.empty() );
    EXPECT_TRUE( tbox.eleFatJetORT.empty() );
    EXPECT_TRUE( tbox.jetFatJetORT.empty() );
    EXPECT_TRUE( tbox.parent() == nullptr );
  }

  TEST(ToolBoxTest, InitOneSubTool)
  {
    ToolBox tbox;
    tbox.eleMuORT = ToolBox::OverlapHandle_t("ORUtils::DeltaROverlapTool/EleMuORT");
    tbox.masterTool = ToolBox::MasterHandle_t("ORUtils::OverlapRemovalTool");
    EXPECT_SUCCESS( tbox.setGlobalProperty("OutputLevel", MSG::WARNING) );
    ASSERT_SUCCESS( tbox.initialize() );
    auto masterAsgTool = dynamic_cast<asg::AsgTool*>( tbox.masterTool.get() );
    ASSERT_TRUE( masterAsgTool != nullptr );
#ifdef ROOTCORE
    // Compare master's sub-tool to the created sub-tool
    using htype_t = ToolHandle<IOverlapTool>;
    const auto& subHandle = *masterAsgTool->getProperty<htype_t>("EleMuORT");
    ASSERT_EQ( &(*subHandle), tbox.eleMuORT.get() );
#else
    // Haven't figured out how to do this in Athena without errors
#endif
  }

  // TODO: Test global property setting for full list of handles

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
