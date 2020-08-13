/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <AsgExampleTools/DataHandleTestTool.h>

#include <AsgDataHandles/ReadHandle.h>
#include <AsgDataHandles/ReadDecorHandle.h>
#include <AsgDataHandles/WriteHandle.h>
#include <AsgTesting/UnitTest.h>
#include <gtest/gtest.h>
#include <map>

#ifndef SIMULATIONBASE
#include <xAODMuon/MuonAuxContainer.h>
#endif

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
    declareProperty ("readDecorFailure", m_readDecorFailure, "whether to expect a read decoration failure");
    declareProperty ("doWriteName", m_doWriteName, "if we should write, the name we expect to write to");
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
    ANA_CHECK (m_readDecorKey.initialize ());
    if (!m_writeKey.empty())
      ANA_CHECK (m_writeKey.initialize ());
#endif
    return StatusCode::SUCCESS;
  }



  void DataHandleTestTool ::
  runTest ()
  {
#ifndef SIMULATIONBASE
    const xAOD::MuonContainer *muonsStore {nullptr};
    ASSERT_SUCCESS (evtStore()->retrieve (muonsStore, "Muons"));
    ASSERT_NE (0u, muonsStore->size());
    const xAOD::Muon *testMuon = (*muonsStore)[0];

    auto readHandle = makeHandle (m_readKey);
    if (m_readFailure == true)
    {
      EXPECT_EQ (nullptr, readHandle.get());
      EXPECT_FALSE (readHandle.isValid());
    } else
    {
      EXPECT_EQ (muonsStore, readHandle.get());
      EXPECT_TRUE (readHandle.isValid());
    }

    SG::ReadDecorHandle<xAOD::MuonContainer,float> readDecorHandle (m_readDecorKey);
    if (m_readDecorFailure == true)
    {
      EXPECT_ANY_THROW (readDecorHandle (*testMuon));
    } else
    {
      SG::AuxElement::ConstAccessor<float> acc ("pt");
      EXPECT_EQ (acc (*testMuon), readDecorHandle (*testMuon));
    }

    if (!m_doWriteName.empty())
    {
      auto writeHandle = makeHandle (m_writeKey);
      auto newMuons = std::make_unique<xAOD::MuonContainer>();
      auto newAux = std::make_unique<xAOD::MuonAuxContainer>();
      xAOD::MuonContainer *recordMuons {newMuons.get()};
      xAOD::MuonAuxContainer *recordAux {newAux.get()};
      EXPECT_SUCCESS (writeHandle.record (std::move (newMuons), std::move (newAux)));
      const xAOD::MuonContainer *retrieveMuons {nullptr};
      EXPECT_SUCCESS (evtStore()->retrieve (retrieveMuons, m_doWriteName));
      EXPECT_EQ (recordMuons, retrieveMuons);
      xAOD::MuonAuxContainer *retrieveAux {nullptr};
      EXPECT_SUCCESS (evtStore()->retrieve (retrieveAux, m_doWriteName + "Aux."));
      EXPECT_EQ (recordAux, retrieveAux);
    }
#endif
  }
}
