/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//          
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (krumnack@iastate.edu) for bug
// reports, feature suggestions, praise and complaints.


//
// includes
//

#include <SampleHandler/Global.h>

#include <RootCoreUtils/Assert.h>
#include <SampleHandler/DiskListLocal.h>
#include <SampleHandler/SampleHandler.h>
#include <SampleHandler/Sample.h>
#include <SampleHandler/ScanDir.h>
#include <SampleHandler/ToolsDiscovery.h>
#include <TSystem.h>
#include <cstdlib>
#include <fstream>
#include <gtest/gtest.h>

//
// main program
//

using namespace SH;

struct ScanDirTest : testing::Test
{
  static const std::string root;

  static void SetUpTestCase ()
  {
    gSystem->MakeDirectory ((root + "").c_str());
    std::ofstream ((root + "/file.root_17").c_str());
    std::ofstream ((root + "/ignore").c_str());
    gSystem->MakeDirectory ((root + "/sample").c_str());
    std::ofstream ((root + "/sample/samplefile.root").c_str());
    gSystem->MakeDirectory ((root + "/deepsample").c_str());
    gSystem->MakeDirectory ((root + "/deepsample/subdir").c_str());
    std::ofstream ((root + "/deepsample/subdir/deepfile.root").c_str());
  }
};
const std::string ScanDirTest::root = "test1";

TEST_F (ScanDirTest, depth0)
{
  SH::SampleHandler sh;
  ScanDir()
    .sampleDepth(0)
    .scan (sh, root);
  EXPECT_TRUE (sh.get ("file.root_17"));
  EXPECT_TRUE (sh.get ("sample"));
  EXPECT_TRUE (sh.get ("deepsample"));
  EXPECT_FALSE (sh.get ("ignore"));
}

TEST_F (ScanDirTest, standard)
{
  SH::SampleHandler sh;
  ScanDir()
    .scan (sh, root);
  EXPECT_TRUE (sh.get ("test1"));
  EXPECT_TRUE (sh.get ("sample"));
  EXPECT_TRUE (sh.get ("subdir"));
  EXPECT_FALSE (sh.get ("deepsample"));
}

TEST_F (ScanDirTest, depth_minus1)
{
  SH::SampleHandler sh;
  ScanDir()
    .sampleDepth(-1)
    .scan (sh, root);
  EXPECT_TRUE (sh.get ("file.root_17"));
  EXPECT_TRUE (sh.get ("samplefile.root"));
  EXPECT_TRUE (sh.get ("deepfile.root"));
}

TEST_F (ScanDirTest, samplePostfix)
{
  SH::SampleHandler sh;
  ScanDir()
    .sampleDepth(-1)
    .samplePostfix(".root*")
    .scan (sh, root);
  EXPECT_TRUE (sh.get ("file"));
  EXPECT_TRUE (sh.get ("samplefile"));
  EXPECT_TRUE (sh.get ("deepfile"));
}

TEST_F (ScanDirTest, minDepth1)
{
  SH::SampleHandler sh;
  ScanDir()
    .sampleDepth(0)
    .minDepth (1)
    .scan (sh, root);
  EXPECT_FALSE (sh.get ("file.root_17"));
  EXPECT_TRUE (sh.get ("sample"));
  EXPECT_TRUE (sh.get ("deepsample"));
  EXPECT_FALSE (sh.get ("ignore"));
}

TEST_F (ScanDirTest, maxDepth1)
{
  SH::SampleHandler sh;
  ScanDir()
    .sampleDepth(0)
    .maxDepth (1)
    .scan (sh, root);
  EXPECT_TRUE (sh.get ("file.root_17"));
  EXPECT_TRUE (sh.get ("sample"));
  EXPECT_FALSE (sh.get ("deepsample"));
  EXPECT_FALSE (sh.get ("ignore"));
}

TEST_F (ScanDirTest, filePattern)
{
  SH::SampleHandler sh;
  ScanDir()
    .sampleDepth(0)
    .filePattern ("*.root")
    .scan (sh, root);
  EXPECT_FALSE (sh.get ("file.root_17"));
  EXPECT_TRUE (sh.get ("sample"));
  EXPECT_TRUE (sh.get ("deepsample"));
  EXPECT_FALSE (sh.get ("ignore"));
}

