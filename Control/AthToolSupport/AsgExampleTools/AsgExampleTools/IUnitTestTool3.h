/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef ASG_TOOLS__I_UNIT_TEST_TOOL3_H
#define ASG_TOOLS__I_UNIT_TEST_TOOL3_H

#include <AsgTools/IAsgTool.h>

namespace asg
{
  class IUnitTestTool1;

  /// \brief the interface for \ref UnitTestTool3

  class IUnitTestTool3 : virtual public IAsgTool
  {
    // Declare the interface that this class provides
    ASG_TOOL_INTERFACE( CP::IUnitTestTool3 )

    /// \brief get the subtool we configured
  public:
    virtual const IUnitTestTool1 *getSubtool () const = 0;

    /// \brief get whether the subtool configured for our subtool is empty
  public:
    virtual bool subsubtoolEmpty () const = 0;

    /// \brief get the subtool configured for our subtool
  public:
    virtual const IUnitTestTool1 *getSubsubtool () const = 0;
  };
}

#endif
