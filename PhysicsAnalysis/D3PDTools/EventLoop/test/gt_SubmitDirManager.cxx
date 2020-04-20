/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <AsgTesting/UnitTest.h>
#include <EventLoop/ManagerData.h>
#include <EventLoop/SubmitDirManager.h>
#include <TH1F.h>
#include <TKey.h>
#include <TSystem.h>
#include <TTree.h>
#include <boost/filesystem.hpp>

//
// unit test
//

namespace EL
{
  namespace Detail
  {
    TEST (SubmitDirManagerTest, relativePath)
    {
      SubmitDirManager manager;
      ManagerData data;
      data.step = ManagerStep::updateSubmitDir;

      data.submitDir = "alpha";
      ASSERT_SUCCESS (manager.doManagerStep (data));
      EXPECT_EQ (data.submitDir[0], '/');

      data.submitDir = "/alpha";
      ASSERT_SUCCESS (manager.doManagerStep (data));
      EXPECT_EQ (data.submitDir, "/alpha");

      data.submitDir = "/../alpha/beta";
      ASSERT_SUCCESS (manager.doManagerStep (data));
      EXPECT_EQ (data.submitDir, "/alpha/beta");

      data.submitDir = "/alpha/../beta";
      ASSERT_SUCCESS (manager.doManagerStep (data));
      EXPECT_EQ (data.submitDir, "/beta");

      data.submitDir = "/alpha/./beta";
      ASSERT_SUCCESS (manager.doManagerStep (data));
      EXPECT_EQ (data.submitDir, "/alpha/beta");

      data.submitDir = "/alpha//beta";
      ASSERT_SUCCESS (manager.doManagerStep (data));
      EXPECT_EQ (data.submitDir, "/alpha/beta");

      data.submitDir = "/../alpha///./../beta";
      ASSERT_SUCCESS (manager.doManagerStep (data));
      EXPECT_EQ (data.submitDir, "/beta");

      data.submitDir = "/alpha/";
      ASSERT_SUCCESS (manager.doManagerStep (data));
      EXPECT_EQ (data.submitDir, "/alpha");

      data.submitDir = "/alpha/.//";
      ASSERT_SUCCESS (manager.doManagerStep (data));
      EXPECT_EQ (data.submitDir, "/alpha");

      data.submitDir = "/alpha/..";
      ASSERT_FAILURE (manager.doManagerStep (data));

      data.submitDir = "/alpha/.././";
      ASSERT_FAILURE (manager.doManagerStep (data));

      data.submitDir = "/pnfs/alpha";
      ASSERT_FAILURE (manager.doManagerStep (data));
    }



    TEST (SubmitDirManagerTest, mkdir)
    {
      // make sure we have a clean directory to work with
      const std::string baseDir {"SubmitDirManagerTest"};
      ASSERT_SUCCESS (gSystem->Exec (("rm -rf " + baseDir).c_str()));
      ASSERT_SUCCESS (gSystem->mkdir (baseDir.c_str()));

      SubmitDirManager manager;
      ManagerData data;
      data.step = ManagerStep::createSubmitDir;

      data.submitDir = baseDir + "/simple";
      data.submitDirMode = SubmitDirMode::NO_CLOBBER;
      ASSERT_SUCCESS (manager.doManagerStep (data));
      ASSERT_TRUE (boost::filesystem::is_directory (data.submitDir));

      // check that we don't overwrite directories in regular mode
      ASSERT_FAILURE (manager.doManagerStep (data));

      // check that we do remove and recreate existing directories in
      // OVERWRITE mode.  to test that we place a file inside that
      // directory and check that it disappears.
      std::string dummyFile = data.submitDir + "/dummy";
      ASSERT_SUCCESS (gSystem->Exec (("touch " + dummyFile).c_str()));
      ASSERT_TRUE (boost::filesystem::is_regular_file (dummyFile));
      data.submitDirMode = SubmitDirMode::OVERWRITE;
      ASSERT_SUCCESS (manager.doManagerStep (data));
      ASSERT_TRUE (boost::filesystem::is_directory (data.submitDir));
      ASSERT_FALSE (boost::filesystem::is_regular_file (dummyFile));



      // check that the unique submit directory creation works
      std::string uniqueBase = baseDir + "/unique";
      data.submitDir = uniqueBase;
      data.submitDirMode = SubmitDirMode::UNIQUE;
      ASSERT_SUCCESS (manager.doManagerStep (data));
      ASSERT_NE (data.submitDir, uniqueBase);
      ASSERT_TRUE (data.submitDir.find (uniqueBase) == 0);
      ASSERT_TRUE (boost::filesystem::is_directory (data.submitDir));
      ASSERT_FALSE (boost::filesystem::exists (uniqueBase));


      // check that we create a symlink when working with unique link
      // mode.  checking that it is correct by creating a file via the
      // original directory and then reading it via the symlink
      data.submitDir = uniqueBase;
      data.submitDirMode = SubmitDirMode::UNIQUE_LINK;
      ASSERT_SUCCESS (manager.doManagerStep (data));
      ASSERT_TRUE (boost::filesystem::is_symlink (uniqueBase));
      ASSERT_SUCCESS (gSystem->Exec (("touch " + data.submitDir + "/dummy1").c_str()));
      ASSERT_TRUE (boost::filesystem::is_regular_file (uniqueBase + "/dummy1"));


      // check that we also re-create a symlink when there is already
      // a sym-link in place.  checking that it is correct by creating
      // a file via the original directory with a different name from
      // above and then reading it via the symlink
      data.submitDir = uniqueBase;
      data.submitDirMode = SubmitDirMode::UNIQUE_LINK;
      ASSERT_SUCCESS (manager.doManagerStep (data));
      ASSERT_TRUE (boost::filesystem::is_symlink (uniqueBase));
      ASSERT_SUCCESS (gSystem->Exec (("touch " + data.submitDir + "/dummy2").c_str()));
      ASSERT_TRUE (boost::filesystem::is_regular_file (uniqueBase + "/dummy2"));
    }
  }
}

ATLAS_GOOGLE_TEST_MAIN
