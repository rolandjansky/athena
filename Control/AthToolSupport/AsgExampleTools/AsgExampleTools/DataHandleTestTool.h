/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef ASG_TOOLS__DATA_HANDLES_TEST_TOOL_H
#define ASG_TOOLS__DATA_HANDLES_TEST_TOOL_H

#include <AsgTools/AsgTool.h>
#include <AsgExampleTools/IDataHandleTestTool.h>

// currently in 21.2 the full data handle functionality as used by
// this test is not available in athena
#ifdef XAOD_STANDALONE
#include <AsgDataHandles/ReadHandleKey.h>
#include <AsgDataHandles/ReadHandleKeyArray.h>
#include <AsgDataHandles/ReadDecorHandleKey.h>
#include <AsgDataHandles/WriteHandleKey.h>
#include <AsgDataHandles/WriteDecorHandleKey.h>
#include <xAODMuon/MuonContainer.h>
#endif

namespace asg
{
  /// \brief a tool used to unit test AnaToolHandle
  ///
  /// This allows to unit test the various capabilities of
  /// stand-alone data handles in a controlled fashion.

  struct DataHandleTestTool : virtual public IDataHandleTestTool,
			 public AsgTool
  {
    ASG_TOOL_CLASS (DataHandleTestTool, IDataHandleTestTool)

    /// \brief standard constructor
  public:
    DataHandleTestTool (const std::string& val_name);

    /// \brief standard destructor
  public:
    ~DataHandleTestTool ();

  public:
    StatusCode initialize () override;

  public:
    void runTest () override;

  public:
#ifdef XAOD_STANDALONE
    SG::ReadHandleKey<xAOD::MuonContainer> m_readKey {this, "readKey", "Muons", "regular read key"};
    SG::ReadDecorHandleKey<xAOD::MuonContainer> m_readDecorKey {this, "readDecorKey", "Muons.pt", "read decor key"};
    SG::ReadHandleKeyArray<xAOD::MuonContainer> m_readKeyArray {this, "readKeyArray", {}, "array read key"};
    SG::WriteHandleKey<xAOD::MuonContainer> m_writeKey {this, "writeKey", "", "regular write key"};
    SG::WriteDecorHandleKey<xAOD::MuonContainer> m_writeDecorKey {this, "writeDecorKey", "", "write decor key"};
#endif
    bool m_readFailure {false};
    bool m_readArray {false};
    bool m_readDecorFailure {false};
    std::string m_doWriteName;
    std::string m_doWriteDecorName;
  };
}

#endif
