#include <functional>

#include "GaudiKernel/ISvcLocator.h"
#include "AthenaBaseComps/AthAlgorithm.h"

#include "AthenaMonitoring/GenericMonitoringTool.h"

class MockGenericMonitoringTool : public GenericMonitoringTool {
  private:
    class MockSvcLocator : public ISvcLocator {
      public:
        unsigned long addRef() override { return 0; }
        unsigned long release() override { return 0; }
        StatusCode queryInterface(const InterfaceID&, void**) override { return StatusCode::SUCCESS; }
        const std::list<IService*>& getServices() const override { return m_services; }
        bool existsService(const std::string&) const override { return true; }
        SmartIF<IService>& service( const Gaudi::Utils::TypeNameString&, const bool) override { return m_service; }
      private:
        std::list<IService*> m_services;
        SmartIF<IService> m_service;
    };

    class MockAlgorithm : public AthAlgorithm {
      public:
        MockAlgorithm() : AthAlgorithm("MockAlgorithm", new MockSvcLocator(), "0.0.1") {}
        StatusCode execute() override { return StatusCode::SUCCESS; }
    };
  public:
    MockGenericMonitoringTool()
      : GenericMonitoringTool("MockGenericMonitoringTool", "ToolSvc.MonTool", new MockAlgorithm()) {}

    std::function<uint32_t()> mock_lumiBlock;
    uint32_t lumiBlock() override {
      return mock_lumiBlock ? mock_lumiBlock() : 0;
    }
};