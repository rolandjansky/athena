/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//          
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (krumnack@iastate.edu) for bug
// reports, feature suggestions, praise and complaints.


//
// includes
//

#include <SampleHandler/Global.h>

#include <RootCoreUtils/Assert.h>
#include <SampleHandler/MetaObject.h>
#include <iostream>

//
// main program
//

using namespace SH;

#define CHECK_FAIL(X)							\
  {									\
    bool failXXX = true;						\
    try	{								\
      X; if(failXXX) failXXX = false;						\
    } catch (...) {}							\
    RCU_ASSERT2 (failXXX, (std::string ("command didn't fail: ") + #X).c_str()); \
  }



int main ()
{
  MetaObject meta;
  const bool valBool = true;
  const std::string strBool = "true";
  const std::string strInteger = "42";
  const int valInteger = 42;
  const std::string strDouble = "42.25";
  const double valDouble = 42.25;
  const std::string valString = "42 alpha";


  try
  {
    meta.setBool ("bool", valBool);
    meta.setInteger ("integer", valInteger);
    meta.setDouble ("double", valDouble);
    meta.setString ("string", valString);

    RCU_ASSERT_SOFT (meta.castBool    ("bool", !valBool)  == valBool);
    RCU_ASSERT_SOFT (meta.castInteger ("bool", 666)       == valBool);
    RCU_ASSERT_SOFT (meta.castDouble  ("bool", 666)       == valBool);
    RCU_ASSERT_SOFT (meta.castString  ("bool", "default") == strBool);

    RCU_ASSERT_SOFT (meta.castBool    ("integer", false)     == true);
    RCU_ASSERT_SOFT (meta.castInteger ("integer", 666)       == valInteger);
    RCU_ASSERT_SOFT (meta.castDouble  ("integer", 666)       == valInteger);
    RCU_ASSERT_SOFT (meta.castString  ("integer", "default") == strInteger);

    RCU_ASSERT_SOFT (meta.castBool    ("double", false)     == true);
    RCU_ASSERT_SOFT (meta.castInteger ("double", 666)       == int (valDouble));
    RCU_ASSERT_SOFT (meta.castDouble  ("double", 666)       == valDouble);
    RCU_ASSERT_SOFT (meta.castString  ("double", "default") == strDouble);
    
  } catch (std::exception& e)
    {
      std::cout << "caught exception: " << e.what() << std::endl;
      return 1;
    }
  

  CHECK_FAIL (meta.castBool    ("string", false));
  CHECK_FAIL (meta.castInteger ("string", 666));
  CHECK_FAIL (meta.castDouble  ("string", 666));
  RCU_ASSERT (meta.castString  ("string", "default") == "42 alpha");

  return 0;
}
