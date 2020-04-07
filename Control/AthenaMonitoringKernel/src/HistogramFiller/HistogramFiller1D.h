/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AthenaMonitoringKernel_HistogramFiller_HistogramFiller1D_h
#define AthenaMonitoringKernel_HistogramFiller_HistogramFiller1D_h

#include "TH1.h"

#include "AthenaMonitoringKernel/HistogramFiller.h"
#include "HistogramFillerUtils.h"

#include "CxxUtils/AthUnlikelyMacros.h"
#include "GaudiKernel/MsgStream.h"

namespace Monitored {

  /**
   * @brief Filler for plain 1D histograms
   */
  class HistogramFiller1D : public HistogramFiller {
  public:
    HistogramFiller1D(const HistogramDef& definition, std::shared_ptr<IHistogramProvider> provider)
      : HistogramFiller(definition, provider) {
    }

    virtual HistogramFiller1D* clone() const override {
      return new HistogramFiller1D( *this );
    }

    virtual unsigned fill() override {
      if (m_monVariables.size() != 1) { return 0; }
      const IMonitoredVariable& var = m_monVariables.at(0).get();

      std::function<bool(size_t)> cutMaskAccessor;
      if (m_monCutMask) {
        // handling of the cutmask
        auto cutMaskValuePair = getCutMaskFunc();
        if (cutMaskValuePair.first == 0) { return 0; }
        if (ATH_UNLIKELY(cutMaskValuePair.first > 1 && cutMaskValuePair.first != var.size())) {
          MsgStream log(Athena::getMessageSvc(), "HistogramFiller1D");
          log << MSG::ERROR << "CutMask does not match the size of plotted variable: "
              << cutMaskValuePair.first << " " << var.size() << endmsg;
        }
        cutMaskAccessor = cutMaskValuePair.second;
      }

      if (m_monWeight) {
        const std::vector<double> weightVector{m_monWeight->getVectorRepresentation()};
        auto weightAccessor = [&](size_t i){ return weightVector[i]; };

        if (ATH_UNLIKELY(weightVector.size() != var.size())) {
          MsgStream log(Athena::getMessageSvc(), "HistogramFiller1D");
          log << MSG::ERROR << "Weight does not match the size of plotted variable: "
              << weightVector.size() << " " << var.size() << endmsg;
        }
        // Need to fill here while weightVector is still in scope
        if (not m_monCutMask) return fill(var, weightAccessor, detail::noCut);
        else                  return fill(var, weightAccessor, cutMaskAccessor);
      }

      if (not m_monCutMask) return fill(var, detail::noWeight, detail::noCut);
      else                  return fill(var, detail::noWeight, cutMaskAccessor);
    }

  protected:
    /**
     * Helper to extract double/string representation from MonitoredVariable.
     * Forward remaining template parameters to fill function below.
     */
    template<typename ...Fs>
    unsigned fill(const IMonitoredVariable& var , Fs... f) {
      if (not var.hasStringRepresentation()) {
        const auto valuesVector{var.getVectorRepresentation()};
        return fill( std::size(valuesVector), [&](size_t i){ return valuesVector[i]; }, f...);
      } else {
        const auto valuesVector{var.getStringVectorRepresentation()};
        return fill( std::size(valuesVector), [&](size_t i){ return valuesVector[i].c_str(); }, f...);
      }
    }

    /**
     * Helper to fill both double and string-valued variables
     *
     * @param n  size of the vector to fill
     * @param v  accessor for values in vector
     * @param w  weight accessor
     * @param c  cut mask accessor
     *
     * If weight and/or cut are not needed use a lambda expression (instead of std::function)
     * as this will allow the compiler to produce more optimal code.
     */
    template<typename V, typename W, typename C>
    unsigned fill( size_t n, V v, W w, C c) {

      std::scoped_lock lock(*m_mutex);
      int fills = 0;
      TH1* histogram = this->histogram<TH1>();
      for ( size_t i = 0; i < n; ++i ) {
        if (c(i)) {
          const auto& x = v(i);
          // In case re-binning occurs need to take the OH lock for online (no-op offline)
          if ( ATH_UNLIKELY(histogram->GetXaxis()->CanExtend() and
                            detail::fillWillRebinHistogram(histogram->GetXaxis(), x)) ) {
            oh_scoped_lock_histogram lock;
            fills += histogram->Fill( x, w(i) );
          }
          else fills += histogram->Fill( x, w(i) );
        }
      }
      return fills;
    }
  };
}

#endif /* AthenaMonitoringKernel_HistogramFiller_HistogramFiller1D_h */
