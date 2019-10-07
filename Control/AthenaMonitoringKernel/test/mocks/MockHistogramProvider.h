#ifndef AthenaMonitoringKernel_test_mocks_MockHistogramProvider_h
#define AthenaMonitoringKernel_test_mocks_MockHistogramProvider_h

#include <functional>

#include "AthenaMonitoringKernel/IHistogramProvider.h"

class MockHistogramProvider : public Monitored::IHistogramProvider {
  public:
    std::function<TNamed*()> mock_histogram;
    TNamed* histogram() override {
      return mock_histogram ? mock_histogram() : nullptr;
    }
};

#endif /* AthenaMonitoringKernel_test_mocks_MockHistogramProvider_h */