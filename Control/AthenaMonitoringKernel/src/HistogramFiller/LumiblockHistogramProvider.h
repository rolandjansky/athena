/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/  
  
#ifndef AthenaMonitoringKernel_HistogramFiller_LumiblockHistogramProvider_h
#define AthenaMonitoringKernel_HistogramFiller_LumiblockHistogramProvider_h

#include <memory>

#include "AthenaMonitoringKernel/GenericMonitoringTool.h"
#include "AthenaMonitoringKernel/HistogramDef.h"
#include "AthenaMonitoringKernel/IHistogramProvider.h"

#include "HistogramFactory.h"

namespace Monitored {
  /**
   * @brief Implementation of IHistogramProvider for lumi block based histograms
   * 
   * This provider produces histograms that groups data based on current lumi block and defined histogry depth. 
   * Note: kLBNHistoryDepth must be defined in histogram definition options
   */
  class LumiblockHistogramProvider : public IHistogramProvider {
  public:
    /**
     * @brief Constructor
     * 
     * @param gmTool Source of the lumi block info
     * @param factory ROOT object factory 
     * @param def General definition of a histogram
     */
    LumiblockHistogramProvider(GenericMonitoringTool* const gmTool, 
        std::shared_ptr<HistogramFactory> factory, 
        const HistogramDef& histDef)
      : IHistogramProvider()
      , m_gmTool(gmTool)
      , m_factory(factory)
      , m_histDef(new HistogramDef(histDef))
      {}

    /**
     * @brief Getter of ROOT object 
     * 
     * Each time the method is called, factory produces ROOT object based on the current lumi block.
     * Note: ROOT objects are cached at the factory. Nevertheless, it is recommended to call this method as rarely as possible.
     * 
     * @return ROOT object
     */
    TNamed* histogram() override {
      HistogramDef def = *m_histDef;

      const unsigned lumiBlock = m_gmTool->lumiBlock();
      const int historyDepth = def.kLBNHistoryDepth;
      if (historyDepth<=0) {
        throw HistogramException("Histogram >"+ def.path + "< has invalid kLBNHistoryDepth.");
      }
      const unsigned lumiPage = lumiBlock/historyDepth;
      const unsigned minLumi = lumiPage * historyDepth;
      const unsigned maxLumi = minLumi + historyDepth - 1;

      if ( historyDepth > 1 )
	def.alias = def.alias + "_LB" + std::to_string(minLumi) + "_" + std::to_string(maxLumi);
      else
	def.alias = def.alias + "_LB" + std::to_string(minLumi);

      return m_factory->create(def);
    }
  private:
    GenericMonitoringTool* const m_gmTool;
    std::shared_ptr<HistogramFactory> m_factory;
    std::shared_ptr<HistogramDef> m_histDef;
  };
}

#endif /* AthenaMonitoringKernel_HistogramFiller_LumiblockHistogramProvider_h */
