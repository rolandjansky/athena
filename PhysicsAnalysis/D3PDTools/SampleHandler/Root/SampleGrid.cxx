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

#include <SampleHandler/SampleGrid.h>

#include <RootCoreUtils/Assert.h>
#include <RootCoreUtils/ThrowMsg.h>
#include <SampleHandler/GridTools.h>
#include <SampleHandler/MetaFields.h>
#include <SampleHandler/MetaObject.h>
#include <SampleHandler/SampleLocal.h>
#include <SampleHandler/SamplePtr.h>
#include <memory>

//
// method implementations
//

ClassImp (SH::SampleGrid)

namespace SH
{
  void SampleGrid ::
  testInvariant () const
  {
    RCU_INVARIANT (this != 0);
  }



  SampleGrid ::
  SampleGrid ()
    : Sample ("unnamed")
  {
    RCU_NEW_INVARIANT (this);
  }



  SampleGrid ::
  SampleGrid (const std::string& name)
    : Sample (name)
  {
    RCU_NEW_INVARIANT (this);
  }



  std::size_t SampleGrid ::
  getNumFiles () const
  {
    RCU_READ_INVARIANT (this);
    RCU_THROW_MSG ("Sample::numFiles not supported for SampleGrid");
    return 0; // compiler dummy
  }



  std::string SampleGrid ::
  getFileName (const std::size_t /*index*/) const
  {
    RCU_READ_INVARIANT (this);
    RCU_THROW_MSG ("Sample::fileName not supported for SampleGrid");
    return ""; // compiler dummy
  }



  SamplePtr SampleGrid ::
  doMakeLocal () const
  {
    RCU_READ_INVARIANT (this);

    std::auto_ptr<SampleLocal> result (new SampleLocal (name()));
    for (auto& file : makeFileList ())
      result->add (file.c_str());
    return SamplePtr (result.release());
  }



  std::vector<std::string> SampleGrid ::
  doMakeFileList () const
  {
    RCU_READ_INVARIANT (this);

    const std::string sample_name
      = meta()->castString (MetaFields::gridName, name());
    const std::string file_filter
      = meta()->castString (MetaFields::gridFilter, MetaFields::gridFilter_default);
    return rucioDirectAccessGlob (sample_name, file_filter);
  }
}
