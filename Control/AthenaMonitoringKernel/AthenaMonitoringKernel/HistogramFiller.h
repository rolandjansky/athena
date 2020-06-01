/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AthenaMonitoringKernel_HistogramFiller_h
#define AthenaMonitoringKernel_HistogramFiller_h

#include <functional>
#include <memory>
#include <vector>

#include "AthenaMonitoringKernel/HistogramDef.h"
#include "AthenaMonitoringKernel/IHistogramProvider.h"
#include "AthenaMonitoringKernel/IMonitoredVariable.h"

namespace Monitored {

  // Forward declare generic histogram filler (see HistogramFillerUtils.h)
  namespace detail {
    template<typename H, typename W, typename M, typename ...Ms>
    void doFill(H* hist, W weight, size_t i, const M& m1, const Ms&... m);
    template<typename W>
    void doFill(TProfile*, W, size_t, const double&, const char* const&);
    template<typename W>
    void doFill(TProfile*, W, size_t, const char* const&, const char* const&);
  }

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
      : m_histDef(new HistogramDef(histDef)),
        m_histogramProvider(histogramProvider) {}
    /**
     * @brief Copy constructor
     *
     * @param hf Other HistogramFiller
     */
    HistogramFiller(const HistogramFiller& hf)
      : m_histDef(hf.m_histDef),
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
     * @return number of fills performed
     */
    virtual unsigned fill() const = 0;


    /**
     * @brief clone filler for actual filling
     * Note that this operation is very cheap as the this class is effectively a flyweight
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
    H* histogram() const {
      return static_cast<H*>(m_histogramProvider->histogram());
    }

    // convenience function to provide a function that interprets the cutmask
    std::pair<size_t, std::function<bool(size_t)>> getCutMaskFunc() const {
      std::function<bool(size_t)> cutMaskValue = [] (size_t){ return true; }; // default is true
      size_t maskSize = 1;
      if ( m_monCutMask != nullptr ) {
        maskSize = m_monCutMask->size();
        if (maskSize == 1) {
          if (!m_monCutMask->get(0)) {
            // globally fails cut; zero first argument is a signal that one can abort
            return std::make_pair(0, [](size_t){ return false; });
            // otherwise, default cutMaskValue is sufficient
          }
        } else {
          return std::make_pair(maskSize, [this](size_t i){ return static_cast<bool>(m_monCutMask->get(i)); });
        }
      }
      return std::make_pair(maskSize, cutMaskValue);
    }

    /**
     * Fill histogram from IMonitoredVariable.
     *
     * Supports arbitrary dimensions and double/string representation.
     *
     * @tparam H        histogram type (TH1, TH2, ...)
     * @param  weight   weight accessor (use detail::noWeight if not needed)
     * @param  cut      cut mask accessor (use detail::noCut if not needed)
     * @param  m...     IMonitoredVariable list to fill from
     */
    template<class H, typename W, typename C, typename ...Ms>
    unsigned fill(W weight, C cut, const Ms&... m) const {
      auto hist = this->histogram<H>();

      size_t i = 0;
      for (; i < std::max({m.size()...}); ++i ) {
        if ( cut(i) ) {
          detail::doFill(hist, weight, i, m...);
        }
      }
      return i;
    }

    std::shared_ptr<HistogramDef> m_histDef;
    std::shared_ptr<IHistogramProvider> m_histogramProvider;
    std::vector<std::reference_wrapper<Monitored::IMonitoredVariable>> m_monVariables;
    Monitored::IMonitoredVariable* m_monWeight{nullptr}; // bare pointer instead of reference as it can be null
    Monitored::IMonitoredVariable* m_monCutMask{nullptr};
    
  private:
    HistogramFiller& operator=(HistogramFiller const&) = delete;
  };
}
#endif /* AthenaMonitoringKernel_HistogramFiller_HistogramFiller_h */
