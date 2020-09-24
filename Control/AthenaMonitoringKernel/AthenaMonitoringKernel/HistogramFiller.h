/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AthenaMonitoringKernel_HistogramFiller_h
#define AthenaMonitoringKernel_HistogramFiller_h

#include <functional>
#include <memory>
#include <vector>
#include "CxxUtils/AthUnlikelyMacros.h"

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
        m_histogramProvider(hf.m_histogramProvider) {}
    /**
     * @brief Move constructor
     */
    HistogramFiller(HistogramFiller&&) = default;

    /**
     * @brief Virtual destructor
     */
    virtual ~HistogramFiller() {}


    /**
     * @brief helper class to pass variables to fillers
     */
    struct VariablesPack {
      VariablesPack() {}
      VariablesPack( const std::initializer_list<const Monitored::IMonitoredVariable*>&  v)
	:var( v ) {}

      /**
       * @brief number of variables in the pack ( not counting the weight and mask )
       */
      size_t size () const {
	return var.size() - std::count( var.begin(), var.end(), nullptr );
      }

      /**
       * @brief removes all monitored variables
       */
      inline void reset() {
	std::fill( var.begin(), var.end(), nullptr );
	cut = nullptr;
	weight = nullptr;
      }

      /**
       * @brief sets monitored variable at the index (may need to reszie vector of variables)
       */
      inline void set( unsigned index, const IMonitoredVariable* ptr ) {
	if ( ATH_UNLIKELY( var.size() <= index ) ) {
	  var.resize(index+1);
	}
	var[index] = ptr;
      }

      /**
       * @brief names of all varaibles stored
       */
      std::vector<std::string> names() const {
	std::vector<std::string> r;
	for ( const auto* v: var )
	  if ( v != nullptr )
	    r.push_back( v->name() );
	return r;
      }

      std::vector<const Monitored::IMonitoredVariable*> var = { nullptr, nullptr, nullptr, nullptr }; //!< storage for variables, default size of 4, serves all histograming uses
      const Monitored::IMonitoredVariable* weight = nullptr; //!< pointer to weight variable, typically absent
      const Monitored::IMonitoredVariable* cut = nullptr; //!< pointer to cut mask variable, typically absent
    };

    /**
     * @brief Method that actually fills the ROOT object
     * @return number of fills performed
     */
    virtual unsigned fill( const VariablesPack& ) const = 0;




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
    std::pair<size_t, std::function<bool(size_t)>> getCutMaskFunc(const Monitored::IMonitoredVariable* mask ) const {
      std::function<bool(size_t)> cutMaskValue = [mask] (size_t){ return true; }; // default is true
      size_t maskSize = 1;
      if ( mask != nullptr ) {
        maskSize = mask->size();
        if (maskSize == 1) {
          if (!mask->get(0)) {
            // globally fails cut; zero first argument is a signal that one can abort
            return std::make_pair(0, [](size_t){ return false; });
            // otherwise, default cutMaskValue is sufficient
          }
        } else {
          return std::make_pair(maskSize, [mask](size_t i){ return static_cast<bool>(mask->get(i)); });
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

  private:
    HistogramFiller& operator=(HistogramFiller const&) = delete;
  };
}
#endif /* AthenaMonitoringKernel_HistogramFiller_HistogramFiller_h */
