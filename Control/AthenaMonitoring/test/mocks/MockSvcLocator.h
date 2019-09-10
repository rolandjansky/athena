#ifndef AthenaMonitoring_test_mocks_MockSvcLocator_h
#define AthenaMonitoring_test_mocks_MockSvcLocator_h

#include "GaudiKernel/ISvcLocator.h"

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

#endif /* AthenaMonitoring_test_mocks_MockSvcLocator_h */