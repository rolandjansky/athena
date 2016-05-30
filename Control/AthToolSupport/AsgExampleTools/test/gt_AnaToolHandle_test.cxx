//        Copyright Iowa State University 2016.
//                  Author: Nils Krumnack
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (nils.erik.krumnack@cern.ch) for bug
// reports, feature suggestions, praise and complaints.


//
// includes
//

#include <AsgTools/AnaToolHandle.h>
#include <AsgTools/MessageCheck.h>
#include <AsgTools/UnitTest.h>
#include <AsgExampleTools/UnitTestTool1.h>
#include <AsgExampleTools/UnitTestTool2.h>
#include <cmath>
#include <gtest/gtest.h>
#include <sstream>

#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

//
// method implementations
//

using namespace asg::msgUserCode;

namespace asg
{
  namespace
  {
#ifndef ROOTCORE
    /// \brief make a unique tool name to be used in unit tests
    std::string makeUniqueName ()
    {
      static unsigned index = 0;
      std::ostringstream str;
      str << "unique" << ++ index;
      return str.str();
    }
#endif
  }



  /// \brief fixture for all tests that do things before a tool is made
  ///
  /// e.g. actually making the tool or checking that methods fail
  /// correctly when the tool is not yet made.
  class AnaToolHandleMakeTest : public ::testing::Test
  {
  public:
    AnaToolHandleMakeTest()
      : tool ("asg::UnitTestTool1/test")
    {}

    AnaToolHandle<IUnitTestTool1> tool;
  };


  /// \brief fixture for all tests that do things before a tool is
  /// initialized
  ///
  /// e.g. setting properties and then initializing, or checking that
  /// various methods fail in this state
  class AnaToolHandleSetTest : public AnaToolHandleMakeTest
  {
  public:
    AnaToolHandleSetTest()
    {
      ANA_CHECK_THROW (tool.make ());
    }
  };


  /// \brief fixture for all tests that do things after a tool is
  /// initialized
  ///
  /// e.g. accessing the tool itself, or checking that various methods
  /// fail in this state
  class AnaToolHandleInitTest : public AnaToolHandleSetTest
  {
  public:
    AnaToolHandleInitTest()
    {
      ANA_CHECK_THROW (tool.initialize());
    }
  };


  /// \brief fixture for all tests that do things when a tool is in
  /// broken state
  ///
  /// mostly checking that we don't do anything forbidden
  class AnaToolHandleBrokenTest : public AnaToolHandleSetTest
  {
  public:
    AnaToolHandleBrokenTest()
    {
      AnaToolHandle<IUnitTestTool1> tool2 = std::move (tool);
    }
  };




  // check that we can create and initialize a simple tool
  TEST (AnaToolHandleTest, basic1)
  {
    AnaToolHandle<IUnitTestTool1> tool ("asg::UnitTestTool1/test");
    ASSERT_FALSE (tool.isUserConfigured());
    ASSERT_TRUE (tool.isConfigurable());
    ASSERT_SUCCESS (tool.make());
    ASSERT_FALSE (tool.isUserConfigured());
    ASSERT_TRUE (tool.isConfigurable());
    ASSERT_SUCCESS (tool.initialize());
    ASSERT_FALSE (tool.isUserConfigured());
    ASSERT_TRUE (tool.isConfigurable());
    ASSERT_TRUE (tool.get() != nullptr);
  }

  // check that we can create and initialize a simple tool
  TEST (AnaToolHandleTest, basic2)
  {
    AnaToolHandle<IUnitTestTool2> tool ("asg::UnitTestTool2/test");
    ASSERT_FALSE (tool.isUserConfigured());
    ASSERT_TRUE (tool.isConfigurable());
    ASSERT_SUCCESS (tool.make());
    ASSERT_SUCCESS (tool.initialize());
    ASSERT_TRUE (tool.get() != nullptr);
  }



