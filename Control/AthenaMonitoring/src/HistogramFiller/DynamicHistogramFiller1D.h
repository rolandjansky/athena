/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/  
  
#ifndef AthenaMonitoring_HistogramFiller_DynamicHistogramFiller1D_h
#define AthenaMonitoring_HistogramFiller_DynamicHistogramFiller1D_h

#include <memory>

#include "TH1.h"

#include "AthenaMonitoring/GenericMonitoringTool.h"
#include "AthenaMonitoring/HistogramFiller/HistogramFactory.h"
#include "AthenaMonitoring/HistogramFiller/HistogramFiller.h"

namespace Monitored {
  /**
   * @brief Filler for dynamic 1D histograms
   */
  class DynamicHistogramFiller1D : public HistogramFiller {
  public: 
    DynamicHistogramFiller1D(GenericMonitoringTool* const gmTool, 
        std::shared_ptr<HistogramFactory> factory, 
        const HistogramDef& histDef)
      : HistogramFiller(histDef), m_gmTool(gmTool), m_factory(factory), m_dynamicSize(parseDynamicSize(histDef)) {}

    DynamicHistogramFiller1D(const DynamicHistogramFiller1D& hf) 
      : HistogramFiller(hf), m_gmTool(hf.m_gmTool), m_factory(hf.m_factory), m_dynamicSize(hf.m_dynamicSize) {}
    
    DynamicHistogramFiller1D* clone() override { return new DynamicHistogramFiller1D(*this); }

    virtual unsigned fill() override {
      if (m_monVariables.size() != 1) {
        return 0;
      }

      auto hist = histogram();
      auto valuesVector = m_monVariables[0].get().getVectorRepresentation();
      std::lock_guard<std::mutex> lock(*(this->m_mutex));

      if (!hist) {
        return 0;
      }

      for (auto value : valuesVector) {
        hist->Fill(value);
      }

      return std::size(valuesVector);
    } 
  protected:
    TH1* histogram() {
      const unsigned lumiBlock = m_gmTool->lumiBlock();
      const unsigned lumiPage = lumiBlock/m_dynamicSize;
      const unsigned minLumi = lumiPage * m_dynamicSize;
      const unsigned maxLumi = minLumi + m_dynamicSize - 1;

      HistogramDef def = *m_histDef;
      
      def.alias = def.alias + "(" + std::to_string(minLumi) + "-" + std::to_string(maxLumi) + ")";

      return static_cast<TH1*>(m_factory->create(def));
    }

    static unsigned parseDynamicSize(const HistogramDef& histDef) {
      const std::string sizeKey = "kDynamicSize=";
      const std::size_t sizeKeyPosition = histDef.opt.find(sizeKey);

      if (sizeKeyPosition == std::string::npos) {
        throw HistogramException("Dynamic histogram >"+ histDef.path + "< NOT define kDynamicSize");
      }

      const std::size_t sizeStartPosition = sizeKeyPosition + sizeKey.length();
      const std::size_t sizeStopPosition = histDef.opt.find_first_not_of("1234567890", sizeStartPosition);
      const std::size_t sizeLength = sizeStopPosition - sizeStartPosition;

      if (sizeLength == 0) {
        throw HistogramException("Dynamic histogram >"+ histDef.path + "< NOT define valid kDynamicSize");
      }

      const std::string sizeStrValue = histDef.opt.substr(sizeStartPosition, sizeLength);

      return std::stoul(sizeStrValue);
    }

    GenericMonitoringTool* const m_gmTool;
    std::shared_ptr<HistogramFactory> m_factory;
    unsigned m_dynamicSize;
  };
}

#endif /* AthenaMonitoring_HistogramFiller_DynamicHistogramFiller1D_h */
