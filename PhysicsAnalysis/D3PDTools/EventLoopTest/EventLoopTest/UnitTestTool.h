/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef EVENT_LOOP_TEST__UNIT_TEST_TOOL1_H
#define EVENT_LOOP_TEST__UNIT_TEST_TOOL1_H

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