  // check that inPremakeState is working
  TEST (AnaToolHandleTest, inPremakeState)
  {
    AnaToolHandle<IUnitTestTool1> tool ("asg::UnitTestTool1/test");
    ASSERT_TRUE (tool.inPremakeState());
    ASSERT_SUCCESS (tool.make());
    ASSERT_FALSE (tool.inPremakeState());
    ASSERT_SUCCESS (tool.initialize());
    ASSERT_FALSE (tool.inPremakeState());
    AnaToolHandle<IUnitTestTool1> tool2 = std::move (tool);
    ASSERT_FALSE (tool.inPremakeState());
  }



  // check that the isInitialized member works as advertised
  TEST (AnaToolHandleTest, isInitialized)
  {
    AnaToolHandle<IUnitTestTool1> tool ("asg::UnitTestTool1/test");
    ASSERT_FALSE (tool.isInitialized());
    ASSERT_SUCCESS (tool.make());
    ASSERT_FALSE (tool.isInitialized());
    ASSERT_SUCCESS (tool.initialize());
    ASSERT_TRUE (tool.isInitialized());
    AnaToolHandle<IUnitTestTool1> tool2 = std::move (tool);
    ASSERT_FALSE (tool.isInitialized());
  }



  // check that inBrokenState is working
  TEST (AnaToolHandleTest, inBrokenState)
  {
    AnaToolHandle<IUnitTestTool1> tool ("asg::UnitTestTool1/test");
    ASSERT_FALSE (tool.inBrokenState());
    ASSERT_SUCCESS (tool.make());
    ASSERT_FALSE (tool.inBrokenState());
    ASSERT_SUCCESS (tool.initialize());
    ASSERT_FALSE (tool.inBrokenState());
    AnaToolHandle<IUnitTestTool1> tool2 = std::move (tool);
    ASSERT_TRUE (tool.inBrokenState());
  }



  // check make()
  TEST_F (AnaToolHandleMakeTest, makeBasic)
  {
    ASSERT_SUCCESS (tool.make());
    ASSERT_FALSE (tool.inPremakeState());
    ASSERT_SUCCESS (tool.initialize());
    ASSERT_EQ ("asg::UnitTestTool1", tool.type());
    ASSERT_MATCH_REGEX ("^(ToolSvc.)?test$", tool->name());
  }

  // check make(), with no type
  TEST (AnaToolHandleTest, makeBasic_noType)
  {
    AnaToolHandle<IUnitTestTool1> tool ("test");
#ifdef ROOTCORE
    ASSERT_FAILURE (tool.make ());
#else
    ASSERT_SUCCESS (tool.make ());
    ASSERT_FAILURE (tool.initialize ());
#endif
  }

  // check make(), with an invalid type
  TEST (AnaToolHandleTest, makeBasic_undefinedType)
  {
    AnaToolHandle<IUnitTestTool1> tool ("UNKNOWN_TOOL_TYPE/test");
#ifdef ROOTCORE
    ASSERT_FAILURE (tool.make ());
#else
    ASSERT_SUCCESS (tool.make ());
    ASSERT_FAILURE (tool.initialize ());
#endif
  }

  // check make()
  TEST_F (AnaToolHandleSetTest, makeBasic)
  {
    ASSERT_DEATH (tool.make(), "");
  }

  // check make()
  TEST_F (AnaToolHandleInitTest, makeBasic)
  {
    ASSERT_DEATH (tool.make(), "");
  }

  // check make()
  TEST_F (AnaToolHandleBrokenTest, makeBasic)
  {
    ASSERT_FAILURE (tool.make());
  }



  // check make(type)
  TEST_F (AnaToolHandleMakeTest, makeTyped)
  {
    // making separate ToolHandle with different type
    AnaToolHandle<IUnitTestTool1> tool ("UNKNOWN_TOOL_TYPE/test");
    ASSERT_EQ ("UNKNOWN_TOOL_TYPE", tool.type());
    ASSERT_SUCCESS (tool.make ("asg::UnitTestTool1"));
    ASSERT_EQ ("asg::UnitTestTool1", tool.type());
    ASSERT_FALSE (tool.inPremakeState());
    ASSERT_SUCCESS (tool.initialize());
    ASSERT_MATCH_REGEX ("^(ToolSvc.)?test$", tool->name());
  }

