/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/  
  
#ifndef AthenaMonitoring_HistogramFiller_HistogramFillerFactory_h
#define AthenaMonitoring_HistogramFiller_HistogramFillerFactory_h

#include <memory>
#include <vector>

#include "AthenaMonitoring/GenericMonitoringTool.h"
#include "AthenaMonitoring/HistogramDef.h"

#include "AthenaMonitoring/HistogramFiller/HistogramFiller.h"
#include "AthenaMonitoring/HistogramFiller/HistogramFactory.h"

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
    HistogramFiller* createStaticFiller(const HistogramDef& def);
    HistogramFiller* createDynamicFiller(const HistogramDef& def);

    GenericMonitoringTool *m_gmTool;
    std::shared_ptr<HistogramFactory> m_factory;
  };
}

#endif /* AthenaMonitoring_HistogramFiller_HistogramFillerFactory_h */