TEST_F (ScanDirTest, fileRegex)
{
  SH::SampleHandler sh;
  ScanDir()
    .sampleDepth(0)
    .fileRegex (".*\\.root")
    .scan (sh, root);
  EXPECT_FALSE (sh.get ("file.root_17"));
  EXPECT_TRUE (sh.get ("sample"));
  EXPECT_TRUE (sh.get ("deepsample"));
  EXPECT_FALSE (sh.get ("ignore"));
}

TEST_F (ScanDirTest, samplePattern)
{
  SH::SampleHandler sh;
  ScanDir()
    .sampleDepth(0)
    .samplePattern ("*sample")
    .scan (sh, root);
  EXPECT_FALSE (sh.get ("file.root_17"));
  EXPECT_TRUE (sh.get ("sample"));
  EXPECT_TRUE (sh.get ("deepsample"));
  EXPECT_FALSE (sh.get ("ignore"));
}

TEST_F (ScanDirTest, sampleRename)
{
  SH::SampleHandler sh;
  ScanDir()
    .sampleDepth(0)
    .sampleRename ("*sample", "sample1")
    .sampleRename ("deepsample", "sample2")
    .scan (sh, root);
  EXPECT_TRUE (sh.get ("sample1"));
  EXPECT_FALSE (sh.get ("sample2"));
  EXPECT_FALSE (sh.get ("sample"));
  EXPECT_FALSE (sh.get ("deepsample"));
}

TEST_F (ScanDirTest, sampleRenameOverride)
{
  SH::SampleHandler sh;
  ScanDir()
    .sampleDepth(0)
    .sampleRename ("deepsample", "sample2")
    .sampleRename ("*sample", "sample1")
    .scan (sh, root);
  EXPECT_TRUE (sh.get ("sample1"));
  EXPECT_TRUE (sh.get ("sample2"));
  EXPECT_FALSE (sh.get ("sample"));
  EXPECT_FALSE (sh.get ("deepsample"));
}

TEST_F (ScanDirTest, sampleRenameSingle)
{
  SH::SampleHandler sh;
  ScanDir()
    .sampleDepth(0)
    .sampleRename ("sample", "sample1")
    .scan (sh, root);
  EXPECT_TRUE (sh.get ("sample1"));
  EXPECT_FALSE (sh.get ("sample"));
  EXPECT_TRUE (sh.get ("deepsample"));
}

TEST_F (ScanDirTest, scanDirLegacy0)
{
  SH::SampleHandler sh;
  DiskListLocal list (root);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpragmas"
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
  scanDir (sh, list);
#pragma GCC diagnostic pop
  EXPECT_FALSE (sh.get ("file.root_17"));
  EXPECT_TRUE (sh.get ("sample"));
  EXPECT_FALSE (sh.get ("deepsample"));
  EXPECT_FALSE (sh.get ("ignore"));
}

TEST_F (ScanDirTest, scanDirLegacy1)
{
  SH::SampleHandler sh;
  DiskListLocal list (root);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpragmas"
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
  scanDir (sh, list, "*.root_*");
#pragma GCC diagnostic pop
  EXPECT_FALSE (sh.get ("file.root_17"));
  EXPECT_FALSE (sh.get ("sample"));
  EXPECT_FALSE (sh.get ("deepsample"));
  EXPECT_FALSE (sh.get ("ignore"));
}

TEST_F (ScanDirTest, scanDirLegacy2)
{
  SH::SampleHandler sh;
  DiskListLocal list (root);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpragmas"
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
  scanDir (sh, list, "*.root*", "adsf");
#pragma GCC diagnostic pop
  EXPECT_FALSE (sh.get ("file.root_17"));
  EXPECT_FALSE (sh.get ("sample"));
  EXPECT_FALSE (sh.get ("deepsample"));
  EXPECT_FALSE (sh.get ("ignore"));
}

TEST_F (ScanDirTest, scanDirLegacy3)
{
  SH::SampleHandler sh;
  DiskListLocal list (root);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpragmas"
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
  scanDir (sh, list, "*.root*", "*", "e");
#pragma GCC diagnostic pop
  EXPECT_FALSE (sh.get ("file.root_17"));
  EXPECT_TRUE (sh.get ("sampl"));
  EXPECT_FALSE (sh.get ("sample"));
  EXPECT_FALSE (sh.get ("deepsample"));
  EXPECT_FALSE (sh.get ("ignore"));
}