  // check make(type)
  TEST_F (AnaToolHandleMakeTest, makeTyped_named)
  {
    // making separate ToolHandle with different type
    AnaToolHandle<IUnitTestTool1> tool ("UNKNOWN_TOOL_TYPE/UNKNOWN_TOOL_NAME");
    ASSERT_EQ ("UNKNOWN_TOOL_TYPE", tool.type());
    ASSERT_EQ ("UNKNOWN_TOOL_NAME", tool.name());
    ASSERT_SUCCESS (tool.make ("asg::UnitTestTool1/test"));
    ASSERT_EQ ("asg::UnitTestTool1", tool.type());
    ASSERT_MATCH_REGEX ("^(ToolSvc.)?test$", tool.name());
    ASSERT_FALSE (tool.inPremakeState());
    ASSERT_SUCCESS (tool.initialize());
    ASSERT_MATCH_REGEX ("^(ToolSvc.)?test$", tool->name());
  }

  // check that we can fail tocreate and initialize a simple tool when
  // passing an unknown typename as argument into make()
  TEST_F (AnaToolHandleMakeTest, makeTyped_unknownType)
  {
#ifdef ROOTCORE
    ASSERT_FAILURE (tool.make ("UNDEFINED_TYPE_NAME"));
#else
    ASSERT_SUCCESS (tool.make ("UNDEFINED_TYPE_NAME"));
    ASSERT_FAILURE (tool.initialize ());
#endif
  }

  // check make(type)
  TEST_F (AnaToolHandleSetTest, makeTyped)
  {
    ASSERT_DEATH (tool.make ("asg::UnitTestTool1"), "");
  }

  // check make(type)
  TEST_F (AnaToolHandleInitTest, makeTyped)
  {
    ASSERT_DEATH (tool.make ("asg::UnitTestTool1"), "");
  }

  // check make(type)
  TEST_F (AnaToolHandleBrokenTest, makeTyped)
  {
    ASSERT_FAILURE (tool.make ("asg::UnitTestTool1"));
  }



#ifdef ROOTCORE
  // check makeNew<type>()
  TEST_F (AnaToolHandleMakeTest, makeNew)
  {
    // making separate ToolHandle with different type
    AnaToolHandle<IUnitTestTool1> tool ("UNKNOWN_TOOL_TYPE/test");
    ASSERT_EQ ("UNKNOWN_TOOL_TYPE", tool.type());
    ASSERT_SUCCESS (tool.makeNew<asg::UnitTestTool1> ("asg::UnitTestTool1"));
    ASSERT_EQ ("asg::UnitTestTool1", tool.type());
    ASSERT_FALSE (tool.inPremakeState());
    ASSERT_SUCCESS (tool.initialize());
    ASSERT_MATCH_REGEX ("^(ToolSvc.)?test$", tool->name());
  }

  // check makeNew<type>()
  TEST_F (AnaToolHandleSetTest, makeNew)
  {
    ASSERT_DEATH (tool.makeNew<asg::UnitTestTool1> ("asg::UnitTestTool1"), "");
  }

  // check makeNew<type>()
  TEST_F (AnaToolHandleInitTest, makeNew)
  {
    ASSERT_DEATH (tool.makeNew<asg::UnitTestTool1> ("asg::UnitTestTool1"), "");
  }

  // check makeNew<type>()
  TEST_F (AnaToolHandleBrokenTest, makeNew)
  {
    ASSERT_FAILURE (tool.makeNew<asg::UnitTestTool1> ("asg::UnitTestTool1"));
  }
#endif



