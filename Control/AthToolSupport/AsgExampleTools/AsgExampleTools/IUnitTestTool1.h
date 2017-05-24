#ifndef ASG_TOOLS__I_UNIT_TEST_TOOL1_H
#define ASG_TOOLS__I_UNIT_TEST_TOOL1_H

//        Copyright Iowa State University 2016.
//                  Author: Nils Krumnack
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (nils.erik.krumnack@cern.ch) for bug
// reports, feature suggestions, praise and complaints.


#include <AsgTools/IAsgTool.h>
#include <AsgTools/MsgLevel.h>

namespace asg
{
  /// \brief the interface for \ref UnitTestTool1

  class IUnitTestTool1 : virtual public IAsgTool
  {
    // Declare the interface that this class provides
    ASG_TOOL_INTERFACE( CP::IUnitTestTool1 )

    /// \brief get the integer property
  public:
    virtual std::string getPropertyString () const = 0;

    /// \brief get the integer property
  public:
    virtual int getPropertyInt () const = 0;

    /// \brief get the integer property
  public:
    virtual void setPropertyInt (int val_property) = 0;

    /// \brief get whether we have been initialized
  public:
    virtual bool isInitialized () const = 0;

    /// \brief the message level during initialize()
  public:
    virtual MSG::Level getOrigMsgLevel () const = 0;
  };
}

#endif
