/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef EVENT_LOOP_TEST__I_UNIT_TEST_TOOL_H
#define EVENT_LOOP_TEST__I_UNIT_TEST_TOOL_H

#include <AsgTools/IAsgTool.h>
#include <AsgTools/MsgLevel.h>

namespace EL
{
  /// \brief the interface for \ref UnitTestTool

  class IUnitTestTool : virtual public asg::IAsgTool
  {
    // Declare the interface that this class provides
    ASG_TOOL_INTERFACE( CP::IUnitTestTool )

    /// \brief get the integer property
  public:
    virtual int getPropertyInt () const = 0;

    /// \brief get the subtool property
  public:
    virtual const IUnitTestTool *getSubtool () const = 0;
  };
}

#endif