  // check ASG_MAKE_ANA_TOOL
  TEST_F (AnaToolHandleMakeTest, makeMacro)
  {
    // making separate ToolHandle with different type
    AnaToolHandle<IUnitTestTool1> tool ("UNKNOWN_TOOL_TYPE/test");
    ASSERT_EQ ("UNKNOWN_TOOL_TYPE", tool.type());
    ASSERT_SUCCESS (ASG_MAKE_ANA_TOOL (tool, asg::UnitTestTool1));
    ASSERT_EQ ("asg::UnitTestTool1", tool.type());
    ASSERT_FALSE (tool.inPremakeState());
    ASSERT_SUCCESS (tool.initialize());
    ASSERT_MATCH_REGEX ("^(ToolSvc.)?test$", tool->name());
  }

  // check ASG_MAKE_ANA_TOOL
  TEST_F (AnaToolHandleSetTest, makeMacro)
  {
    ASSERT_DEATH (ASG_MAKE_ANA_TOOL (tool, asg::UnitTestTool1), "");
  }

  // check ASG_MAKE_ANA_TOOL
  TEST_F (AnaToolHandleInitTest, makeMacro)
  {
    ASSERT_DEATH (ASG_MAKE_ANA_TOOL (tool, asg::UnitTestTool1), "");
  }

  // check ASG_MAKE_ANA_TOOL
  TEST_F (AnaToolHandleBrokenTest, makeMacro)
  {
    ASSERT_FAILURE (ASG_MAKE_ANA_TOOL (tool, asg::UnitTestTool1));
  }



  // check that we can actually change the tool type
  TEST (AnaToolHandleTest, changeType)
  {
    AnaToolHandle<IUnitTestTool1> tool ("asg::UnitTestTool1A/test");
    ASSERT_SUCCESS (tool.make());
    ASSERT_SUCCESS (tool.initialize());
    ASSERT_EQ (-7, tool->getPropertyInt());
  }




  // check setProperty<int>()
  TEST_F (AnaToolHandleSetTest, setPropertyInt)
  {
    ASSERT_SUCCESS (tool.setProperty<int> ("propertyInt", 42));
    ASSERT_SUCCESS (tool.initialize());
    ASSERT_EQ (42, tool->getPropertyInt());
  }

  // check setProperty<int>()
  TEST_F (AnaToolHandleSetTest, setPropertyInt_failure)
  {
    ASSERT_SUCCESS (tool.setProperty<int> ("UNKNOWN_PROPERTY", 42));
    ASSERT_FAILURE (tool.initialize ());
  }

  // check setProperty<int>()
  TEST_F (AnaToolHandleMakeTest, setPropertyInt)
  {
    ASSERT_SUCCESS (tool.setProperty<int> ("propertyInt", 42));
    ASSERT_SUCCESS (tool.initialize());
    ASSERT_EQ (42, tool->getPropertyInt());
  }

  // check setProperty<int>()
  TEST_F (AnaToolHandleInitTest, setPropertyInt)
  {
    ASSERT_DEATH (tool.setProperty<int> ("propertyInt", 42), "");
  }

  // check setProperty<int>()
  TEST_F (AnaToolHandleBrokenTest, setPropertyInt)
  {
    ASSERT_FAILURE (tool.setProperty<int> ("propertyInt", 42));
  }



  // check initialize()
  TEST_F (AnaToolHandleSetTest, initialize)
  {
    ASSERT_SUCCESS (tool.initialize());
    ASSERT_TRUE (tool.isInitialized());
    ASSERT_TRUE (tool->isInitialized());
  }

  // check initialize()
  TEST_F (AnaToolHandleSetTest, initialize_failure)
  {
    ASSERT_SUCCESS (tool.setProperty<bool> ("initializeFail", true));
    ASSERT_FAILURE (tool.initialize());
    ASSERT_FALSE (tool.isInitialized());
    ASSERT_TRUE (tool.inBrokenState());
  }

  // check initialize()
  TEST_F (AnaToolHandleMakeTest, initialize)
  {
    ASSERT_SUCCESS (tool.initialize());
    ASSERT_TRUE (tool.isInitialized());
    ASSERT_TRUE (tool->isInitialized());
  }

  // check initialize()
  TEST_F (AnaToolHandleInitTest, initialize)
  {
    ASSERT_DEATH (tool.initialize(), "");
  }