TEST_F (ScanDirTest, scanFilesLegacy)
{
  SH::SampleHandler sh;
  DiskListLocal list (root);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpragmas"
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
  scanFiles (sh, list, "*.root*");
#pragma GCC diagnostic pop
  EXPECT_TRUE (sh.get ("file"));
  EXPECT_TRUE (sh.get ("samplefile"));
  EXPECT_TRUE (sh.get ("deepfile"));
  EXPECT_FALSE (sh.get ("ignore"));
}

TEST_F (ScanDirTest, scanSingleDirLegacy)
{
  SH::SampleHandler sh;
  DiskListLocal list (root);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpragmas"
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
  scanSingleDir (sh, "xx", list);
#pragma GCC diagnostic pop
  EXPECT_TRUE (sh.get ("xx"));
  EXPECT_TRUE (sh.get ("xx")->numFiles() == 3);
  EXPECT_FALSE (sh.get ("file"));
  EXPECT_FALSE (sh.get ("samplefile"));
  EXPECT_FALSE (sh.get ("deepfile"));
  EXPECT_FALSE (sh.get ("ignore"));
}

TEST_F (ScanDirTest, absSampleDepth_minus1)
{
  SH::SampleHandler sh;
  ScanDir()
    .absSampleDepth(-1)
    .scan (sh, root);
  EXPECT_TRUE (sh.get ("test1"));
  EXPECT_FALSE (sh.get ("file.root_17"));
  EXPECT_FALSE (sh.get ("sample"));
  EXPECT_FALSE (sh.get ("deepsample"));
  EXPECT_FALSE (sh.get ("ignore"));
}

TEST_F (ScanDirTest, sampleDepth_minus2)
{
  SH::SampleHandler sh;
  ScanDir()
    .sampleDepth(-2)
    .scan (sh, root);
  EXPECT_TRUE (sh.get ("test1"));
  EXPECT_FALSE (sh.get ("file.root_17"));
  EXPECT_TRUE (sh.get ("sample"));
  EXPECT_TRUE (sh.get ("subdir"));
  EXPECT_FALSE (sh.get ("deepsample"));
  EXPECT_FALSE (sh.get ("ignore"));
}

TEST_F (ScanDirTest, sampleName)
{
  SH::SampleHandler sh;
  ScanDir()
    .sampleName ("mysample")
    .scan (sh, root);
  EXPECT_TRUE (sh.get ("mysample"));
  EXPECT_FALSE (sh.get ("test1"));
  EXPECT_FALSE (sh.get ("file.root_17"));
  EXPECT_FALSE (sh.get ("sample"));
  EXPECT_FALSE (sh.get ("subdir"));
  EXPECT_FALSE (sh.get ("deepsample"));
  EXPECT_FALSE (sh.get ("ignore"));
}

TEST_F (ScanDirTest, extraNameComponent_minus2)
{
  SH::SampleHandler sh;
  ScanDir()
    .extraNameComponent (-2)
    .sampleDepth (-1)
    .samplePostfix (".root*")
    .scan (sh, root);
  EXPECT_TRUE (sh.get ("deepfile_subdir"));
  EXPECT_TRUE (sh.get ("file_test1"));
  EXPECT_TRUE (sh.get ("samplefile_sample"));
  EXPECT_FALSE (sh.get ("mysample"));
  EXPECT_FALSE (sh.get ("test1"));
  EXPECT_FALSE (sh.get ("file.root_17"));
  EXPECT_FALSE (sh.get ("sample"));
  EXPECT_FALSE (sh.get ("subdir"));
  EXPECT_FALSE (sh.get ("deepsample"));
  EXPECT_FALSE (sh.get ("ignore"));
}

TEST_F (ScanDirTest, extraNameComponent_1)
{
  SH::SampleHandler sh;
  ScanDir()
    .extraNameComponent (1)
    .sampleDepth (2)
    .samplePostfix (".root*")
    .scan (sh, root);
  EXPECT_TRUE (sh.get ("deepfile_subdir"));
  EXPECT_FALSE (sh.get ("file_test1"));
  EXPECT_FALSE (sh.get ("samplefile_sample"));
  EXPECT_FALSE (sh.get ("mysample"));
  EXPECT_FALSE (sh.get ("test1"));
  EXPECT_FALSE (sh.get ("file.root_17"));
  EXPECT_FALSE (sh.get ("sample"));
  EXPECT_FALSE (sh.get ("subdir"));
  EXPECT_FALSE (sh.get ("deepsample"));
  EXPECT_FALSE (sh.get ("ignore"));
}

int main (int argc, char **argv)
{
  ::testing::InitGoogleTest (&argc, argv);
  return RUN_ALL_TESTS();
}
