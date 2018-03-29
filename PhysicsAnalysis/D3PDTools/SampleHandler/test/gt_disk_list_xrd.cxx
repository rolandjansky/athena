/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//          Copyright Nils Krumnack 2011.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (krumnack@iastate.edu) for bug
// reports, feature suggestions, praise and complaints.


//
// includes
//

#include <SampleHandler/Global.h>

#include <cstdlib>
#include <memory>
#include <TFile.h>
#include <RootCoreUtils/Assert.h>
#include <SampleHandler/DiskListXRD.h>
#include <gtest/gtest.h>

//
// main program
//

using namespace SH;

TEST (DiskListTest, DISABLED_all)
{
  DiskListXRD list1 ("eosatlas", "/eos/atlas/user/k/krumnack");
  bool valid = true;
  while ((valid = list1.next()) && list1.fileName() != "EventLoop-UnitTest") {};
  ASSERT_TRUE (valid);
  std::auto_ptr<DiskList> list2 (list1.openDir());
  ASSERT_TRUE (list2.get() != 0);
  if (!list2->next())
    ADD_FAILURE() << "empty dir";
  std::auto_ptr<TFile> file
    (TFile::Open (list2->path().c_str(), "READ"));
  ASSERT_NE (nullptr, file.get());
  while (list1.next()) {};
}

int main (int argc, char **argv)
{
  ::testing::InitGoogleTest (&argc, argv);
  return RUN_ALL_TESTS();
}
