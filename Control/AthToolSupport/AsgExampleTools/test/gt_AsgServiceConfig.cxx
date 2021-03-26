/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <AsgServices/AsgServiceConfig.h>
#include <AsgMessaging/MessageCheck.h>
#include <AsgTesting/UnitTest.h>
#include <AsgExampleTools/UnitTestService1.h>
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
    /// \brief make a unique service name to be used in unit tests
    std::string makeUniqueName ()
    {
      static unsigned index = 0;
      std::ostringstream str;
      str << "unique" << ++ index;
      return str.str();
    }
  }



  TEST (AsgServiceConfigTest, basic)
  {
    const std::string name = makeUniqueName();
    AsgServiceConfig config ("asg::UnitTestService1/" + name);
    std::shared_ptr<IUnitTestService1> service;
    ASSERT_SUCCESS (config.makeService (service));
    EXPECT_EQ (name, service->name());
    EXPECT_TRUE (service->isInitialized());
    EXPECT_EQ (-7, service->getPropertyInt());
    EXPECT_EQ ("", service->getPropertyString());
  }



  TEST (AsgServiceConfigTest, basic_properties)
  {
    const std::string name = makeUniqueName();
    AsgServiceConfig config ("asg::UnitTestService1/" + name);
    EXPECT_SUCCESS (config.setProperty ("propertyInt", 17));
    EXPECT_SUCCESS (config.setProperty ("propertyString", "alpha"));
    std::shared_ptr<IUnitTestService1> service;
    ASSERT_SUCCESS (config.makeService (service));
    EXPECT_EQ (name, service->name());
    EXPECT_TRUE (service->isInitialized());
    EXPECT_EQ (17, service->getPropertyInt());
    EXPECT_EQ ("alpha", service->getPropertyString());
  }



  TEST (AsgServiceConfigTest, basic_propertyFromString)
  {
    const std::string name = makeUniqueName();
    AsgServiceConfig config ("asg::UnitTestService1/" + name);
    config.setPropertyFromString ("propertyInt", "17");
    config.setPropertyFromString ("propertyString", "'alpha'");
    std::shared_ptr<IUnitTestService1> service;
    ASSERT_SUCCESS (config.makeService (service));
    EXPECT_EQ (name, service->name());
    EXPECT_TRUE (service->isInitialized());
    EXPECT_EQ (17, service->getPropertyInt());
    EXPECT_EQ ("alpha", service->getPropertyString());
  }



#ifdef XAOD_STANDALONE
  TEST (AsgServiceConfigTest, basic_delete)
  {
    const std::string name = makeUniqueName();
    {
      AsgServiceConfig config ("asg::UnitTestService1/" + name);
      std::shared_ptr<IUnitTestService1> service;
      ASSERT_SUCCESS (config.makeService (service));
      ASSERT_EQ (service, ServiceStore::get (name, true));
    }
    ASSERT_EQ (nullptr, ServiceStore::get (name, true));
  }
#endif



  TEST (AsgServiceConfigTest, serviceHandle)
  {
    const std::string name = makeUniqueName();
    AsgServiceConfig config1 ("asg::UnitTestService1/" + name);
    ServiceHandle<IUnitTestService1> service1 ("", "AsgServiceConfigTest");
    ASSERT_SUCCESS (config1.makeService (service1));
    EXPECT_EQ (name, service1->name());

    ServiceHandle<IUnitTestService1> service2 ("asg::UnitTestService1/" + name, "UnitTest");
    ASSERT_SUCCESS (service2.retrieve ());
    EXPECT_EQ (name, service2->name());
    EXPECT_EQ (&*service1, &*service2);
  }
}

ATLAS_GOOGLE_TEST_MAIN
