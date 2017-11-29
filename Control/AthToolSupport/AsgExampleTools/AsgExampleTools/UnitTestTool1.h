#ifndef ASG_TOOLS__UNIT_TEST_TOOL1_H
#define ASG_TOOLS__UNIT_TEST_TOOL1_H

//        Copyright Iowa State University 2016.
//                  Author: Nils Krumnack
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (nils.erik.krumnack@cern.ch) for bug
// reports, feature suggestions, praise and complaints.


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

  public:
    virtual MSG::Level getOrigMsgLevel () const override;


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

    /// \brief the message level at initialize
  public:
    MSG::Level m_origMsgLevel = MSG::INFO;

    /// \brief the number of times the tool of the given name has been
    /// instantiated
  public:
    static int& instance_counts (const std::string& name);
  };
}

#endif
