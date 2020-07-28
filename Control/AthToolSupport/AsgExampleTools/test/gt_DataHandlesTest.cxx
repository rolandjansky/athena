/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <AsgTools/AsgToolConfig.h>
#include <AsgMessaging/MessageCheck.h>
#include <AsgTesting/UnitTest.h>
#include <AsgExampleTools/IDataHandleTestTool.h>
#include <xAODRootAccess/TEvent.h>
#include <xAODRootAccess/TStore.h>
#include <TFile.h>
#include <cmath>
#include <gtest/gtest.h>
#include <sstream>

#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

//
// method implementations
//

using namespace asg::msgUserCode;

namespace asg
{
  struct DataHandlesTest : public ::testing::Test
  {
    static void SetUpTestCase ()
    {
      const char *test_file = getenv ("ASG_TEST_FILE_MC");
      ASSERT_NE (nullptr, test_file);
      file.reset (TFile::Open (test_file, "READ"));
      ASSERT_NE (nullptr, file);
    }

    static void TearDownTestCase ()
    {
      file.reset ();
    }

    virtual void SetUp () override
    {
      ASSERT_NE (nullptr, file);
      ASSERT_SUCCESS (event.readFrom (file.get()));
      ASSERT_TRUE (event.getEntry (0) >= 0);
    }

    /// \brief make a unique tool name to be used in unit tests
    std::string makeUniqueName ()
    {
      static unsigned index = 0;
      std::ostringstream str;
      str << "unique" << ++ index;
      return str.str();
    }

    static inline std::unique_ptr<TFile> file;
    xAOD::TEvent event;
    xAOD::TStore store;
    AsgToolConfig config {"asg::DataHandleTestTool/" + makeUniqueName()};
    std::shared_ptr<void> cleanup;
    ToolHandle<IDataHandleTestTool> tool;
  };



  // just test that a basic read handle access works
  TEST_F (DataHandlesTest, base_test)
  {
    ASSERT_SUCCESS (config.makeTool (tool, cleanup));
    tool->runTest ();
  }



  // just test that reading unknown objects fails as it should
  TEST_F (DataHandlesTest, read_failure)
  {
    config.setPropertyFromString ("readFailure", "1");
    config.setPropertyFromString ("readKey", "MuonsFailure");
    ASSERT_SUCCESS (config.makeTool (tool, cleanup));
    tool->runTest ();
  }
}

ATLAS_GOOGLE_TEST_MAIN
