/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#undef NDEBUG
#include <cassert>
#include <iostream>
#include <list>
#include <functional>
#include <memory>

#include "TestTools/expect.h"
#include "GaudiKernel/MsgStream.h"
#include "AthenaKernel/getMessageSvc.h"

#include "AthenaMonitoringKernel/MonitoredScalar.h"

using namespace std;
using namespace Monitored;

#define REGISTER_TEST_CASE(TEST_CASE_NAME) registerTestCase(&MonitoredScalarTestSuite::TEST_CASE_NAME, #TEST_CASE_NAME)

class MonitoredScalarTestSuite {
  // ==================== All registered test cases ====================
  private:
    list<function<void(void)>> registeredTestCases() {
      return {
        REGISTER_TEST_CASE(test_shouldDeduceValueTypeAndReturnOriginalValue),
        REGISTER_TEST_CASE(test_shouldAcceptExplicitValueTypeAndReturnOriginalValue),
        REGISTER_TEST_CASE(test_shouldAcceptCustomTypeAndReturnOriginalValue),
        REGISTER_TEST_CASE(test_shouldAllowToProvideRepresentationConverter),
        REGISTER_TEST_CASE(test_shouldAllowToChangeUnderlayingValue),
        REGISTER_TEST_CASE(test_shouldReturnSingleElement),
      };
    }

  // ==================== Test code ====================
  private:
    void beforeEach() { }

    void afterEach() { }

    void test_shouldDeduceValueTypeAndReturnOriginalValue() { 
      auto phi = Monitored::Scalar("phi", 4.123456789012345); 

      assert(phi == 4.123456789012345);
    }

    void test_shouldAcceptExplicitValueTypeAndReturnOriginalValue() { 
      auto phi = Monitored::Scalar<float>("phi", 4.1234567f); 

      assert(phi == 4.1234567f);
    }

    void test_shouldAcceptCustomTypeAndReturnOriginalValue() { 
      auto phi = Monitored::Scalar("phi", TestValue(4.123456789012345)); 

      assert(TestValue(4.123456789012345) == phi);
    }

    void test_shouldAllowToProvideRepresentationConverter() { 
      auto phi = Monitored::Scalar<double>("phi", 4.123456789012345, [](double value) { return value * 1000; }); 

      assert(phi == 4.123456789012345);
      assert(phi.get(0) == 4123.456789012345);
    }

    void test_shouldAllowToChangeUnderlayingValue() { 
      auto phi = Monitored::Scalar("phi", 4.2); 

      phi = 5.5;

      assert(phi == 5.5);
    }

    void test_shouldReturnSingleElement() {
      auto phi = Monitored::Scalar("phi", 4.2);

      assert(phi.size() == 1);
      assert(phi.get(0) == 4.2);
    }

  // ==================== Helper methods ====================
  private:
    class TestValue {
    public:
      TestValue(double value) : m_value(value) {}
      operator double() const { return m_value; }
      bool operator==(const TestValue &other) const { return m_value == other.m_value; }
    private:
      double m_value;
    };

  // ==================== Initialization & run ====================
  public:
    MonitoredScalarTestSuite() 
      : m_log(Athena::getMessageSvc(), "MonitoredScalarTestSuite") { }

    void run() {
      for (function<void(void)> testCase : registeredTestCases()) {
        testCase();
      }
    }

  // ==================== Test case registration ====================
  private:
    typedef void (MonitoredScalarTestSuite::*TestCase)(void);

    function<void(void)> registerTestCase(TestCase testCase, const string& testCaseName) {
      return [this, testCase, testCaseName]() {
        m_log << MSG::INFO << "Current test case: " << testCaseName << endmsg;
        beforeEach();
        invoke(testCase, this);
        afterEach();
      };
    }

  // ==================== Properties ====================
  private:
    MsgStream m_log;
};

int main() {
  MonitoredScalarTestSuite().run();

  return 0;
}
