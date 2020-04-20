/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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

#include <RootCoreUtils/UnitTestDir.h>
#include <SampleHandler/MetaObject.h>
#include <SampleHandler/MetaVector.h>
#include <TFile.h>
#include <iostream>
#include <memory>

//
// main program
//

using namespace SH;

int main ()
{
  RCU::UnitTestDir dir ("SampleHandler", "MetaPersist");

  try
  {
    std::unique_ptr<MetaObject> meta1 (new MetaObject);
    const double valDouble = 42;
    meta1->setDouble ("double", valDouble);
    const std::string valString = "test";
    meta1->setString ("string", valString);
    const std::vector<Long64_t> valVector (1, 1);
    meta1->addReplace (new MetaVector<Long64_t>("vectorLong64", valVector));

    {
      TFile file ((dir.path() + "/meta.root").c_str(), "RECREATE");
      file.WriteObject (meta1.get(), "meta");
    }

    TFile file ((dir.path() + "/meta.root").c_str(), "READ");
    std::unique_ptr<MetaObject> meta2
      (dynamic_cast<MetaObject*>(file.Get ("meta")));

    RCU_ASSERT_SOFT (meta2.get() != 0);
    RCU_ASSERT_SOFT (meta2->castDouble ("double") == valDouble);
    RCU_ASSERT_SOFT (meta2->castString ("string") == valString);
    const MetaVector<Long64_t> *const metaVectorLong64
      = dynamic_cast<const MetaVector<Long64_t>*>(meta2->get ("vectorLong64"));
    RCU_ASSERT_SOFT (metaVectorLong64 != 0);
    RCU_ASSERT_SOFT (metaVectorLong64->value == valVector);

    return 0;
  } catch (std::exception& e)
  {
    std::cout << "caught exception: " << e.what() << std::endl;
    return 1;
  }
}