  // check initialize()
  TEST_F (AnaToolHandleBrokenTest, initialize)
  {
    ASSERT_FAILURE (tool.initialize ());
  }



  // check get() (and by implication * and ->)
  TEST_F (AnaToolHandleInitTest, get)
  {
    IUnitTestTool1 *mytool = tool.get();
    ASSERT_TRUE (mytool != nullptr);
    ASSERT_MATCH_REGEX ("^(ToolSvc.)?test$", mytool->name());
  }

  // check get() (and by implication * and ->)
  TEST_F (AnaToolHandleMakeTest, get)
  {
    ASSERT_DEATH (tool.get(), "");
  }

  // check get() (and by implication * and ->)
  TEST_F (AnaToolHandleSetTest, get)
  {
    ASSERT_DEATH (tool.get(), "");
  }

  // check get() (and by implication * and ->)
  TEST_F (AnaToolHandleBrokenTest, get)
  {
    ASSERT_DEATH (tool.get(), "");
  }



  // check that we can create a tool handle with the same name but
  // different parameters after we are done with the first one.  this
  // is routinely done as part of unit tests, so this absolutely has
  // to stay
  TEST (AnaToolHandleTest, duplicate_series)
  {
    {
      AnaToolHandle<IUnitTestTool1> tool ("asg::UnitTestTool1/test");
      ASSERT_FALSE (tool.isUserConfigured());
      ASSERT_TRUE (tool.isConfigurable());
      ASSERT_SUCCESS (tool.setProperty<int> ("propertyInt", 42));
      ASSERT_SUCCESS (tool.initialize());
      ASSERT_EQ (42, tool->getPropertyInt());
    }
    {
      AnaToolHandle<IUnitTestTool1> tool ("asg::UnitTestTool1/test");
      ASSERT_FALSE (tool.isUserConfigured());
      ASSERT_TRUE (tool.isConfigurable());
      ASSERT_SUCCESS (tool.setProperty<int> ("propertyInt", 4));
      ASSERT_SUCCESS (tool.initialize());
      ASSERT_EQ (4, tool->getPropertyInt());
    }
    {
      AnaToolHandle<IUnitTestTool1> tool ("asg::UnitTestTool1/test");
      ASSERT_FALSE (tool.isUserConfigured());
      ASSERT_TRUE (tool.isConfigurable());
      ASSERT_SUCCESS (tool.setProperty<int> ("propertyInt", 17));
      ASSERT_SUCCESS (tool.initialize());
      ASSERT_EQ (17, tool->getPropertyInt());
    }
  }



  // check that a tool gets shared between two AnaToolHandle objects
  // if they are given the same name and have no parents.
  TEST (AnaToolHandleTest, shared_tool)
  {
    {
      AnaToolHandle<IUnitTestTool1> tool1 ("asg::UnitTestTool1/test");
      EXPECT_FALSE (tool1.isUserConfigured());
      EXPECT_TRUE (tool1.isConfigurable());
      ASSERT_SUCCESS (tool1.setProperty<int> ("propertyInt", 42));
      EXPECT_FALSE (tool1.isUserConfigured());
      EXPECT_TRUE (tool1.isConfigurable());
      ASSERT_SUCCESS (tool1.initialize());
      EXPECT_FALSE (tool1.isUserConfigured());
      EXPECT_TRUE (tool1.isConfigurable());

      AnaToolHandle<IUnitTestTool1> tool2 ("asg::UnitTestTool1/test");
      EXPECT_TRUE (tool2.isUserConfigured());
      EXPECT_FALSE (tool2.isConfigurable());
      ASSERT_SUCCESS (tool2.setProperty<int> ("propertyInt", 7));
      EXPECT_TRUE (tool2.isUserConfigured());
      EXPECT_FALSE (tool2.isConfigurable());
      ASSERT_SUCCESS (tool2.initialize());
      EXPECT_TRUE (tool2.isUserConfigured());
      EXPECT_FALSE (tool2.isConfigurable());

      ASSERT_EQ (tool1.get(), tool2.get());
      ASSERT_EQ (42, tool1->getPropertyInt());
    }
    AnaToolHandle<IUnitTestTool1> tool1 ("asg::UnitTestTool1/test");
    EXPECT_FALSE (tool1.isUserConfigured());
    EXPECT_TRUE (tool1.isConfigurable());
    ASSERT_SUCCESS (tool1.setProperty<int> ("propertyInt", 17));
    ASSERT_SUCCESS (tool1.initialize());
  }



