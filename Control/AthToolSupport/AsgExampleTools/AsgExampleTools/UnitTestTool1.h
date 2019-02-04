/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef ASG_TOOLS__UNIT_TEST_TOOL1_H
#define ASG_TOOLS__UNIT_TEST_TOOL1_H

#include <AsgTools/AsgTool.h>
#include <AsgExampleTools/IUnitTestTool1.h>

namespace asg
{
  /// \brief a tool used to unit test AnaToolHandle
  ///
  /// This allows to unit test the various capabilities of
  /// AnaToolHandle in a controlled fashion.

  struct UnitTestTool1 : virtual public IUnitTestTool1,
			 public AsgTool
  {
    ASG_TOOL_CLASS (UnitTestTool1, IUnitTestTool1)

    /// \brief standard constructor
  public:
    UnitTestTool1 (const std::string& val_name);

    /// \brief standard destructor
  public:
    ~UnitTestTool1 ();

  public:
    StatusCode initialize () override;

  public:
    virtual std::string getPropertyString () const override;

  public:
    virtual int getPropertyInt () const override;

  public:
    virtual void setPropertyInt (int val_property) override;

  public:
    virtual bool isInitialized () const override;

    /// \brief whether initialize has been called
  public:
    bool m_isInitialized = false;

    /// \brief the string property
  public:
    std::string m_propertyString;

    /// \brief the integer property
  public:
    int m_propertyInt = 0;

    /// \brief whether initialize should fail
  public:
    bool m_initializeFail = false;

    /// \brief the number of times the tool of the given name has been
    /// instantiated
  public:
    static int& instance_counts (const std::string& name);
  };
}

#endif
