/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/  
  
#ifndef AthenaMonitoring_HistogramFiller_LumiblockHistogramProvider_h
#define AthenaMonitoring_HistogramFiller_LumiblockHistogramProvider_h

#include <memory>

#include "AthenaMonitoring/GenericMonitoringTool.h"
#include "AthenaMonitoring/HistogramDef.h"
#include "AthenaMonitoring/HistogramFiller/HistogramFactory.h"
#include "AthenaMonitoring/HistogramFiller/IHistogramProvider.h"

namespace Monitored {
  class LumiblockHistogramProvider : public IHistogramProvider {
  public:
    LumiblockHistogramProvider(GenericMonitoringTool* const gmTool, 
        std::shared_ptr<HistogramFactory> factory, 
        const HistogramDef& histDef)
      : IHistogramProvider(), m_gmTool(gmTool), m_factory(factory), m_histDef(new HistogramDef(histDef)), m_dynamicSize(parseDynamicSize(histDef)) {}

    TNamed* histogram() override {
      const unsigned lumiBlock = m_gmTool->lumiBlock();
      const unsigned lumiPage = lumiBlock/m_dynamicSize;
      const unsigned minLumi = lumiPage * m_dynamicSize;
      const unsigned maxLumi = minLumi + m_dynamicSize - 1;

      HistogramDef def = *m_histDef;
      
      def.alias = def.alias + "(" + std::to_string(minLumi) + "-" + std::to_string(maxLumi) + ")";

      return m_factory->create(def);
    }

    static unsigned parseDynamicSize(const HistogramDef& histDef) {
      const std::string sizeKey = "kDynamicSize=";
      const std::size_t sizeKeyPosition = histDef.opt.find(sizeKey);

      if (sizeKeyPosition == std::string::npos) {
        throw HistogramException("Lumiblock histogram >"+ histDef.path + "< NOT define kDynamicSize");
      }

      const std::size_t sizeStartPosition = sizeKeyPosition + sizeKey.length();
      const std::size_t sizeStopPosition = histDef.opt.find_first_not_of("1234567890", sizeStartPosition);
      const std::size_t sizeLength = sizeStopPosition - sizeStartPosition;

      if (sizeLength == 0) {
        throw HistogramException("Lumiblock histogram >"+ histDef.path + "< NOT define valid kDynamicSize");
      }

      const std::string sizeStrValue = histDef.opt.substr(sizeStartPosition, sizeLength);

      return std::stoul(sizeStrValue);
    }
  private:
    GenericMonitoringTool* const m_gmTool;
    std::shared_ptr<HistogramFactory> m_factory;
    std::shared_ptr<HistogramDef> m_histDef;
    unsigned m_dynamicSize;
  };
}

#endif /* AthenaMonitoring_HistogramFiller_LumiblockHistogramProvider_h */