#ifndef AthenaMonitoringKernel_test_mocks_MockTHistServiceHandle_h
#define AthenaMonitoringKernel_test_mocks_MockTHistServiceHandle_h

#include "GaudiKernel/ServiceHandle.h"

#include "MockITHistSvc.h"

class MockTHistServiceHandle : public ServiceHandle<ITHistSvc> {
  public:
    MockTHistServiceHandle() 
    : ServiceHandle<ITHistSvc>("THistSvc/THistSvc", "MockTHistServiceHandle"),
      m_histSvc(new MockITHistSvc()) {}

    ~MockTHistServiceHandle() {
      delete m_histSvc;
    }
    
    StatusCode retrieve() const { 
      return ServiceHandle<ITHistSvc>::retrieve();
    }

    MockITHistSvc& histSvc() {
      return *m_histSvc;
    }
  protected:
    StatusCode retrieve(ITHistSvc*& service) const override {
      service = m_histSvc;
      return StatusCode::SUCCESS;
    }
  private:
    MockITHistSvc* m_histSvc;
};

#endif /* AthenaMonitoringKernel_test_mocks_MockTHistServiceHandle_h */