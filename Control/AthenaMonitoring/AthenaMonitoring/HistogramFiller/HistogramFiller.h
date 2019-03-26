/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AthenaMonitoring_HistogramFiller_HistogramFiller_h
#define AthenaMonitoring_HistogramFiller_HistogramFiller_h

#include <functional>
#include <mutex>
#include <memory>
#include <vector>

#include "TNamed.h"
#include "TEfficiency.h"

#include "AthenaMonitoring/IMonitoredVariable.h"
#include "AthenaMonitoring/HistogramDef.h"

namespace Monitored {
  /**
   * @brief Base class for all histogram fillers 
   */
  class HistogramFiller {
  public:
    /**
     * @brief Default constructor
     * 
     * @param hist ROOT object to fill
     * @param histDef Histogram definition of ROOT object
     */
    HistogramFiller(TNamed* hist, const HistogramDef& histDef) 
      : m_hist(hist), 
        m_mutex(std::make_shared<std::mutex>()), 
        m_histDef(new HistogramDef(histDef)) {}
    /**
     * @brief Copy constructor
     * 
     * @param hf Other HistogramFiller
     */
    HistogramFiller(const HistogramFiller& hf) 
      : m_hist(hf.m_hist)
      , m_mutex(hf.m_mutex)
      , m_histDef(hf.m_histDef) {}
    /**
     * @brief Move constructor
     */
    HistogramFiller(HistogramFiller&&) = default;
  
    /**
     * @brief Virtual destructor
     */
    virtual ~HistogramFiller() {}
    /**
     * @brief Method that actually fills the ROOT object
     */
    virtual unsigned fill() = 0;
    /**
     * @brief Clones the filler
     * 
     * @return Clone of the current instance
     */
    virtual HistogramFiller* clone() = 0;
  
    void setMonitoredVariables(std::vector<std::reference_wrapper<Monitored::IMonitoredVariable>> monitoredVariables) {
      m_monVariables = monitoredVariables;
    }
  
    std::vector<std::string> histogramVariablesNames() {
      return m_histDef->name;
    }
    
  protected:
    /**
     * @brief Getter for associated ROOT object
     * 
     * @return Instance of ROOT object
     */
    virtual TNamed* histogram() = 0;
  
    TNamed* m_hist;
    std::shared_ptr<std::mutex> m_mutex;
    std::shared_ptr<HistogramDef> m_histDef;
    std::vector<std::reference_wrapper<Monitored::IMonitoredVariable>> m_monVariables;
    
  private:
    HistogramFiller& operator=(HistogramFiller const&) = delete;
  };
}
#endif /* AthenaMonitoring_HistogramFiller_HistogramFiller_h */