  // check that in Athena we pick up whether set job options cause us
  // to flag a tool as user configured
#ifndef ROOTCORE
  TEST (AnaToolHandleTest, usesJobOptions)
  {
    // this is a dummy tool that is only used to fill the job options
    // service.  it is purposely not initialized to make sure that it
    // does nothing but set the job options
    AnaToolHandle<IUnitTestTool1> tool1 ("asg::UnitTestTool1/test");
    ASSERT_SUCCESS (tool1.setProperty<int> ("propertyInt", 42));

    AnaToolHandle<IUnitTestTool1> tool2 ("asg::UnitTestTool1/test");
    EXPECT_TRUE (tool2.isUserConfigured());
    EXPECT_FALSE (tool2.isConfigurable());
    ASSERT_SUCCESS (tool2.setProperty<int> ("propertyInt", 7));
    ASSERT_SUCCESS (tool2.initialize());

    ASSERT_EQ (42, tool2->getPropertyInt());
  }
#endif



  template <typename TH>
  struct SetToolHandlePropertyTest : public ::testing::Test
  {
    void testSetToolHandle (const std::string& handleName,
			    bool isRegular, bool /*isPublic*/)
    {
      SCOPED_TRACE (handleName);

      TH par;
      AnaToolHandle<IUnitTestTool2> mainTool ("asg::UnitTestTool2/mainTool");

#ifdef ROOTCORE
      if (!par.isSettable())
#else
      if (!par.isSettable() || !par.isGettable())
#endif
      {
	ASSERT_FAILURE (mainTool.setProperty (handleName, par.handle));
	return;
      }
      ASSERT_SUCCESS (mainTool.setProperty (handleName, par.handle));

      if (!isRegular && !par.isGettable())
      {
	ASSERT_FAILURE (mainTool.initialize ());
	return;
      }
      ASSERT_SUCCESS (mainTool.initialize ());

      if (!par.isGettable())
      {
	ASSERT_ANY_THROW (mainTool->getToolHandle (handleName));
	return;
      }
      auto *const subtool = mainTool->getToolHandle (handleName);
#ifdef ROOTCORE
      EXPECT_EQ (&*par.handle, subtool);
#else
      EXPECT_EQ (par.handle->getPropertyInt(), subtool->getPropertyInt());
#endif
      if (!isRegular)
      {
	EXPECT_TRUE (mainTool->wasUserConfigured (handleName));
      } else
      {
	EXPECT_SUCCESS (mainTool->retrieveToolHandle (handleName));
	EXPECT_EQ (subtool, mainTool->getToolHandle (handleName));
      }
    }
  };

  TYPED_TEST_CASE_P (SetToolHandlePropertyTest);

  TYPED_TEST_P (SetToolHandlePropertyTest, setRegPublicHandle)
  {
    this->testSetToolHandle ("regPublicHandle", true, true);
  }

  TYPED_TEST_P (SetToolHandlePropertyTest, setRegPrivateHandle)
  {
    this->testSetToolHandle ("regPrivateHandle", true, false);
  }

  TYPED_TEST_P (SetToolHandlePropertyTest, setAnaPublicHandle)
  {
    this->testSetToolHandle ("anaPublicHandle", false, true);
  }

  TYPED_TEST_P (SetToolHandlePropertyTest, setAnaPrivateHandle)
  {
    this->testSetToolHandle ("anaPrivateHandle", false, false);
  }

  REGISTER_TYPED_TEST_CASE_P (SetToolHandlePropertyTest, setRegPublicHandle, setRegPrivateHandle, setAnaPublicHandle, setAnaPrivateHandle);



