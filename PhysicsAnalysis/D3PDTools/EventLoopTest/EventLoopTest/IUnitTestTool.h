#ifndef EVENT_LOOP_TEST__I_UNIT_TEST_TOOL_H
#define EVENT_LOOP_TEST__I_UNIT_TEST_TOOL_H

//        Copyright Iowa State University 2016.
//                  Author: Nils Krumnack
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (nils.erik.krumnack@cern.ch) for bug
// reports, feature suggestions, praise and complaints.


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
