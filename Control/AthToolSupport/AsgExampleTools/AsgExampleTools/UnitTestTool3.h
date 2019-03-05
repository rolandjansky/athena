/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef ASG_TOOLS__UNIT_TEST_TOOL3_H
#define ASG_TOOLS__UNIT_TEST_TOOL3_H

#include <AsgTools/AnaToolHandle.h>
#include <AsgTools/AsgTool.h>
#include <AsgExampleTools/IUnitTestTool3.h>

namespace asg
{
  class IUnitTestTool2;

  /// \brief a tool used to unit test AnaToolHandle
  ///
  /// This allows to unit test that I can use an AnaToolHandle
  /// property to provide a customization point to the user and then
  /// pass it on as a property to another tool it holds and creates
  /// via an AnaToolHandle.

  struct UnitTestTool3 : virtual public IUnitTestTool3,
			 public AsgTool
  {
    ASG_TOOL_CLASS (UnitTestTool3, IUnitTestTool3)

    /// \brief standard constructor
  public:
    UnitTestTool3 (const std::string& val_name);

  public:
    StatusCode initialize () override;

    /// \brief get the subtool we configured
  public:
    virtual const IUnitTestTool1 *getSubtool () const override;

    /// \brief get the subtool configured for our subtool
  public:
    virtual const IUnitTestTool1 *getSubsubtool () const override;

  public:
    virtual bool subsubtoolEmpty () const override;

    /// \brief a public tool handle the tool the user configures
  private:
    ToolHandle<IUnitTestTool1> m_subtool0;

    /// \brief the tool the user configures
  private:
    AnaToolHandle<IUnitTestTool1> m_subtool1;

    /// \brief the tool we configure
  private:
    AnaToolHandle<IUnitTestTool2> m_subtool2;

    /// \brief the property name we set on \ref m_subsubtool
  private:
    std::string m_propertyName;

    /// \brief whether to use the public tool handle
  private:
    bool m_usePublic = false;
  };
}

#endif
