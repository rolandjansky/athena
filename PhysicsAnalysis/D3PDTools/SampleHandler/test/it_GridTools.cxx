/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


//
// includes
//

#include <AsgTesting/UnitTest.h>
#include <RootCoreUtils/Assert.h>
#include <RootCoreUtils/ShellExec.h>
#include <SampleHandler/DiskListLocal.h>
#include <SampleHandler/GridTools.h>
#include <SampleHandler/MetaObject.h>
#include <SampleHandler/SampleHandler.h>
#include <SampleHandler/SampleMeta.h>
#include <SampleHandler/ScanDir.h>
#include <SampleHandler/ToolsDiscovery.h>
#include <TSystem.h>
#include <cstdlib>
#include <fstream>
#include <gtest/gtest.h>
#include <sys/types.h>
#include <unistd.h>

#pragma GCC diagnostic ignored "-Wpragmas"
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

namespace sh = RCU::Shell;

//
// main program
//

using namespace SH;
using namespace asg::msgUserCode;

TEST (GridToolsTest, faxListFiles)
{
  const auto files = faxListFilesGlob
    ("user.ivukotic:user.ilijav.HCtest.1", "*.root*");
  ASSERT_EQ (5u, files.size());
}

TEST (GridToolsTest, rucioDirectAccessGlob)
{
  const auto files = rucioDirectAccessGlob
    ("user.ivukotic:user.ilijav.HCtest.1", "*.root*", "");
  ASSERT_EQ (5u, files.size());
}

TEST (GridToolsTest, rucioListDids)
{
  const auto entries = rucioListDids
    ("user.ivukotic:user.ilijav.HCtest.1");
  std::map<std::string,unsigned> scopes;
  std::map<std::string,unsigned> types;
  for (auto& entry : entries)
  {
    if (scopes.find (entry.scope) == scopes.end())
      scopes[entry.scope] = 0;
    ++ scopes[entry.scope];
    if (types.find (entry.type) == types.end())
      types[entry.type] = 0;
    ++ types[entry.type];
  }
  EXPECT_EQ (1u, scopes.size());
  EXPECT_TRUE (scopes.find ("user.ivukotic") != scopes.end());
  EXPECT_GE (1u, types.size());
  ASSERT_TRUE (types.find ("DATASET") != types.end());
  EXPECT_EQ (1u, types.find ("DATASET")->second);
}

TEST (GridToolsTest, rucioListFileReplicas)
{
  const auto entries = rucioListFileReplicas
    ("user.ivukotic:user.ilijav.HCtest.1");
  std::map<std::string,unsigned> scopes;
  std::map<std::string,unsigned> types;
  for (auto& entry : entries)
  {
    if (scopes.find (entry.scope) == scopes.end())
      scopes[entry.scope] = 0;
    ++ scopes[entry.scope];
  }
  EXPECT_EQ (1u, scopes.size());
  EXPECT_TRUE (scopes.find ("user.ivukotic") != scopes.end());
}

TEST (GridToolsTest, rucioGetMetadata)
{
  std::set<std::string> datasets
    ({"user.ivukotic:user.ilijav.HCtest.1", "user.ivukotic:Details.log.2"});
  const auto entries = rucioGetMetadata (datasets);
  EXPECT_EQ (datasets.size(), entries.size());
  for (auto& dataset : datasets)
  {
    SCOPED_TRACE (dataset);
    EXPECT_TRUE (entries.find (dataset) != entries.end());
  }
}

TEST (GridToolsTest, scanRucio)
{
  SH::SampleHandler sh;
  scanRucio (sh, "user.ivukotic:user.*.HCtest.1");
  ASSERT_EQ (1u, sh.size());
  ASSERT_TRUE (sh.get ("user.ivukotic:user.ilijav.HCtest.1"));
}

TEST (GridToolsTest, scanRucio_fail)
{
  SH::SampleHandler sh;
  EXPECT_THROW_REGEX (scanRucio (sh, "user.ivukotic:user.NONEXISTENT.HCtest.1*"), "failed to find any datasets matching pattern: user.ivukotic:user.NONEXISTENT.HCtest.1*");
}

TEST (GridToolsTest, scanRucio_simple)
{
  SH::SampleHandler sh;
  scanRucio (sh, "user.ivukotic:user.NONEXISTENT.HCtest.1");
  ASSERT_EQ (1u, sh.size());
  ASSERT_TRUE (sh.get ("user.ivukotic:user.NONEXISTENT.HCtest.1"));
}

TEST (GridToolsTest, scanRucio_simple_fail)
{
  SH::SampleHandler sh;
  EXPECT_THROW_REGEX (scanRucio (sh, "user.ivukotic:user.NONEXISTENT.HCtest.1", true), ".*user.NONEXISTENT.HCtest.1.*");
}

TEST (GridToolsTest, download)
{
  // can't use my current directory, it may easily overflow
  std::ostringstream location;
  {
    const char *TMPDIR = getenv ("TMPDIR");
    if (TMPDIR)
      location << TMPDIR << "/";
    else
      location << "/tmp/";
  }
  location << "GridToolsTest." << getpid();
  sh::exec ("rm -rf " + sh::quote (location.str()));
  sh::exec ("mkdir -p " + sh::quote (location.str()));

  RucioDownloadResult result = rucioDownload (location.str(), "user.ivukotic:Details.log.2");
  ASSERT_EQ ("user.ivukotic:Details.log.2", result.did);
  ASSERT_EQ (1u, result.totalFiles);
  ASSERT_EQ (1u, result.downloadedFiles);
  ASSERT_EQ (0u, result.alreadyLocal);
  ASSERT_EQ (0u, result.notDownloaded);
}

TEST (GridToolsTest, downloadCache)
{
  // can't use my current directory, it may easily overflow
  std::ostringstream location;
  {
    const char *TMPDIR = getenv ("TMPDIR");
    if (TMPDIR)
      location << TMPDIR << "/";
    else
      location << "/tmp/";
  }
  location << "GridToolsTest." << getpid();
  sh::exec ("rm -rf " + sh::quote (location.str()));
  sh::exec ("mkdir -p " + sh::quote (location.str()));

  std::vector<std::string> result =
    rucioCacheDatasetGlob (location.str(), "user.krumnack:user.krumnack.EventLoopTest.2019-03-25.dataset0", "*.root*");
  ASSERT_EQ (1u, result.size());
  ASSERT_EQ (location.str() + "/user.krumnack.EventLoopTest.2019-03-25.dataset0/EventLoopTest.2019-03-25.test_ntuple0.root", result[0]);
  ASSERT_FALSE (gSystem->AccessPathName (result[0].c_str()));

  std::vector<std::string> result2 =
    rucioCacheDatasetGlob (location.str(), "user.krumnack:user.krumnack.EventLoopTest.2019-03-25.dataset1", "*.root*");
  ASSERT_EQ (2u, result2.size());
  std::sort (result2.begin(), result2.end());
  ASSERT_EQ (location.str() + "/user.krumnack.EventLoopTest.2019-03-25.dataset1/EventLoopTest.2019-03-25.test_ntuple1.root", result2[0]);
  ASSERT_EQ (location.str() + "/user.krumnack.EventLoopTest.2019-03-25.dataset1/EventLoopTest.2019-03-25.test_ntuple2.root", result2[1]);
  ASSERT_FALSE (gSystem->AccessPathName (result2[0].c_str()));
}

ATLAS_GOOGLE_TEST_MAIN
