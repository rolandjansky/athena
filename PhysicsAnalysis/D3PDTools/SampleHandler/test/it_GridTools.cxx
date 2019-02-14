/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//          Copyright Nils Krumnack 2016.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (krumnack@iastate.edu) for bug
// reports, feature suggestions, praise and complaints.


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

TEST (GridToolsTest, MANUAL_faxListFiles)
{
  const auto files = faxListFilesGlob
    ("user.ivukotic:user.ilijav.HCtest.1", "*.root*");
  ASSERT_EQ (5u, files.size());
}

TEST (GridToolsTest, MANUAL_rucioDirectAccessGlob)
{
  const auto files = rucioDirectAccessGlob
    ("user.ivukotic:user.ilijav.HCtest.1", "*.root*");
  ASSERT_EQ (5u, files.size());
}

TEST (GridToolsTest, MANUAL_rucioListDids)
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

TEST (GridToolsTest, MANUAL_rucioListFileReplicas)
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

TEST (GridToolsTest, MANUAL_rucioGetMetadata)
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

TEST (GridToolsTest, MANUAL_scanRucio)
{
  SH::SampleHandler sh;
  scanRucio (sh, "user.ivukotic:user.*.HCtest.1");
  ASSERT_EQ (1u, sh.size());
  ASSERT_TRUE (sh.get ("user.ivukotic:user.ilijav.HCtest.1"));
}

TEST (GridToolsTest, MANUAL_scanRucio_fail)
{
  SH::SampleHandler sh;
  EXPECT_THROW_REGEX (scanRucio (sh, "user.ivukotic:user.NONEXISTENT.HCtest.1*"), "failed to find any datasets matching pattern: user.ivukotic:user.NONEXISTENT.HCtest.1*");
}

TEST (GridToolsTest, MANUAL_scanRucio_simple)
{
  SH::SampleHandler sh;
  scanRucio (sh, "user.ivukotic:user.NONEXISTENT.HCtest.1");
  ASSERT_EQ (1u, sh.size());
  ASSERT_TRUE (sh.get ("user.ivukotic:user.NONEXISTENT.HCtest.1"));
}

TEST (GridToolsTest, MANUAL_scanRucio_simple_fail)
{
  SH::SampleHandler sh;
  EXPECT_THROW_REGEX (scanRucio (sh, "user.ivukotic:user.NONEXISTENT.HCtest.1", true), ".*user.NONEXISTENT.HCtest.1.*");
}

TEST (GridToolsTest, MANUAL_download)
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

ATLAS_GOOGLE_TEST_MAIN