  struct PublicAnaSubTool
  {
    PublicAnaSubTool ()
    {
      ANA_CHECK_THROW (handle.make ("asg::UnitTestTool1/subtool"));
      ANA_CHECK_THROW (handle.initialize ());
    }

    bool isSettable () const {return true;}
    bool isGettable () const {return true;}

    AnaToolHandle<IUnitTestTool1> handle;
  };
  INSTANTIATE_TYPED_TEST_CASE_P (PublicAnaSubToolTest, SetToolHandlePropertyTest, PublicAnaSubTool);

  struct PrivateAnaSubTool
  {
    PrivateAnaSubTool ()
      : parent (new UnitTestTool1 ("parentTool")),
	handle ("asg::UnitTestTool1/subtool", parent.get())
    {
      ANA_CHECK_THROW (parent->initialize ());
      ANA_CHECK_THROW (handle.initialize ());
    }

    bool isSettable () const {return true;}
    bool isGettable () const {return true;}

    std::unique_ptr<UnitTestTool1> parent;
    AnaToolHandle<IUnitTestTool1> handle;
  };
  INSTANTIATE_TYPED_TEST_CASE_P (PrivateAnaSubToolTest, SetToolHandlePropertyTest, PrivateAnaSubTool);

  struct EmptyRegSubTool
  {
    EmptyRegSubTool ()
    {
    }

    bool isSettable () const {return false;}
    bool isGettable () const {return false;}

    ToolHandle<IUnitTestTool1> handle;
  };
  INSTANTIATE_TYPED_TEST_CASE_P (EmptyRegSubToolTest, SetToolHandlePropertyTest, EmptyRegSubTool);

  struct InvalidSubTool
  {
    InvalidSubTool ()
      : handle ("UNDEFINED_TOOL_TYPE/UNDEFINED_TOOL_NAME")
    {
    }

    bool isSettable () const {return true;}
    bool isGettable () const {return false;}

    ToolHandle<IUnitTestTool1> handle;
  };
  INSTANTIATE_TYPED_TEST_CASE_P (InvalidSubToolTest, SetToolHandlePropertyTest, InvalidSubTool);

  struct NamedSubTool
  {
    NamedSubTool ()
#ifdef ROOTCORE
      : mytool (new UnitTestTool1 ("sharedTestTool")),
	handle ("sharedTestTool")
#else
      : handle ("asg::UnitTestTool1/sharedTestTool")
#endif
    {
#ifdef ROOTCORE
      ANA_CHECK_THROW (mytool->initialize ());
#else
      ANA_CHECK_THROW (handle.retrieve ());
#endif
    }

    bool isSettable () const {return true;}
    bool isGettable () const {return true;}

#ifdef ROOTCORE
    std::unique_ptr<UnitTestTool1> mytool;
#endif
    ToolHandle<IUnitTestTool1> handle;
  };
  INSTANTIATE_TYPED_TEST_CASE_P (NamedSubToolTest, SetToolHandlePropertyTest, NamedSubTool);

#ifdef ROOTCORE
  struct PointerRegSubTool
  {
    PointerRegSubTool ()
    {
      ANA_CHECK_THROW (tool.make ("asg::UnitTestTool1/subtool"));
      ANA_CHECK_THROW (tool.initialize ());
      handle = tool.get ();
    }

    bool isSettable () const {return true;}
    bool isGettable () const {return true;}

    AnaToolHandle<IUnitTestTool1> tool;
    ToolHandle<IUnitTestTool1> handle;
  };
  INSTANTIATE_TYPED_TEST_CASE_P (PointerRegSubToolTest, SetToolHandlePropertyTest, PointerRegSubTool);
#endif



#ifndef ROOTCORE
  TEST (AnaToolHandleTest, athena_job_options_property)
  {
    std::string name = makeUniqueName();
    ServiceHandle<IJobOptionsSvc> joSvc("JobOptionsSvc","");
    joSvc->addPropertyToCatalogue ("ToolSvc." + name, StringProperty("propertyInt", "57"));

    AnaToolHandle<IUnitTestTool1> handle ("asg::UnitTestTool1/" + name);
    ASSERT_TRUE (handle.isUserConfigured());
    ASSERT_SUCCESS (handle.setProperty("propertyInt", 17));
    ASSERT_SUCCESS (handle.initialize());
    EXPECT_EQ ("ToolSvc." + name, handle->name());
    EXPECT_EQ (57, handle->getPropertyInt ());
  }

