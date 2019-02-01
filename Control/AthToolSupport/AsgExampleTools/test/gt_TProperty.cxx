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
#include <cmath>
#include <gtest/gtest.h>
#include <gtest/gtest-spi.h>

#ifdef ROOTCORE
#include <xAODRootAccess/Init.h>
#endif

#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

//
// method implementations
//

using namespace asg::msgUserCode;

namespace asg
{
  /// \brief a test structure that has no streamer or string
  /// conversions
  struct UserStruct
  {};

  namespace detail
  {
    TEST (TypeTraitTest, hasOStreamer_true)
    {
      ASSERT_TRUE (HasOStreamer<int>::value);
    }

    TEST (TypeTraitTest, hasOStreamer_false)
    {
      ASSERT_FALSE (HasOStreamer<UserStruct>::value);
    }

    TEST (TypeTraitTest, hasOStringConvert_true)
    {
      ASSERT_TRUE (HasOStringConvert<std::string>::value);
    }

    TEST (TypeTraitTest, hasOStringConvert_false)
    {
      ASSERT_FALSE (HasOStringConvert<int>::value);
    }

    TEST (StringVectorTest, unpackTest_blank)
    {
      std::vector<std::string> result;
      ASSERT_FAILURE_REGEX (unpackStringVector (" ", result), "did not find a vector in the string");
    }

    TEST (StringVectorTest, unpackTest_empty)
    {
      std::vector<std::string> result;
      ASSERT_SUCCESS (unpackStringVector (" [ ] ", result));
      std::vector<std::string> expected ({});
      ASSERT_EQ (expected, result);
    }

    TEST (StringVectorTest, unpackTest_single)
    {
      std::vector<std::string> result;
      ASSERT_SUCCESS (unpackStringVector (" [ 0 ] ", result));
      std::vector<std::string> expected ({"0"});
      ASSERT_EQ (expected, result);
    }

    TEST (StringVectorTest, unpackTest_pair)
    {
      std::vector<std::string> result;
      ASSERT_SUCCESS (unpackStringVector (" [ 0, 1 ] ", result));
      std::vector<std::string> expected ({"0", "1"});
      ASSERT_EQ (expected, result);
    }

    TEST (StringVectorTest, unpackTest_empty_comma)
    {
      std::vector<std::string> result;
      ASSERT_FAILURE_REGEX (unpackStringVector (" [ , 1] ", result), "comma not preceded by value");
    }

    TEST (StringVectorTest, unpackTest_empty_end)
    {
      std::vector<std::string> result;
      ASSERT_FAILURE_REGEX (unpackStringVector (" [ 0 , ] ", result), "vector property has ',' before ']'");
    }

    TEST (StringVectorTest, unpackTest_nested)
    {
      std::vector<std::string> result;
      ASSERT_SUCCESS (unpackStringVector (" [ [ 0 , 1 ] ] ", result));
      std::vector<std::string> expected ({"[ 0 , 1 ]"});
      ASSERT_EQ (expected, result);
    }

    TEST (GetStringHelperTest, get_int)
    {
      std::string str;
      ASSERT_SUCCESS (GetStringHelper<int>::get (0, str));
      ASSERT_EQ ("0", str);
    }

    TEST (GetStringHelperTest, get_enum)
    {
      std::string str;
      ASSERT_SUCCESS (GetStringHelper<MSG::Level>::get (MSG::Level::WARNING, str));
      ASSERT_EQ ("4", str);
    }

    TEST (GetStringHelperTest, get_string_regular)
    {
      std::string str;
      ASSERT_SUCCESS (GetStringHelper<std::string>::get ("hello world", str));
      ASSERT_EQ ("\'hello world\'", str);
    }

    TEST (GetStringHelperTest, get_string_single_quote)
    {
      std::string str;
      ASSERT_SUCCESS (GetStringHelper<std::string>::get ("hello ' world", str));
      ASSERT_EQ ("\"hello ' world\"", str);
    }

    TEST (GetStringHelperTest, get_string_double_quote)
    {
      std::string str;
      ASSERT_SUCCESS (GetStringHelper<std::string>::get ("hello \" world", str));
      ASSERT_EQ ("\'hello \" world\'", str);
    }

    TEST (GetStringHelperTest, get_string_single_quote_backslash)
    {
      std::string str;
      ASSERT_SUCCESS (GetStringHelper<std::string>::get ("hello ' \\ world", str));
      ASSERT_EQ ("\"hello ' \\\\ world\"", str);
    }

    TEST (GetStringHelperTest, get_string_double_quote_backslash)
    {
      std::string str;
      ASSERT_SUCCESS (GetStringHelper<std::string>::get ("hello \" \\ world", str));
      ASSERT_EQ ("\'hello \" \\\\ world\'", str);
    }

    TEST (GetStringHelperTest, get_string_mixed_quote)
    {
      std::string str;
      ASSERT_SUCCESS (GetStringHelper<std::string>::get ("hello ' \" world", str));
      ASSERT_EQ ("\'hello \\' \" world\'", str);
    }

    TEST (GetStringHelperTest, get_vector_int)
    {
      std::string str;
      ASSERT_SUCCESS (GetStringHelper<std::vector<int> >::get ({0,1,2}, str));
      ASSERT_EQ ("[0,1,2]", str);
    }

    TEST (GetStringHelperTest, get_fail)
    {
      std::string str;
      ASSERT_FAILURE_REGEX (GetStringHelper<UserStruct>::get (UserStruct (), str), "GetStringHelper: not implemented for");
    }

    TEST (GetStringHelperTest, get_toolhandle)
    {
      std::string str;
      ASSERT_SUCCESS (GetStringHelper<ToolHandle<AsgTool> >
                      ::get (ToolHandle<AsgTool> ("alpha/beta"), str));
      ASSERT_EQ ("'alpha/beta'", str);
    }

