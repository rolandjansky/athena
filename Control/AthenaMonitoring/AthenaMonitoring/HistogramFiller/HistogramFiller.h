/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AthenaMonitoring_HistogramFiller_HistogramFiller_h
#define AthenaMonitoring_HistogramFiller_HistogramFiller_h

#include <functional>
#include <mutex>
#include <memory>
#include <vector>

#include "TH1.h"
#include "TEfficiency.h"

#include "AthenaMonitoring/IMonitoredVariable.h"
#include "AthenaMonitoring/HistogramDef.h"

#include "AthenaMonitoring/HistogramFiller/HistogramFillerFactory.h"

namespace Monitored {
  /**
   * @brief base class for fillers 
   */
  class HistogramFiller {
  public:
    HistogramFiller(TH1* hist, const HistogramDef& histDef) 
      : m_hist(hist), m_mutex(std::make_shared<std::mutex>()), m_histDef(new HistogramDef(histDef)) {}
    HistogramFiller(TEfficiency* eff, const HistogramDef& histDef) 
      : m_eff(eff), m_mutex(std::make_shared<std::mutex>()), m_histDef(new HistogramDef(histDef)) {}
    HistogramFiller(const HistogramFiller& hf) 
      : m_hist(hf.m_hist)
      , m_eff(hf.m_eff)
      , m_mutex(hf.m_mutex)
      , m_histDef(hf.m_histDef) {}
    HistogramFiller(HistogramFiller&&) = default;
  
    virtual ~HistogramFiller() {}
    virtual unsigned fill() = 0;
    virtual HistogramFiller* clone() = 0;
  
    void setMonitoredVariables(std::vector<std::reference_wrapper<Monitored::IMonitoredVariable>> monitoredVariables) {
      m_monVariables = monitoredVariables;
    }
  
    std::vector<std::string> histogramVariablesNames() {
      return m_histDef->name;
    }
    
  protected:
    virtual TH1* histogram() = 0;
  
    TH1* m_hist;
    TEfficiency* m_eff;
    std::shared_ptr<std::mutex> m_mutex;
    std::shared_ptr<HistogramDef> m_histDef;
    std::vector<std::reference_wrapper<Monitored::IMonitoredVariable>> m_monVariables;
    
  private:
    HistogramFiller& operator=(HistogramFiller const&) = delete;
  };
}
#endif /* AthenaMonitoring_HistogramFiller_HistogramFiller_h */
