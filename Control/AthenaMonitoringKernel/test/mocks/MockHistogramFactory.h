
#ifndef AthenaMonitoringKernel_test_mocks_MockHistogramFactory_h
#define AthenaMonitoringKernel_test_mocks_MockHistogramFactory_h

#include <functional>

#include "../../src/HistogramFiller/HistogramFactory.h"

class MockHistogramFactory : public Monitored::HistogramFactory {
  public:
    MockHistogramFactory()
      : Monitored::HistogramFactory(ServiceHandle<ITHistSvc>("", "") ,"") {}

    std::function<TNamed*(const Monitored::HistogramDef& def)> mock_create;
    TNamed* create(const Monitored::HistogramDef& def) override {
      return mock_create ? mock_create(def) : nullptr;
    }
    std::function<TNamed*(const Monitored::HistogramDef& def)> mock_remove;
    void remove( const Monitored::HistogramDef& def ) override {
      if ( mock_remove )  mock_remove(def);
    }
};

#endif /* AthenaMonitoringKernel_test_mocks_MockHistogramFactory_h */
