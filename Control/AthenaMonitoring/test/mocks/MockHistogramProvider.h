#ifndef AthenaMonitoring_test_mocks_MockHistogramProvider_h
#define AthenaMonitoring_test_mocks_MockHistogramProvider_h

#include <functional>

#include "AthenaMonitoring/IHistogramProvider.h"

class MockHistogramProvider : public Monitored::IHistogramProvider {
  public:
    std::function<TNamed*()> mock_histogram;
    TNamed* histogram() override {
      return mock_histogram ? mock_histogram() : nullptr;
    }
};

#endif /* AthenaMonitoring_test_mocks_MockHistogramProvider_h */