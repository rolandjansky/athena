/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//        Copyright Iowa State University 2016.
//                  Author: Nils Krumnack
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (nils.erik.krumnack@cern.ch) for bug
// reports, feature suggestions, praise and complaints.


//
// includes
//

#include <EventLoopTest/UnitTestConfig.h>

#include <RootCoreUtils/Assert.h>
#include <SampleHandler/DiskWriterLocal.h>

//
// method implementations
//

namespace EL
{
  std::unique_ptr<SH::DiskWriter> UnitTestConfig ::
  make_file_writer (const std::string& name) const
  {
    return std::unique_ptr<SH::DiskWriterLocal>
      (new SH::DiskWriterLocal ("data/" + name));
  }



  void UnitTestConfig ::
  setupJob (Job& /*job*/) const
  {}
}
