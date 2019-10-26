#ifndef AthenaMonitoringKernel_test_mocks_MockMonitoredVariable_h
#define AthenaMonitoringKernel_test_mocks_MockMonitoredVariable_h

#include <functional>

#include "AthenaMonitoringKernel/IMonitoredVariable.h"

class MockMonitoredVariable : public Monitored::IMonitoredVariable {
  public:
    MockMonitoredVariable(const std::string& name) : IMonitoredVariable(name) {}

    std::function<std::vector<double>()> mock_getVectorRepresentation;
    const std::vector<double> getVectorRepresentation() const override {
      return mock_getVectorRepresentation ? mock_getVectorRepresentation() : std::vector<double>();
    }
    bool hasStringRepresentation() const override {  return false; }
    std::vector<std::string> getStringVectorRepresentation() const override { return {}; } 
};

#endif /* AthenaMonitoringKernel_test_mocks_MockMonitoredVariable_h */
