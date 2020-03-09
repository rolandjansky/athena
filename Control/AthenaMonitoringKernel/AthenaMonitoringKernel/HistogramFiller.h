/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AthenaMonitoringKernel_HistogramFiller_h
#define AthenaMonitoringKernel_HistogramFiller_h

#include <functional>
#include <mutex>
#include <memory>
#include <vector>

#include "AthenaMonitoringKernel/HistogramDef.h"
#include "AthenaMonitoringKernel/IHistogramProvider.h"
#include "AthenaMonitoringKernel/IMonitoredVariable.h"

namespace Monitored {
  /**
   * @brief Base class for all histogram fillers
   */
  class HistogramFiller {
  public:
    /**
     * @brief Default constructor
     *
     * @param histDef Histogram definition of ROOT object
     */
    HistogramFiller(const HistogramDef& histDef, std::shared_ptr<IHistogramProvider> histogramProvider)
      : m_mutex(std::make_shared<std::mutex>()),
        m_histDef(new HistogramDef(histDef)),
        m_histogramProvider(histogramProvider),
        m_monWeight(nullptr),
        m_monCutMask(nullptr) {}
    /**
     * @brief Copy constructor
     *
     * @param hf Other HistogramFiller
     */
    HistogramFiller(const HistogramFiller& hf)
      : m_mutex(hf.m_mutex),
        m_histDef(hf.m_histDef),
        m_histogramProvider(hf.m_histogramProvider),
        m_monWeight(hf.m_monWeight),
        m_monCutMask(hf.m_monCutMask) {}
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
     * @brief clone filler for actual filling
     * Note that this operation is very chip as the this class is effectively a flyweight
     */
    virtual HistogramFiller* clone() const = 0;


    void setMonitoredVariables(std::vector<std::reference_wrapper<Monitored::IMonitoredVariable>>&& monitoredVariables) {
      m_monVariables = std::move(monitoredVariables);
    }

    /**
     * @brief Stores histogram weight
     * @param monitoredWeight weight to use
     */
    void setMonitoredWeight(Monitored::IMonitoredVariable* monitoredWeight) {
      m_monWeight = monitoredWeight;
    }

    void setMonitoredCutMask(Monitored::IMonitoredVariable* monitoredCutMask) {
      m_monCutMask = monitoredCutMask;
    }

    const std::vector<std::string>& histogramVariablesNames() const {
      return m_histDef->name;
    }

    const std::string& histogramWeightName() const {
      return m_histDef->weight;
    }

    const std::string& histogramCutMaskName() const {
      return m_histDef->cutMask;
    }
    
  protected:
    template <class H>
    H* histogram() {
      return static_cast<H*>(m_histogramProvider->histogram());
    }

    // convenience function to provide a function that interprets the cutmask
    std::pair<size_t, std::function<bool(size_t)>> getCutMaskFunc() {
      std::function<bool(size_t)> cutMaskValue = [] (size_t){ return true; }; // default is true
      size_t maskSize = 1;
      const std::vector<double> cutMaskVector{m_monCutMask ? m_monCutMask->getVectorRepresentation() : std::vector<double>{}};
      if ( m_monCutMask != nullptr ) {
        maskSize = cutMaskVector.size();
        if (maskSize == 1) {
          if (!cutMaskVector[0]) {
            // globally fails cut; zero first argument is a signal that one can abort
            return std::make_pair(0, [](size_t){ return false; });
            // otherwise, default cutMaskValue is sufficient
          }
        } else {
          return std::make_pair(maskSize, [cutMaskVector=std::move(cutMaskVector)](size_t i){ return bool(cutMaskVector[i]); });
        }
      }
      return std::make_pair(maskSize, cutMaskValue);
    }

    std::shared_ptr<std::mutex> m_mutex;
    std::shared_ptr<HistogramDef> m_histDef;
    std::shared_ptr<IHistogramProvider> m_histogramProvider;
    std::vector<std::reference_wrapper<Monitored::IMonitoredVariable>> m_monVariables;
    Monitored::IMonitoredVariable* m_monWeight; // bare pointer instead of reference as it can be null
    Monitored::IMonitoredVariable* m_monCutMask;
    
  private:
    HistogramFiller& operator=(HistogramFiller const&) = delete;
  };
}
#endif /* AthenaMonitoringKernel_HistogramFiller_HistogramFiller_h */
