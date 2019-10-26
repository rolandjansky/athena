/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/  
  
#ifndef AthenaMonitoringKernel_HistogramFiller_HistogramFillerFactory_h
#define AthenaMonitoringKernel_HistogramFiller_HistogramFillerFactory_h

#include <memory>
#include <vector>

#include "AthenaMonitoringKernel/GenericMonitoringTool.h"
#include "AthenaMonitoringKernel/HistogramDef.h"
#include "AthenaMonitoringKernel/HistogramFiller.h"
#include "AthenaMonitoringKernel/IHistogramProvider.h"

#include "HistogramFactory.h"

namespace Monitored {
  /**
   * @brief Factory of the histogram fillers 
   */
  class HistogramFillerFactory {
  public:
    /**
     * @brief Default constructor
     * 
     * @param gmTool An instance of GenericMonitoringTool
     * @param groupName Name of the group to which produced histograms will belong
     */
    HistogramFillerFactory(GenericMonitoringTool * const gmTool, std::string groupName)
      : m_gmTool(std::move(gmTool)), 
        m_factory(new HistogramFactory(gmTool->histogramService(), std::move(groupName))) {}
      
    /**
     * @brief Virtual destructor
     */
    virtual ~HistogramFillerFactory() {}

    /**
     * @brief Creates HistogramFiller instance for given definition
     * 
     * Based on the histogram type and opts, method will return proper kind of the HistogramFiller
     * 
     * @param def Histogram definition 
     * @return HistogramFiller handler
     */
    HistogramFiller* create(const HistogramDef& def);
  private:
    std::shared_ptr<IHistogramProvider> createHistogramProvider(const HistogramDef& def);

    GenericMonitoringTool *m_gmTool;
    std::shared_ptr<HistogramFactory> m_factory;
  };
}

#endif /* AthenaMonitoringKernel_HistogramFiller_HistogramFillerFactory_h */