    TEST (GetStringHelperTest, get_toolhandle_array)
    {
      std::string str;
      ASSERT_SUCCESS (GetStringHelper<ToolHandleArray<AsgTool> >
                      ::get ({ToolHandle<AsgTool> ("alpha/beta")}, str));
      ASSERT_EQ ("['alpha/beta']", str);
    }



    TEST (TypeTraitTest, hasIStreamer_true)
    {
      ASSERT_TRUE (HasIStreamer<int>::value);
    }

    TEST (TypeTraitTest, hasIStreamer_false)
    {
      ASSERT_FALSE (HasIStreamer<UserStruct>::value);
    }

    TEST (TypeTraitTest, hasIStringConvert_true)
    {
      ASSERT_TRUE (HasIStringConvert<std::string>::value);
    }

    TEST (TypeTraitTest, hasIStringConvert_false)
    {
      ASSERT_FALSE (HasIStringConvert<int>::value);
    }

    TEST (StringVectorTest, packTest)
    {
      std::string result;
      ASSERT_SUCCESS (packStringVector ({"", "\"", "\\", "a"}, result));
      ASSERT_EQ ("[\"\",\\\",\\\\,a]", result);
    }

    TEST (SetStringHelperTest, set_int)
    {
      int value = 7;
      ASSERT_SUCCESS (SetStringHelper<int>::set ("0", value));
      ASSERT_EQ (0, value);
    }

    TEST (SetStringHelperTest, set_enum)
    {
      MSG::Level value = MSG::Level::INFO;
      ASSERT_SUCCESS (SetStringHelper<MSG::Level>::set ("2", value));
      ASSERT_EQ (MSG::Level::DEBUG, value);
    }

    TEST (SetStringHelperTest, set_string_raw)
    {
      std::string value;
      ASSERT_SUCCESS (SetStringHelper<std::string>::set ("hello world", value));
      ASSERT_EQ ("hello world", value);
    }

    TEST (SetStringHelperTest, set_string_regular)
    {
      std::string str;
      ASSERT_SUCCESS (SetStringHelper<std::string>::set ("\'hello world\'", str));
      ASSERT_EQ ("hello world", str);
    }

    TEST (SetStringHelperTest, set_string_single_quote)
    {
      std::string str;
      ASSERT_SUCCESS (SetStringHelper<std::string>::set ("\"hello ' world\"", str));
      ASSERT_EQ ("hello ' world", str);
    }

    TEST (SetStringHelperTest, set_string_double_quote)
    {
      std::string str;
      ASSERT_SUCCESS (SetStringHelper<std::string>::set ("\'hello \" world\'", str));
      ASSERT_EQ ("hello \" world", str);
    }

    TEST (SetStringHelperTest, set_string_single_quote_backslash)
    {
      std::string str;
      ASSERT_SUCCESS (SetStringHelper<std::string>::set ("\"hello ' \\\\ world\"", str));
      ASSERT_EQ ("hello ' \\ world", str);
    }

    TEST (SetStringHelperTest, set_string_double_quote_backslash)
    {
      std::string str;
      ASSERT_SUCCESS (SetStringHelper<std::string>::set ("\'hello \" \\\\ world\'", str));
      ASSERT_EQ ("hello \" \\ world", str);
    }

    TEST (SetStringHelperTest, set_string_mixed_quote)
    {
      std::string str;
      ASSERT_SUCCESS (SetStringHelper<std::string>::set ("\'hello \\' \" world\'", str));
      ASSERT_EQ ("hello ' \" world", str);
    }

    TEST (SetStringHelperTest, set_vector_int)
    {
      std::vector<int> value;
      ASSERT_SUCCESS (SetStringHelper<std::vector<int> >::set ("[0,1,2]", value));
      std::vector<int> expected {0, 1, 2};
      ASSERT_EQ (expected, value);
    }

    TEST (SetStringHelperTest, set_fail)
    {
      UserStruct value;
      ASSERT_FAILURE_REGEX (SetStringHelper<UserStruct>::set ("", value), "SetStringHelper: not implemented for ");
    }

    TEST (SetStringHelperTest, set_toolhandle)
    {
      ToolHandle<AsgTool> value;
      ASSERT_SUCCESS (SetStringHelper<ToolHandle<AsgTool> >
                      ::set ("'alpha/beta'", value));
      ASSERT_EQ ("alpha/beta", value.typeAndName());
    }

    TEST (SetStringHelperTest, set_toolhandle_array)
    {
      ToolHandleArray<AsgTool> value;
      ASSERT_SUCCESS (SetStringHelper<ToolHandleArray<AsgTool> >
                      ::set ("['alpha/beta']", value));
      ASSERT_EQ (1u, value.size());
      ASSERT_EQ ("alpha/beta", value.at(0).typeAndName());
    }
  }

  TEST (TPropertyTest, getString)
  {
    int value = 0;
    std::unique_ptr<Property> property (createProperty (value));

    std::string strValue;
    ASSERT_SUCCESS (property->getString (strValue));
    ASSERT_EQ ("0", strValue);
  }


  TEST (TPropertyTest, setString)
  {
    int value = 0;
    std::unique_ptr<Property> property (createProperty (value));

    ASSERT_SUCCESS (property->setString ("7"));
    ASSERT_EQ (7, value);
  }

  TEST (TPropertyTest, setStringProperty)
  {
    int value = 0;
    std::unique_ptr<Property> property (createProperty (value));

    std::string value2 = "7";
    std::unique_ptr<Property> property2 (createProperty (value2));

    ASSERT_EQ (0, property->setFrom (*property2));
    ASSERT_EQ (7, value);
  }
}

ATLAS_GOOGLE_TEST_MAIN
