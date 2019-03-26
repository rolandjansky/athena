/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/  
  
#ifndef AthenaMonitoring_HistogramFiller_HistogramFillerFactory_h
#define AthenaMonitoring_HistogramFiller_HistogramFillerFactory_h

#include <memory>
#include <vector>

#include "AthenaMonitoring/HistogramDef.h"

#include "AthenaMonitoring/HistogramFiller/HistogramFiller.h"
#include "AthenaMonitoring/HistogramFiller/HistogramFactory.h"

namespace Monitored {
  class HistogramFillerFactory {
  public:
    HistogramFillerFactory(const ServiceHandle<ITHistSvc>& histSvc, std::string groupName)
      : m_factory(new HistogramFactory(histSvc, std::move(groupName))) {}
      
    virtual ~HistogramFillerFactory() {}

    HistogramFiller* create(const HistogramDef& def); //!< creates filler
  private:
    std::shared_ptr<HistogramFactory> m_factory;
  };
}

#endif /* AthenaMonitoring_HistogramFiller_HistogramFillerFactory_h */