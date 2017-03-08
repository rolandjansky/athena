/** @brief Unit tests for c++ config helper functions.
 *
 *  @todo Test more features:
 *    - ORFlags doXXX object flags => check configured tools
 *    - Global config options: labels, boostedLeptons, outputPassValue
 *    - Pre-configuration and post-configuration of tools
 */

// Infrastructure
#include "AsgTools/UnitTest.h"
#include "AsgTools/MessageCheck.h"
#include "gtest/gtest.h"
#ifdef ROOTCORE
#include "xAODRootAccess/Init.h"
#endif

// Local includes
#include "AssociationUtils/OverlapRemovalInit.h"

namespace
{
  using namespace ORUtils;
  using namespace asg::msgUserCode;

  TEST(OverlapRemovalInitTest, CurrentHelper)
  {
    const std::string masterName = "MyORT";
    const std::string inputLabel = "MyInputLabel";
    const std::string outputLabel = "MyOutputLabel";
    ORUtils::ORFlags flags(masterName, inputLabel, outputLabel);
    ORUtils::ToolBox tools;
    EXPECT_SUCCESS( ORUtils::recommendedTools(flags, tools) );
    EXPECT_SUCCESS( tools.initialize() );
  }

#ifdef ROOTCORE
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
  TEST(OverlapRemovalInitTest, LegacyHelper)
  {
    ORUtils::ORToolBox tools;
    EXPECT_SUCCESS( ORUtils::recommendedTools(tools, "OverlapRemovalTool") );
    EXPECT_SUCCESS( tools.initialize() );
  }

  TEST(OverlapRemovalInitTest, HarmonizedHelper)
  {
    ORUtils::ORToolBox tools;
    EXPECT_SUCCESS( ORUtils::harmonizedTools(tools, "OverlapRemovalTool") );
    EXPECT_SUCCESS( tools.initialize() );
  }
#pragma GCC diagnostic pop
#endif

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
