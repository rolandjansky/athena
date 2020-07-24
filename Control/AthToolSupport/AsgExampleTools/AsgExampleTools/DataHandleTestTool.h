/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef ASG_TOOLS__UNIT_TEST_TOOL1_H
#define ASG_TOOLS__UNIT_TEST_TOOL1_H

#include <AsgTools/AsgTool.h>
#include <AsgExampleTools/IDataHandleTestTool.h>
#include <AsgDataHandles/ReadHandleKey.h>

// AthSimulation doesn't contain the muon-container, so we can't
// really build the tool, but it is simpler to build an empty tool
// than to exclude the tool completely from the AthSimulation build.
#ifndef SIMULATIONBASE
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
#ifndef SIMULATIONBASE
    SG::ReadHandleKey<xAOD::MuonContainer> m_readKey {this, "readKey", "Muons", "regular read key"};
#endif
    bool m_readFailure {false};
  };
}

#endif
