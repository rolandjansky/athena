#ifndef ASG_TOOLS__UNIT_TEST_TOOL2_H
#define ASG_TOOLS__UNIT_TEST_TOOL2_H

//        Copyright Iowa State University 2016.
//                  Author: Nils Krumnack
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (nils.erik.krumnack@cern.ch) for bug
// reports, feature suggestions, praise and complaints.


#include <AsgTools/AnaToolHandle.h>
#include <AsgTools/AsgTool.h>
#include <AsgExampleTools/IUnitTestTool2.h>

namespace asg
{
  /// \brief a tool used to unit test AnaToolHandle
  ///
  /// This allows to unit test anything that happens in conjunction
  /// with a tool holding a ToolHandle/AnaToolHandle.

  struct UnitTestTool2 : virtual public IUnitTestTool2,
			 public AsgTool
  {
    ASG_TOOL_CLASS (UnitTestTool2, IUnitTestTool2)

    /// \brief standard constructor
  public:
    UnitTestTool2 (const std::string& val_name);

  public:
    StatusCode initialize () override;

  public:
    virtual bool
    toolHandleEmpty (const std::string& handleName) const override;

  public:
    virtual const IUnitTestTool1 *
    getToolHandle (const std::string& handleName) const override;

  public:
    virtual StatusCode
    retrieveToolHandle (const std::string& handleName) const override;

  public:
    virtual bool
    wasUserConfigured (const std::string& handleName) const override;

  public:
    ToolHandle<IUnitTestTool1> m_regPublicHandle;

  public:
    ToolHandle<IUnitTestTool1> m_regPrivateHandle;

  public:
    AnaToolHandle<IUnitTestTool1> m_anaPublicHandle;

  public:
    AnaToolHandle<IUnitTestTool1> m_anaPrivateHandle;

  private:
    bool m_wasUserConfiguredPublic = false;

  private:
    bool m_wasUserConfiguredPrivate = false;

  private:
    bool m_allowEmpty = false;
  };
}

#endif
