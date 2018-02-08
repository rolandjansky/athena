#ifndef ASG_TOOLS__I_UNIT_TEST_TOOL2_H
#define ASG_TOOLS__I_UNIT_TEST_TOOL2_H

//        Copyright Iowa State University 2016.
//                  Author: Nils Krumnack
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (nils.erik.krumnack@cern.ch) for bug
// reports, feature suggestions, praise and complaints.


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
