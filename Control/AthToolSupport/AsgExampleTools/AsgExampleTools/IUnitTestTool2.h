/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef ASG_TOOLS__I_UNIT_TEST_TOOL2_H
#define ASG_TOOLS__I_UNIT_TEST_TOOL2_H

#include <AsgTools/IAsgTool.h>

namespace asg
{
  class IUnitTestTool1;

  /// \brief the interface for \ref UnitTestTool2

  class IUnitTestTool2 : virtual public IAsgTool
  {
    // Declare the interface that this class provides
    ASG_TOOL_INTERFACE( CP::IUnitTestTool2 )

    /// \brief whether the given ToolHandle is empty
  public:
    virtual bool
    toolHandleEmpty (const std::string& handleName) const = 0;

    /// \brief get the tool from the regular ToolHandle
  public:
    virtual const IUnitTestTool1 *
    getToolHandle (const std::string& handleName) const = 0;

    /// \brief call retrieve on the regular ToolHandle
  public:
    virtual StatusCode
    retrieveToolHandle (const std::string& handleName) const = 0;

    /// \brief whether the AnaToolHandle was user configured
  public:
    virtual bool
    wasUserConfigured (const std::string& handleName) const = 0;
  };
}

#endif
