#ifndef AthenaMonitoringKernel_test_mocks_MockGenericMonitoringTool_h
#define AthenaMonitoringKernel_test_mocks_MockGenericMonitoringTool_h

#include <functional>

#include "AthenaMonitoringKernel/GenericMonitoringTool.h"

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

    const ServiceHandle<ITHistSvc>& histogramService() const override {
      m_serviceHandle.retrieve().ignore();
      
      return m_serviceHandle;
    }

    MockITHistSvc& histSvc() {
      return m_serviceHandle.histSvc();
    }
  private:
    MockTHistServiceHandle m_serviceHandle;
};

#endif /* AthenaMonitoringKernel_test_mocks_MockGenericMonitoringTool_h */
