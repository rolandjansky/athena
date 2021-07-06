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

#include <SampleHandler/SampleMeta.h>

#include <RootCoreUtils/Assert.h>
#include <RootCoreUtils/ThrowMsg.h>
#include <SampleHandler/SamplePtr.h>

//
// method implementations
//

ClassImp (SH::SampleMeta)

namespace SH
{
  void SampleMeta :: 
  testInvariant () const
  {
  }



  SampleMeta ::
  SampleMeta ()
    : Sample ("unnamed")
  {
    RCU_NEW_INVARIANT (this);
  }



  SampleMeta :: 
  SampleMeta (const std::string& name)
    : Sample (name)
  {
    RCU_NEW_INVARIANT (this);
  }



  std::size_t SampleMeta ::
  getNumFiles () const
  {
    RCU_READ_INVARIANT (this);
    // rationale: this is just so that print() will work
    return 0;
  }



  std::string SampleMeta ::
  getFileName (std::size_t /*index*/) const
  {
    RCU_READ_INVARIANT (this);
    RCU_THROW_MSG ("fileName() should not be called on SampleMeta");
    return ""; //compiler dummy
  }



  SamplePtr SampleMeta ::
  doMakeLocal () const
  {
    RCU_READ_INVARIANT (this);
    RCU_THROW_MSG ("makeLocal() should not be called on SampleMeta");
    return 0; //compiler dummy
  }



  std::vector<std::string> SampleMeta ::
  doMakeFileList () const
  {
    RCU_READ_INVARIANT (this);

    std::vector<std::string> result;
    return result;
  }
}