  TEST (AnaToolHandleTest, athena_job_options_misspelled)
  {
    std::string name = makeUniqueName();
    ServiceHandle<IJobOptionsSvc> joSvc("JobOptionsSvc","");
    joSvc->addPropertyToCatalogue ("ToolSvc." + name, StringProperty("invalid", "57"));

    AnaToolHandle<IUnitTestTool1> handle ("asg::UnitTestTool1/" + name);
    ASSERT_TRUE (handle.isUserConfigured());
    ASSERT_FAILURE (handle.initialize());
  }

  TEST (AnaToolHandleTest, athena_job_options_type_private)
  {
    std::string name = makeUniqueName();
    ServiceHandle<IJobOptionsSvc> joSvc("JobOptionsSvc","");
    joSvc->addPropertyToCatalogue ("ToolSvc." + name, StringProperty("anaPrivateHandle", "asg::UnitTestTool1A/anaPrivateHandle"));

    AnaToolHandle<IUnitTestTool2> handle ("asg::UnitTestTool2/" + name);
    ASSERT_TRUE (handle.isUserConfigured());
    ASSERT_SUCCESS (handle.initialize());
    ASSERT_TRUE (handle->wasUserConfigured ("anaPrivateHandle"));
    ASSERT_EQ (-7, handle->getToolHandle ("anaPrivateHandle")->getPropertyInt());
  }

  TEST (AnaToolHandleTest, athena_job_options_subtool)
  {
    std::string name = makeUniqueName();
    ServiceHandle<IJobOptionsSvc> joSvc("JobOptionsSvc","");
    joSvc->addPropertyToCatalogue ("ToolSvc." + name, StringProperty("anaPrivateHandle", "asg::UnitTestTool1/anaPrivateHandle"));
    joSvc->addPropertyToCatalogue ("ToolSvc." + name + ".anaPrivateHandle", StringProperty("propertyInt", "48"));

    AnaToolHandle<IUnitTestTool2> handle ("asg::UnitTestTool2/" + name);
    EXPECT_TRUE (handle.isUserConfigured());
    ASSERT_SUCCESS (handle.initialize());
    ASSERT_TRUE (handle->wasUserConfigured ("anaPrivateHandle"));
    EXPECT_EQ ("ToolSvc." + name, handle->name());
    EXPECT_EQ ("ToolSvc." + name + ".anaPrivateHandle", handle->getToolHandle ("anaPrivateHandle")->name());
    EXPECT_EQ (48, handle->getToolHandle ("anaPrivateHandle")->getPropertyInt());
  }
#endif



  TEST (AnaToolHandleTest, swap_test)
  {
    AnaToolHandle<IAsgTool> tool1("asg::UnitTestTool1/myTool");
    ASSERT_SUCCESS (tool1.initialize());
#ifndef ROOTCORE
    EXPECT_EQ (3u, tool1->refCount());
#endif

    asg::AnaToolHandle<asg::IAsgTool> tool3;

    {
      asg::AnaToolHandle<asg::IAsgTool> tool2("asg::UnitTestTool1/myTool");
      ASSERT_SUCCESS (tool2.initialize());
#ifndef ROOTCORE
      EXPECT_EQ (4u, tool2->refCount());
#endif
      tool3 = std::move(tool2);
#ifndef ROOTCORE
      EXPECT_EQ (4u, tool3->refCount());
#endif
    }
#ifndef ROOTCORE
    EXPECT_EQ (4u, tool3->refCount());
    EXPECT_EQ (4u, tool1->refCount());
#endif
  }
}

ATLAS_GOOGLE_TEST_MAIN
