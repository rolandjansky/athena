/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AthenaMonitoringKernel_HistogramFiller_HistogramFillerRebinable1D_h
#define AthenaMonitoringKernel_HistogramFiller_HistogramFillerRebinable1D_h

#include "AthenaMonitoringKernel/OHLockedHist.h"
#include "HistogramFiller1D.h"
#include "HistogramFiller2D.h"
#include "HistogramFillerUtils.h"

#include <cmath>

namespace Monitored {

  /**
   * @brief Mixin class for rebinning of histogram axis
   *
   * @tparam BASE  HistogramFiller base class
   * @tparam AXIS  Histogram axis that should be made rebinable
   */
  template<typename BASE, Monitored::Axis AXIS>
  class HistogramFillerRebinableAxis : public BASE {
  public:

    HistogramFillerRebinableAxis(const HistogramDef& definition, std::shared_ptr<IHistogramProvider> provider)
      : BASE(definition, provider) {
      if (definition.kAddBinsDynamically) {
        m_rebinMode = RebinMode::AddBins;
      } else {
        m_rebinMode = RebinMode::ExtendAxes;
      }
    }

    virtual HistogramFillerRebinableAxis* clone() const override {
      return new HistogramFillerRebinableAxis( *this );
    }

    virtual unsigned fill() const override {
      if (AXIS >= this->m_monVariables.size()) { return 0; }
      if (this->m_monVariables[AXIS].get().size()==0) { return 0; }

      { // scope for mutex
        const auto valuesVector = this->m_monVariables[AXIS].get().getVectorRepresentation();

        TH1* hist = this->template histogram<TH1>();
        TAxis* axis = detail::getAxis<TH1,AXIS>(hist);
        if (m_rebinMode == RebinMode::AddBins) {
          const auto max = std::max_element(begin(valuesVector), end(valuesVector));
          if (Monitored::detail::shouldRebinHistogram(axis, *max)) {
            Monitored::detail::rebinHistogram<AXIS>(hist, *max);
          }
        } else {
          hist->SetCanExtend(detail::axis_bit[AXIS]);
          const auto [min,max] = std::minmax_element(begin(valuesVector), end(valuesVector));
          // ExtendAxis is an extremely expensive operation; only call if necessary
          if (*max >= axis->GetXmax()) hist->ExtendAxis(*max, axis);
          if (*min < axis->GetXmin()) hist->ExtendAxis(*min, axis);
        }
      }
      return BASE::fill();
    }

  private:
    enum class RebinMode { AddBins, ExtendAxes };
    RebinMode m_rebinMode;
  };

  /// Rebinable 1D histogram
  typedef HistogramFillerRebinableAxis<HistogramFiller1D, Axis::X> HistogramFillerRebinable1D;
  /// Rebinable 2D histogram (x-axis only)
  typedef HistogramFillerRebinableAxis<HistogramFiller2D, Axis::X> HistogramFillerRebinable2DX;
  /// Rebinable 2D histogram (y-axis only)
  typedef HistogramFillerRebinableAxis<HistogramFiller2D, Axis::Y> HistogramFillerRebinable2DY;
  /// Rebinable 2D histogram (both axes)
  typedef HistogramFillerRebinableAxis<HistogramFillerRebinable2DX, Axis::Y> HistogramFillerRebinable2D;
}

#endif /* AthenaMonitoringKernel_HistogramFiller_HistogramFillerRebinable1D_h */
