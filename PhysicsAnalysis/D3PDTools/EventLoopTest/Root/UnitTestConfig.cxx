/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



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
