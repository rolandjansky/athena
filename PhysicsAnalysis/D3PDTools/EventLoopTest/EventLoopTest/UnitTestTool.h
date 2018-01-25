#ifndef EVENT_LOOP_TEST__UNIT_TEST_TOOL1_H
#define EVENT_LOOP_TEST__UNIT_TEST_TOOL1_H

//        Copyright Iowa State University 2016.
//                  Author: Nils Krumnack
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (nils.erik.krumnack@cern.ch) for bug
// reports, feature suggestions, praise and complaints.


#include <AsgTools/AsgTool.h>
#include <EventLoopTest/IUnitTestTool.h>

namespace EL
{
  /// \brief a tool used to unit test AnaToolHandle
  ///
  /// This allows to unit test the various capabilities of
  /// AnaToolHandle in a controlled fashion.

  struct UnitTestTool : virtual public IUnitTestTool,
                        public asg::AsgTool
  {
    ASG_TOOL_CLASS (UnitTestTool, IUnitTestTool)

    /// \brief standard constructor
  public:
    UnitTestTool (const std::string& val_name);

    /// \brief standard destructor
  public:
    ~UnitTestTool ();

  public:
    ::StatusCode initialize () override;

  public:
    virtual int getPropertyInt () const override;

  public:
    virtual const IUnitTestTool *getSubtool () const override;


    /// \brief the toolhandle for subtools
  public:
    ToolHandle<IUnitTestTool> m_subtool;

    /// \brief the integer property
  public:
    int m_propertyInt = 0;
  };
}

#endif
