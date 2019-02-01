/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <AsgTools/AnaToolHandle.h>
#include <AsgTools/MessageCheck.h>
#include <AsgTesting/UnitTest.h>
#include <AsgExampleTools/UnitTestTool1.h>
#include <AsgExampleTools/UnitTestTool2.h>
#include <AsgExampleTools/UnitTestTool3.h>
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
    /// \brief make a unique tool name to be used in unit tests
    std::string makeUniqueName ()
    {
      static unsigned index = 0;
      std::ostringstream str;
      str << "unique" << ++ index;
      return str.str();
    }
  }



  /// \brief fixture for all tests that do things before a tool is made
  ///
  /// e.g. actually making the tool or checking that methods fail
  /// correctly when the tool is not yet made.
  class AnaToolHandleMakeTest : public ::testing::Test
  {
  public:
    AnaToolHandleMakeTest()
      : name (makeUniqueName()),
	tool ("asg::UnitTestTool1/" + name)
    {}

    std::string name;
    AnaToolHandle<IUnitTestTool1> tool;
  };


  /// \brief fixture for all tests that do things after a tool is
  /// initialized
  ///
  /// e.g. accessing the tool itself, or checking that various methods
  /// fail in this state
  class AnaToolHandleUseTest : public AnaToolHandleMakeTest
  {
  public:
    AnaToolHandleUseTest()
    {
      ANA_CHECK_THROW (tool.initialize());
    }
  };




  // check that we can create and initialize a simple tool
  TEST (AnaToolHandleTest, basic1)
  {
    AnaToolHandle<IUnitTestTool1> tool ("asg::UnitTestTool1/" + makeUniqueName());
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



#ifndef ROOTCORE
  // check that tool destruction works as advertised for tools
  TEST (AnaToolHandleTest, DISABLED_destruct_direct)
  {
    std::string myname;
    {
      ToolHandle<IUnitTestTool1> tool ("asg::UnitTestTool1/" + makeUniqueName());
      ASSERT_SUCCESS (tool.retrieve());
      myname = tool->name();
      ASSERT_EQ (2u, tool->refCount());
      EXPECT_EQ (1, UnitTestTool1::instance_counts(myname));
      tool->release();
      EXPECT_EQ (1, UnitTestTool1::instance_counts(myname));
      tool.release();
      EXPECT_EQ (0, UnitTestTool1::instance_counts(myname));
    }
    interfaceType_t *tool = nullptr;
    EXPECT_FALSE (detail::toolExists (myname, tool));
  }
#endif



  // check that tool destruction works as advertised for public tools
#ifdef ROOTCORE
  TEST (AnaToolHandleTest, destruct_public)
#else
  TEST (AnaToolHandleTest, DISABLED_destruct_public)
#endif
  {
    std::string myname;
    {
      AnaToolHandle<IUnitTestTool1> tool ("asg::UnitTestTool1/" + makeUniqueName());
      ASSERT_SUCCESS (tool.initialize());
      myname = tool->name();
      EXPECT_EQ (1, UnitTestTool1::instance_counts(myname));
    }
    EXPECT_EQ (0, UnitTestTool1::instance_counts(myname));
  }


  // check that tool destruction works as advertised for private tools
#ifdef ROOTCORE
  TEST (AnaToolHandleTest, destruct_private)
#else
  TEST (AnaToolHandleTest, DISABLED_destruct_private)
#endif
  {
    std::string myname;
    {
      std::unique_ptr<UnitTestTool1> parent (new UnitTestTool1 (makeUniqueName()));
      AnaToolHandle<IUnitTestTool1> tool ("asg::UnitTestTool1/" + makeUniqueName(), parent.get());
      ASSERT_SUCCESS (tool.initialize());
      myname = tool->name();
      EXPECT_EQ (1, UnitTestTool1::instance_counts(myname));
    }
    EXPECT_EQ (0, UnitTestTool1::instance_counts(myname));
  }

  // check that tool destruction works as advertised with copy constructor
#ifdef ROOTCORE
  TEST (AnaToolHandleTest, copy_destruct)
#else
  TEST (AnaToolHandleTest, DISABLED_copy_destruct)
#endif
  {
    std::string name = makeUniqueName();
    EXPECT_EQ (0, UnitTestTool1::instance_counts(name));
    {
      AnaToolHandle<IUnitTestTool1> tool ("asg::UnitTestTool1/" + name);
      ASSERT_SUCCESS (tool.initialize());
      name = tool->name();
      EXPECT_EQ (1, UnitTestTool1::instance_counts(name));
      AnaToolHandle<IUnitTestTool1> tool2 = tool;
      EXPECT_EQ (1, UnitTestTool1::instance_counts(name));
    }
    EXPECT_EQ (0, UnitTestTool1::instance_counts(name));
  }

  // check that tool destruction works as advertised with assignment operator
#ifdef ROOTCORE
  TEST (AnaToolHandleTest, assign_destruct)
#else
  TEST (AnaToolHandleTest, DISABLED_assign_destruct)
#endif
  {
    std::string name = makeUniqueName();
    EXPECT_EQ (0, UnitTestTool1::instance_counts(name));
    {
      AnaToolHandle<IUnitTestTool1> tool ("asg::UnitTestTool1/" + name);
      ASSERT_SUCCESS (tool.initialize());
      name = tool->name();
      EXPECT_EQ (1, UnitTestTool1::instance_counts(name));
      AnaToolHandle<IUnitTestTool1> tool2;
      tool2 = tool;
      EXPECT_EQ (1, UnitTestTool1::instance_counts(name));
    }
    EXPECT_EQ (0, UnitTestTool1::instance_counts(name));
  }

  // check that tool destruction works as advertised with swap function
  //
  // not that this swaps twice, as there may be a difference in
  // swapping to and from a valid handle (due to the specifics of the
  // Athena ToolHandle)
#ifdef ROOTCORE
  TEST (AnaToolHandleTest, swap_destruct)
#else
  TEST (AnaToolHandleTest, DISABLED_swap_destruct)
#endif
  {
    std::string name = makeUniqueName();
    EXPECT_EQ (0, UnitTestTool1::instance_counts(name));
    {
      AnaToolHandle<IUnitTestTool1> tool ("asg::UnitTestTool1/" + name);
      ASSERT_SUCCESS (tool.initialize());
      name = tool->name();
      EXPECT_EQ (1, UnitTestTool1::instance_counts(name));
      AnaToolHandle<IUnitTestTool1> tool2;
      tool2.swap (tool);
      EXPECT_EQ (1, UnitTestTool1::instance_counts(name));
      tool2.swap (tool);
      EXPECT_EQ (1, UnitTestTool1::instance_counts(name));
    }
    EXPECT_EQ (0, UnitTestTool1::instance_counts(name));
  }

  // check that we can create and initialize a simple tool
  TEST (AnaToolHandleTest, basic2)
  {
    AnaToolHandle<IUnitTestTool2> tool ("asg::UnitTestTool2/" + makeUniqueName());
    ASSERT_FALSE (tool.isUserConfigured());
    ASSERT_TRUE (tool.isConfigurable());
    ASSERT_SUCCESS (tool.make());
    ASSERT_SUCCESS (tool.initialize());
    ASSERT_TRUE (tool.get() != nullptr);
  }



  // check that the isInitialized member works as advertised
  TEST (AnaToolHandleTest, isInitialized)
  {
    AnaToolHandle<IUnitTestTool1> tool ("asg::UnitTestTool1/" + makeUniqueName());
    ASSERT_FALSE (tool.isInitialized());
    ASSERT_SUCCESS (tool.make());
    ASSERT_FALSE (tool.isInitialized());
    ASSERT_SUCCESS (tool.initialize());
    ASSERT_TRUE (tool.isInitialized());
    AnaToolHandle<IUnitTestTool1> tool2 = std::move (tool);
    ASSERT_FALSE (tool.isInitialized());
  }



  // check make()
  TEST_F (AnaToolHandleMakeTest, makeBasic)
  {
    ASSERT_SUCCESS (tool.make());
    ASSERT_FALSE (tool.isInitialized());
    ASSERT_SUCCESS (tool.initialize());
    ASSERT_EQ ("asg::UnitTestTool1", tool.type());
    ASSERT_MATCH_REGEX ("^(ToolSvc.)?" + tool.name() + "$", tool->name());
  }

  // check make(), with no type
  TEST (AnaToolHandleTest, makeBasic_noType)
  {
    AnaToolHandle<IUnitTestTool1> tool (makeUniqueName());
    ASSERT_FALSE (tool.isUserConfigured ());
    ASSERT_FAILURE (tool.initialize ());
  }

  // check make(), with an invalid type
  TEST (AnaToolHandleTest, makeBasic_undefinedType)
  {
    AnaToolHandle<IUnitTestTool1> tool ("UNKNOWN_TOOL_TYPE/" + makeUniqueName());
    ASSERT_SUCCESS (tool.make ());
    ASSERT_FAILURE (tool.initialize ());
  }



  // check make(type)
  TEST (AnaToolHandleTest, makeTyped)
  {
    // making separate ToolHandle with different type
    AnaToolHandle<IUnitTestTool1> tool ("UNKNOWN_TOOL_TYPE/" + makeUniqueName());
    ASSERT_EQ ("UNKNOWN_TOOL_TYPE", tool.type());
    ASSERT_SUCCESS (tool.make ("asg::UnitTestTool1"));
    ASSERT_EQ ("asg::UnitTestTool1", tool.type());
    ASSERT_FALSE (tool.isInitialized());
    ASSERT_SUCCESS (tool.initialize());
    ASSERT_MATCH_REGEX ("^(ToolSvc.)?" + tool.name() + "$", tool->name());
  }

  // check make(type)
  TEST (AnaToolHandleTest, makeTyped_named)
  {
    // making separate ToolHandle with different type
    AnaToolHandle<IUnitTestTool1> tool ("UNKNOWN_TOOL_TYPE/UNKNOWN_TOOL_NAME");
    ASSERT_EQ ("UNKNOWN_TOOL_TYPE", tool.type());
    ASSERT_EQ ("UNKNOWN_TOOL_NAME", tool.name());
    std::string name = makeUniqueName();
    ASSERT_SUCCESS (tool.make ("asg::UnitTestTool1/" + name));
    ASSERT_EQ ("asg::UnitTestTool1", tool.type());
    ASSERT_MATCH_REGEX ("^(ToolSvc.)?" + name + "$", tool.name());
    ASSERT_FALSE (tool.isInitialized());
    ASSERT_SUCCESS (tool.initialize());
    ASSERT_MATCH_REGEX ("^(ToolSvc.)?" + name + "$", tool->name());
  }

  // check that we can fail tocreate and initialize a simple tool when
  // passing an unknown typename as argument into make()
  TEST_F (AnaToolHandleMakeTest, makeTyped_unknownType)
  {
    ASSERT_SUCCESS (tool.make ("UNDEFINED_TYPE_NAME"));
    ASSERT_FAILURE (tool.initialize ());
  }

#ifndef NDEBUG
  // check make(type)
  TEST_F (AnaToolHandleUseTest, makeTyped)
  {
    ASSERT_DEATH (tool.make ("asg::UnitTestTool1"), "");
  }
#endif



#ifdef ROOTCORE
  // check makeNew<type>()
  TEST (AnaToolHandleTest, makeNew)
  {
    // making separate ToolHandle with different type
    std::string name = makeUniqueName();
    AnaToolHandle<IUnitTestTool1> tool ("UNKNOWN_TOOL_TYPE/" + name);
    ASSERT_EQ ("UNKNOWN_TOOL_TYPE", tool.type());
    ASSERT_SUCCESS (tool.makeNew<asg::UnitTestTool1> ("asg::UnitTestTool1"));
    ASSERT_EQ ("asg::UnitTestTool1", tool.type());
    ASSERT_FALSE (tool.isInitialized());
    ASSERT_SUCCESS (tool.initialize());
    ASSERT_MATCH_REGEX ("^(ToolSvc.)?" + name + "$", tool->name());
  }

  // check makeNew<type>()
  TEST_F (AnaToolHandleUseTest, makeNew)
  {
    ASSERT_DEATH (tool.makeNew<asg::UnitTestTool1> ("asg::UnitTestTool1"), "");
  }
#endif



  // check ASG_SET_ANA_TOOL_TYPE
  TEST (AnaToolHandleTest, setToolTypeMacro)
  {
    // making separate ToolHandle with different type
    std::string name = makeUniqueName();
    AnaToolHandle<IUnitTestTool1> tool ("UNKNOWN_TOOL_TYPE/" + name);
    ASSERT_EQ ("UNKNOWN_TOOL_TYPE", tool.type());
    ASG_SET_ANA_TOOL_TYPE (tool, asg::UnitTestTool1);
    ASSERT_EQ ("asg::UnitTestTool1", tool.type());
    ASSERT_FALSE (tool.isInitialized());
    ASSERT_SUCCESS (tool.initialize());
    ASSERT_MATCH_REGEX ("^(ToolSvc.)?" + name + "$", tool->name());
  }



  // check ASG_MAKE_ANA_TOOL
  TEST (AnaToolHandleTest, makeMacro)
  {
    // making separate ToolHandle with different type
    std::string name = makeUniqueName();
    AnaToolHandle<IUnitTestTool1> tool ("UNKNOWN_TOOL_TYPE/" + name);
    ASSERT_EQ ("UNKNOWN_TOOL_TYPE", tool.type());
    ASSERT_SUCCESS (ASG_MAKE_ANA_TOOL (tool, asg::UnitTestTool1));
    ASSERT_EQ ("asg::UnitTestTool1", tool.type());
    ASSERT_FALSE (tool.isInitialized());
    ASSERT_SUCCESS (tool.initialize());
    ASSERT_MATCH_REGEX ("^(ToolSvc.)?" + name + "$", tool->name());
  }

#ifndef NDEBUG
  // check ASG_MAKE_ANA_TOOL
  TEST_F (AnaToolHandleUseTest, makeMacro)
  {
    ASSERT_DEATH (ASG_MAKE_ANA_TOOL (tool, asg::UnitTestTool1), "");
  }
#endif



  // check that we can actually change the tool type
  TEST (AnaToolHandleTest, changeType)
  {
    AnaToolHandle<IUnitTestTool1> tool ("asg::UnitTestTool1A/" + makeUniqueName());
    ASSERT_SUCCESS (tool.make());
    ASSERT_SUCCESS (tool.initialize());
    ASSERT_EQ (-7, tool->getPropertyInt());
  }




  // check setProperty<int>()
  TEST_F (AnaToolHandleMakeTest, setPropertyInt)
  {
    ASSERT_SUCCESS (tool.setProperty<int> ("propertyInt", 42));
    ASSERT_SUCCESS (tool.initialize());
    ASSERT_EQ (42, tool->getPropertyInt());
  }

  // check setProperty<int>()
  TEST_F (AnaToolHandleMakeTest, setPropertyInt_failure)
  {
    ASSERT_SUCCESS (tool.setProperty<int> ("UNKNOWN_PROPERTY", 42));
    ASSERT_FAILURE (tool.initialize ());
  }

#ifndef NDEBUG
  // check setProperty<int>()
  TEST_F (AnaToolHandleUseTest, setPropertyInt)
  {
    ASSERT_DEATH (tool.setProperty<int> ("propertyInt", 42), "");
  }
#endif



  // check setProperty(const char*)
  TEST_F (AnaToolHandleMakeTest, setPropertyString)
  {
    ASSERT_SUCCESS (tool.setProperty ("propertyString", "42"));
    ASSERT_SUCCESS (tool.initialize());
    ASSERT_EQ ("42", tool->getPropertyString());
  }

  // check setProperty(const char*)
  TEST_F (AnaToolHandleMakeTest, setPropertyString_failure)
  {
    ASSERT_SUCCESS (tool.setProperty ("UNKNOWN_PROPERTY", "42"));
    ASSERT_FAILURE (tool.initialize ());
  }

#ifndef NDEBUG
  // check setProperty(const char*)
  TEST_F (AnaToolHandleUseTest, setPropertyString)
  {
    ASSERT_DEATH (tool.setProperty ("propertyString", "42"), "");
  }
#endif



  // check initialize()
  TEST_F (AnaToolHandleMakeTest, initialize)
  {
    ASSERT_SUCCESS (tool.initialize());
    ASSERT_TRUE (tool.isInitialized());
    ASSERT_TRUE (tool->isInitialized());
  }

  // check initialize()
  TEST_F (AnaToolHandleMakeTest, initialize_failure)
  {
    ASSERT_SUCCESS (tool.setProperty<bool> ("initializeFail", true));
    ASSERT_FAILURE (tool.initialize());
    ASSERT_FALSE (tool.isInitialized());
  }

#ifndef NDEBUG
  // check initialize()
  TEST_F (AnaToolHandleUseTest, initialize)
  {
    ASSERT_DEATH (tool.initialize(), "");
  }
#endif



  // check get() (and by implication * and ->)
  TEST_F (AnaToolHandleUseTest, get)
  {
    IUnitTestTool1 *mytool = tool.get();
    ASSERT_TRUE (mytool != nullptr);
    ASSERT_MATCH_REGEX ("^(ToolSvc.)?" + tool.name() + "$", mytool->name());
  }

  // check get() (and by implication * and ->)
  TEST_F (AnaToolHandleMakeTest, get)
  {
    IUnitTestTool1 *mytool = tool.get();
    ASSERT_TRUE (mytool != nullptr);
    ASSERT_MATCH_REGEX ("^(ToolSvc.)?" + tool.name() + "$", mytool->name());
  }



#ifdef ROOTCORE
  // check that the message level gets set correctly from the property
  TEST (AnaToolHandleTest, setOutputLevel)
  {
    std::string name = makeUniqueName();
    AnaToolHandle<IUnitTestTool1> tool ("asg::UnitTestTool1/" + name);
    ASSERT_SUCCESS (tool.setProperty ("OutputLevel", MSG::ERROR));
    ASSERT_SUCCESS (tool.initialize());
    ASSERT_EQ (MSG::ERROR, tool->getOrigMsgLevel());
  }

  // check that the default message level is correct
  TEST (AnaToolHandleTest, defaultOutputLevel)
  {
    std::string name = makeUniqueName();
    AnaToolHandle<IUnitTestTool1> tool ("asg::UnitTestTool1/" + name);
    ASSERT_SUCCESS (tool.initialize());
    ASSERT_EQ (MSG::INFO, tool->getOrigMsgLevel());
  }
#endif


  // check that we can create a tool handle with the same name but
  // different parameters after we are done with the first one.  this
  // is routinely done as part of unit tests, so this absolutely has
  // to stay
#ifdef ROOTCORE
  TEST (AnaToolHandleTest, duplicate_series)
#else
  TEST (AnaToolHandleTest, DISABLED_duplicate_series)
#endif
  {
    std::string name = makeUniqueName();
    {
      AnaToolHandle<IUnitTestTool1> tool ("asg::UnitTestTool1/" + name);
      ASSERT_FALSE (tool.isUserConfigured());
      ASSERT_TRUE (tool.isConfigurable());
      ASSERT_SUCCESS (tool.setProperty<int> ("propertyInt", 42));
      ASSERT_SUCCESS (tool.initialize());
      ASSERT_EQ (42, tool->getPropertyInt());
    }
    {
      AnaToolHandle<IUnitTestTool1> tool ("asg::UnitTestTool1/" + name);
      ASSERT_FALSE (tool.isUserConfigured());
      ASSERT_TRUE (tool.isConfigurable());
      ASSERT_SUCCESS (tool.setProperty<int> ("propertyInt", 4));
      ASSERT_SUCCESS (tool.initialize());
      ASSERT_EQ (4, tool->getPropertyInt());
    }
    {
      AnaToolHandle<IUnitTestTool1> tool ("asg::UnitTestTool1/" + name);
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
    std::string name = makeUniqueName();
    {
      AnaToolHandle<IUnitTestTool1> tool1 ("asg::UnitTestTool1/" + name);
      EXPECT_FALSE (tool1.isUserConfigured());
      EXPECT_TRUE (tool1.isConfigurable());
      ASSERT_SUCCESS (tool1.setProperty<int> ("propertyInt", 42));
      EXPECT_FALSE (tool1.isUserConfigured());
      EXPECT_TRUE (tool1.isConfigurable());
      ASSERT_SUCCESS (tool1.initialize());
      EXPECT_FALSE (tool1.isUserConfigured());
      EXPECT_TRUE (tool1.isConfigurable());

      AnaToolHandle<IUnitTestTool1> tool2 ("asg::UnitTestTool1/" + name);
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
  }



  // check that a tool gets shared between two AnaToolHandle objects
  // if they are given the same name via setType/setName.
  TEST (AnaToolHandleTest, shared_tool_setName)
  {
    std::string name = makeUniqueName();
    {
      AnaToolHandle<IUnitTestTool1> tool1;
      tool1.setType ("asg::UnitTestTool1");
      tool1.setName (name);
      EXPECT_FALSE (tool1.isUserConfigured());
      EXPECT_TRUE (tool1.isConfigurable());
      ASSERT_SUCCESS (tool1.setProperty<int> ("propertyInt", 42));
      EXPECT_FALSE (tool1.isUserConfigured());
      EXPECT_TRUE (tool1.isConfigurable());
      ASSERT_SUCCESS (tool1.initialize());
      EXPECT_FALSE (tool1.isUserConfigured());
      EXPECT_TRUE (tool1.isConfigurable());

      AnaToolHandle<IUnitTestTool1> tool2;
      tool2.setType ("asg::UnitTestTool1");
      tool2.setName (name);
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
  }



  // check that a tool gets shared between two AnaToolHandle objects
  // if they are given the same name and have no parents.
#ifdef ROOTCORE
  TEST (AnaToolHandleTest, shared_tool_cleanup)
#else
  TEST (AnaToolHandleTest, DISABLED_shared_tool_cleanup)
#endif
  {
    std::string name = makeUniqueName();
    {
      AnaToolHandle<IUnitTestTool1> tool1 ("asg::UnitTestTool1/" + name);
      EXPECT_FALSE (tool1.isUserConfigured());
      EXPECT_TRUE (tool1.isConfigurable());
      ASSERT_SUCCESS (tool1.setProperty<int> ("propertyInt", 42));
      EXPECT_FALSE (tool1.isUserConfigured());
      EXPECT_TRUE (tool1.isConfigurable());
      ASSERT_SUCCESS (tool1.initialize());
      EXPECT_FALSE (tool1.isUserConfigured());
      EXPECT_TRUE (tool1.isConfigurable());

      AnaToolHandle<IUnitTestTool1> tool2 ("asg::UnitTestTool1/" + name);
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
    AnaToolHandle<IUnitTestTool1> tool1 ("asg::UnitTestTool1/" + name);
    EXPECT_FALSE (tool1.isUserConfigured());
    EXPECT_TRUE (tool1.isConfigurable());
    ASSERT_SUCCESS (tool1.setProperty<int> ("propertyInt", 17));
    ASSERT_SUCCESS (tool1.initialize());
  }



  template <typename TH>
  struct SetToolHandlePropertyTest : public ::testing::Test
  {
    void testSetToolHandle (const std::string& handleName,
			    bool isRegular, bool /*isPublic*/)
    {
      SCOPED_TRACE (handleName);

      TH par;
      AnaToolHandle<IUnitTestTool2> mainTool ("asg::UnitTestTool2/" + makeUniqueName());

      if (!par.isSettable())
      {
      	ASSERT_FAILURE (mainTool.setProperty (handleName, par.handle));
      	return;
      }
      ASSERT_SUCCESS (mainTool.setProperty (handleName, par.handle));

      // for AnaToolHandle members we try to get the tool in
      // initialize(), so if we can't get a tool from the tool handle,
      // we will fail
      if (!isRegular && !par.isGettable())
      {
	ASSERT_FAILURE (mainTool.initialize ());
	return;
      }
      ASSERT_SUCCESS (mainTool.initialize ());

      // a regular ToolHandle will throw when trying to get an empty
      // handle, an AnaToolHandle will return a nullptr
      if (par.handle.empty())
      {
	if (isRegular)
	  ASSERT_ANY_THROW (mainTool->getToolHandle (handleName));
	else
	  ASSERT_EQ (nullptr, mainTool->getToolHandle (handleName));
	return;
      }

      // for old-fashioned ToolHandle members we don't try to get the
      // tool until we access it, making it fail at that point.
      if (!par.isGettable())
      {
	ASSERT_ANY_THROW (mainTool->getToolHandle (handleName));
	return;
      }
      auto *const subtool = mainTool->getToolHandle (handleName);
#ifdef ROOTCORE
      EXPECT_EQ (par.getTool(), subtool);
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
      ANA_CHECK_THROW (handle.make ("asg::UnitTestTool1/" + makeUniqueName()));
      ANA_CHECK_THROW (handle.initialize ());
    }

    bool isSettable () const {return true;}
    bool isGettable () const {return true;}
    const IUnitTestTool1 *getTool () const {return &*handle;}

    AnaToolHandle<IUnitTestTool1> handle;
  };
#ifdef ROOTCORE
  INSTANTIATE_TYPED_TEST_CASE_P (PublicAnaSubToolTest, SetToolHandlePropertyTest, PublicAnaSubTool);
#endif

  struct PrivateAnaSubTool
  {
    PrivateAnaSubTool ()
      : parent (new UnitTestTool1 (makeUniqueName())),
	handle ("asg::UnitTestTool1/" + makeUniqueName(), parent.get())
    {
      ANA_CHECK_THROW (parent->initialize ());
      ANA_CHECK_THROW (handle.initialize ());
    }

    bool isSettable () const {return true;}
    bool isGettable () const {return true;}
    const IUnitTestTool1 *getTool () const {return &*handle;}

    std::unique_ptr<UnitTestTool1> parent;
    AnaToolHandle<IUnitTestTool1> handle;
  };
#ifdef ROOTCORE
  INSTANTIATE_TYPED_TEST_CASE_P (PrivateAnaSubToolTest, SetToolHandlePropertyTest, PrivateAnaSubTool);
#endif

  struct EmptyRegSubTool
  {
    EmptyRegSubTool ()
    {
    }

    bool isSettable () const {return true;}
    bool isGettable () const {return false;}
    IUnitTestTool1 *getTool () const {return nullptr;}

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
    const IUnitTestTool1 *getTool () const {return &*handle;}

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
    const IUnitTestTool1 *getTool () const {return &*handle;}

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
      ANA_CHECK_THROW (tool.make ("asg::UnitTestTool1/" + makeUniqueName()));
      ANA_CHECK_THROW (tool.initialize ());
      handle = tool.get ();
    }

    bool isSettable () const {return true;}
    bool isGettable () const {return true;}
    const IUnitTestTool1 *getTool () const {return &*handle;}

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
    std::string name = makeUniqueName();
    AnaToolHandle<IAsgTool> tool1("asg::UnitTestTool1/" + name);
    ASSERT_SUCCESS (tool1.initialize());

    asg::AnaToolHandle<asg::IAsgTool> tool3;

    {
      asg::AnaToolHandle<asg::IAsgTool> tool2("asg::UnitTestTool1/" + name);
      ASSERT_SUCCESS (tool2.initialize());
      tool3 = std::move(tool2);
    }
  }



  struct SubtoolTest : public testing::TestWithParam<std::tuple<std::string,std::string,std::string> >
  {
  };



  TEST_P (SubtoolTest, set)
  {
    std::vector<std::shared_ptr<void> > cleanup;
    int value = 7;

    std::string propertyName;
    if (std::get<1>(GetParam()) == "public")
      propertyName = "subtool0";
    else if (std::get<1>(GetParam()) == "private")
      propertyName = "subtool1";
    else
    {
      ADD_FAILURE () << "unknown parameter " << std::get<1>(GetParam());
      return;
    }

    AnaToolHandle<IUnitTestTool3> th3 ("asg::UnitTestTool3/" + makeUniqueName());
    ASSERT_SUCCESS (th3.setProperty ("propertyName", std::get<0>(GetParam ())));
    if (std::get<2>(GetParam()) == "empty")
    {
      value = -1;
      ToolHandle<asg::IUnitTestTool1> th ("");
      ASSERT_TRUE (th.empty());
      ASSERT_SUCCESS (th3.setProperty (propertyName, th));
    } else if (std::get<2>(GetParam()) == "none")
    {
      value = 42;
    } else if (std::get<2>(GetParam()) == "ATH" ||
               std::get<2>(GetParam()) == "TH" ||
               std::get<2>(GetParam()) == "NOINIT")
    {
      std::shared_ptr<AnaToolHandle<IUnitTestTool1> > th1;
      if (std::get<1>(GetParam()) == "public")
      {
        th1 =
          std::make_shared<AnaToolHandle<IUnitTestTool1> >
          ("asg::UnitTestTool1/" + makeUniqueName());
        cleanup.push_back (th1);
      } else
      {
        auto th0 =
          std::make_shared<AnaToolHandle<IUnitTestTool1> >
          ("asg::UnitTestTool1/" + makeUniqueName());
        cleanup.push_back (th0);
        ASSERT_SUCCESS (th0->initialize ());
        th1 =
          std::make_shared<AnaToolHandle<IUnitTestTool1> >
          ("asg::UnitTestTool1/" + makeUniqueName(),
           dynamic_cast<AsgTool*>(th0->get()));
        cleanup.push_back (th1);
      }
      ASSERT_SUCCESS (th1->setProperty ("propertyInt", value));
      if (std::get<2>(GetParam()) != "NOINIT")
      {
#ifndef ROOTCORE
        ASSERT_EQ (std::get<1>(GetParam()) == "public", th1->getHandle().isPublic());
#endif
        ASSERT_SUCCESS (th1->initialize ());
#ifndef ROOTCORE
        ASSERT_EQ (std::get<1>(GetParam()) == "public", th1->getHandle().isPublic());
#endif
      }

      if (std::get<2>(GetParam()) == "ATH")
      {
        ASSERT_SUCCESS (th3.setProperty (propertyName, *th1));
      } else if (std::get<2>(GetParam()) == "TH")
      {
        ASSERT_SUCCESS (th3.setProperty (propertyName, th1->getHandle()));
      } else if (std::get<2>(GetParam()) == "NOINIT")
      {
        ASSERT_SUCCESS (th3.setProperty (propertyName, *th1));
      } else
      {
        ADD_FAILURE () << "unknown parameter " << std::get<2>(GetParam());
        return;
      }
    } else
    {
      ADD_FAILURE () << "unknown parameter " << std::get<1>(GetParam());
      return;
    }
    if (std::get<1>(GetParam()) == "public")
      ASSERT_SUCCESS (th3.setProperty ("usePublic", true));

    ASSERT_SUCCESS (th3.initialize ());
    if (value == -1)
    {
      ASSERT_TRUE (th3->subsubtoolEmpty());
    } else
    {
      ASSERT_FALSE (th3->subsubtoolEmpty());
      ASSERT_EQ (value, th3->getSubsubtool()->getPropertyInt());
    }
  }

  INSTANTIATE_TEST_CASE_P
  (MySubtoolTest1, SubtoolTest, ::testing::Values
   (std::make_tuple ("regPublicHandle",  "public",  "ATH"),
    std::make_tuple ("anaPublicHandle",  "public",  "ATH"),
    std::make_tuple ("regPublicHandle",  "public",  "TH"),
    std::make_tuple ("anaPublicHandle",  "public",  "TH"),
    std::make_tuple ("regPublicHandle",  "public",  "NOINIT"),
    std::make_tuple ("anaPublicHandle",  "public",  "NOINIT"),
    std::make_tuple ("regPublicHandle",  "public",  "empty"),
    std::make_tuple ("anaPublicHandle",  "public",  "empty"),
    std::make_tuple ("regPrivateHandle", "private", "ATH"),
    std::make_tuple ("anaPrivateHandle", "private", "ATH"),
    std::make_tuple ("regPrivateHandle", "private", "TH"),
    std::make_tuple ("anaPrivateHandle", "private", "TH"),
    std::make_tuple ("regPrivateHandle", "private", "NOINIT"),
    std::make_tuple ("anaPrivateHandle", "private", "NOINIT"),
    std::make_tuple ("regPrivateHandle", "private", "empty"),
    std::make_tuple ("anaPrivateHandle", "private", "empty"),
    std::make_tuple ("regPrivateHandle", "private", "none"),
    std::make_tuple ("anaPrivateHandle", "private", "none")),);
}

ATLAS_GOOGLE_TEST_MAIN
