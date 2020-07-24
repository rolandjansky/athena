/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef ASG_TOOLS__UNIT_TEST_TOOL1_H
#define ASG_TOOLS__UNIT_TEST_TOOL1_H

#include <AsgTools/AsgTool.h>
#include <AsgExampleTools/IDataHandleTestTool.h>
#include <AsgDataHandles/ReadHandleKey.h>
#include <xAODMuon/MuonContainer.h>

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
    SG::ReadHandleKey<xAOD::MuonContainer> m_readKey {this, "readKey", "Muons", "regular read key"};
    bool m_readFailure {false};
  };
}

#endif
