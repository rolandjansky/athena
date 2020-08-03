/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <AsgExampleTools/DataHandleTestTool.h>

#include <AsgDataHandles/ReadHandle.h>
#include <AsgTesting/UnitTest.h>
#include <gtest/gtest.h>
#include <map>

//
// method implementations
//

namespace asg
{
  DataHandleTestTool ::
  DataHandleTestTool (const std::string& val_name)
    : AsgTool (val_name)
  {
    declareProperty ("readFailure", m_readFailure, "whether to expect a read failure");
  }



  DataHandleTestTool ::
  ~DataHandleTestTool ()
  {
  }



  StatusCode DataHandleTestTool ::
  initialize ()
  {
#ifndef SIMULATIONBASE
    ANA_CHECK (m_readKey.initialize ());
#endif
    return StatusCode::SUCCESS;
  }



  void DataHandleTestTool ::
  runTest ()
  {
#ifndef SIMULATIONBASE
    const xAOD::MuonContainer *muonsStore {nullptr};
    ASSERT_SUCCESS (evtStore()->retrieve (muonsStore, "Muons"));

    auto readHandle = makeHandle (m_readKey);
    if (m_readFailure == false)
      EXPECT_EQ (muonsStore, readHandle.get());
    else
      EXPECT_EQ (nullptr, readHandle.get());
#endif
  }
}
