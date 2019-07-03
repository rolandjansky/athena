#ifndef AthenaMonitoring_test_mocks_MockGenericMonitoringTool_h
#define AthenaMonitoring_test_mocks_MockGenericMonitoringTool_h

#include <functional>

#include "AthenaMonitoring/GenericMonitoringTool.h"

#include "MockAlgorithm.h"
#include "MockTHistServiceHandle.h"

class MockGenericMonitoringTool : public GenericMonitoringTool {
  public:
    MockGenericMonitoringTool()
      : GenericMonitoringTool("MockGenericMonitoringTool", "ToolSvc.MonTool", new MockAlgorithm()) {}

    std::function<uint32_t()> mock_lumiBlock;
    uint32_t lumiBlock() override {
      return mock_lumiBlock ? mock_lumiBlock() : 0;
    }

    std::function<uint32_t()> mock_runNumber;
    uint32_t runNumber() override {
      return mock_runNumber ? mock_runNumber() : 0;
    }

    const ServiceHandle<ITHistSvc>& histogramService() override {
      m_serviceHandle.retrieve();
      
      return m_serviceHandle;
    }

    MockITHistSvc& histSvc() {
      return m_serviceHandle.histSvc();
    }
  private:
    MockTHistServiceHandle m_serviceHandle;
};

#endif /* AthenaMonitoring_test_mocks_MockGenericMonitoringTool_h */