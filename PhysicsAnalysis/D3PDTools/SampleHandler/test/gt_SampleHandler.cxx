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
#include <SampleHandler/SampleMeta.h>
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

TEST (SampleHandlerTest, add_single)
{
  SampleHandler sh;
  ASSERT_TRUE (sh.get ("sample") == nullptr);
  sh.add (new SampleMeta ("sample"));
  ASSERT_TRUE (sh.get ("sample") != nullptr);
}

TEST (SampleHandlerTest, add_duplicate)
{
  SampleHandler sh;
  sh.add (new SampleMeta ("sample"));
  ASSERT_ANY_THROW (sh.add (new SampleMeta ("sample")));
}

TEST (SampleHandlerTest, addWithPrefix_single)
{
  SampleHandler target, source;
  source.add (new SampleMeta ("sample"));
  target.addWithPrefix (source, "prefix_");
  ASSERT_TRUE (target.get ("prefix_sample") != nullptr);
}

TEST (SampleHandlerTest, addWithPrefix_duplicate)
{
  SampleHandler target, source;
  source.add (new SampleMeta ("sample"));
  target.add (new SampleMeta ("prefix_sample"));
  EXPECT_ANY_THROW (target.addWithPrefix (source, "prefix_"));
}

int main (int argc, char **argv)
{
  ::testing::InitGoogleTest (&argc, argv);
  return RUN_ALL_TESTS();
}
