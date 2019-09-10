/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/  
  
#ifndef AthenaMonitoring_HistogramFiller_LumiblockHistogramProvider_h
#define AthenaMonitoring_HistogramFiller_LumiblockHistogramProvider_h

#include <memory>

#include "AthenaMonitoring/GenericMonitoringTool.h"
#include "AthenaMonitoring/HistogramDef.h"
#include "AthenaMonitoring/IHistogramProvider.h"

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
      : IHistogramProvider(), m_gmTool(gmTool), m_factory(factory), m_histDef(new HistogramDef(histDef)), m_historyDepth(parseHistoryDepth(histDef)) {}

    /**
     * @brief Getter of ROOT object 
     * 
     * Each time the method is called, factory produces ROOT object based on the current lumi block.
     * Note: ROOT objects are cached at the factory. Nevertheless, it is recommended to call this method as rarely as possible.
     * 
     * @return ROOT object
     */
    TNamed* histogram() override {
      const unsigned lumiBlock = m_gmTool->lumiBlock();
      const unsigned lumiPage = lumiBlock/m_historyDepth;
      const unsigned minLumi = lumiPage * m_historyDepth;
      const unsigned maxLumi = minLumi + m_historyDepth - 1;

      HistogramDef def = *m_histDef;
      
      def.alias = def.alias + "(" + std::to_string(minLumi) + "-" + std::to_string(maxLumi) + ")";

      return m_factory->create(def);
    }
  private:
    /**
     * @brief Parser for kLBNHistoryDepth option
     * 
     * kLBNHistoryDepth should be defined as unsigned integer eg. kLBNHistoryDepth=10
     * 
     * @return User defined LBN history depth
     * @throws HistogramException if kLBNHistoryDepth cannot be properly parsed
     */
    static unsigned parseHistoryDepth(const HistogramDef& histDef) {
      const std::string sizeKey = "kLBNHistoryDepth=";
      const std::size_t sizeKeyPosition = histDef.opt.find(sizeKey);

      if (sizeKeyPosition == std::string::npos) {
        throw HistogramException("Lumiblock histogram >"+ histDef.path + "< NOT define kLBNHistoryDepth");
      }

      const std::size_t sizeStartPosition = sizeKeyPosition + sizeKey.length();
      const std::size_t sizeStopPosition = histDef.opt.find_first_not_of("1234567890", sizeStartPosition);
      const std::size_t sizeLength = sizeStopPosition - sizeStartPosition;

      if (sizeLength == 0) {
        throw HistogramException("Lumiblock histogram >"+ histDef.path + "< NOT define valid kLBNHistoryDepth");
      }

      const std::string sizeStrValue = histDef.opt.substr(sizeStartPosition, sizeLength);

      return std::stoul(sizeStrValue);
    }

    GenericMonitoringTool* const m_gmTool;
    std::shared_ptr<HistogramFactory> m_factory;
    std::shared_ptr<HistogramDef> m_histDef;
    unsigned m_historyDepth;
  };
}

#endif /* AthenaMonitoring_HistogramFiller_LumiblockHistogramProvider_h */